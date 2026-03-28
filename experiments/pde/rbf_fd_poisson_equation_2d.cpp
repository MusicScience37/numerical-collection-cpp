/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file
 * \brief Experiments to solve the Poisson equation in 2D with RBF-FD method.
 */
#include <cmath>
#include <string>
#include <tuple>

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCore>
#include <plotly_plotter/color_scales.h>
#include <plotly_plotter/eigen.h>
#include <plotly_plotter/figure.h>
#include <plotly_plotter/write_html.h>
#include <toml++/toml.h>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataWriter.h>

#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_assembler.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/rbf/rbf_interpolator.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"

using variable_type = Eigen::Vector2d;
using sparse_matrix_type = Eigen::SparseMatrix<double,
    Eigen::RowMajor>;  // BiCGstab works better with row-major format.

static auto test_function(const variable_type& variable) -> double {
    return (variable.array() * num_collect::base::pi<double>).sin().prod();
}

static auto test_function_laplacian(const variable_type& variable) -> double {
    constexpr double coeff =
        -2.0 * num_collect::base::pi<double> * num_collect::base::pi<double>;
    return coeff *
        (variable.array() * num_collect::base::pi<double>).sin().prod();
}

/*!
 * \brief Generate nodes.
 *
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 * \param[in] num_boundary_nodes_per_edge Number of nodes on each edge of the
 * boundary.
 * \return Generated nodes. First num_interior_nodes nodes are in the interior,
 * and the rest are on the boundary in the counter-clockwise order.
 */
static auto generate_nodes(num_collect::index_type num_interior_nodes,
    num_collect::index_type num_boundary_nodes_per_edge)
    -> num_collect::util::vector<variable_type> {
    const auto interior_nodes =
        num_collect::rbf::generate_halton_nodes<double, 2>(num_interior_nodes);
    const auto boundary_nodes =
        num_collect::util::generate_rectangle_boundary_nodes(
            Eigen::Vector2d(0.0, 0.0), Eigen::Vector2d(1.0, 1.0),
            num_boundary_nodes_per_edge);
    num_collect::util::vector<variable_type> nodes;
    nodes.reserve(interior_nodes.size() + boundary_nodes.size());
    nodes.insert(nodes.end(), interior_nodes.begin(), interior_nodes.end());
    nodes.insert(nodes.end(), boundary_nodes.begin(), boundary_nodes.end());
    return nodes;
}

/*!
 * \brief Assemble the system of the Poisson equation.
 *
 * \param[in] nodes Nodes. Generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary in the counter-clockwise order.
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 * \param[in] polynomial_order Order of polynomials used in RBF-FD. -1 for no
 * polynomials.
 * \param[in] num_neighbors Number of neighbors used in RBF-FD.
 * \param[in] length_parameter_scale Scale of length parameters in RBF-FD.
 * \return Coefficient matrix and right-hand side vector.
 */
static auto assemble_system(
    num_collect::util::vector_view<const variable_type> nodes,
    num_collect::index_type num_interior_nodes, int polynomial_order,
    num_collect::index_type num_neighbors, double length_parameter_scale)
    -> std::tuple<sparse_matrix_type, Eigen::VectorXd> {
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<variable_type>;

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start assembly of the system.");

    num_collect::util::vector<Eigen::Triplet<double>> triplets;
    Eigen::VectorXd right_vec(nodes.size());

    const num_collect::util::nearest_neighbor_searcher<variable_type>
        column_variables_nearest_neighbor_searcher(nodes);

    // Interior nodes.
    const auto interior_nodes = nodes.first(num_interior_nodes);
    if (polynomial_order < 0) {
        // RBF-FD without polynomials.
        // Without polynomials, no RBF I tested worked.
        // Gaussian RBF tends to give very large errors with many points.
        // Thin plate spline RBF won't work without polynomials.
        // TODO Research why Gaussian RBF is not working.
        using assembler_type =
            num_collect::rbf::rbf_fd_assembler<variable_type>;
        assembler_type assembler;
        assembler.num_neighbors(num_neighbors);
        assembler.length_parameter_scale(length_parameter_scale);
        constexpr num_collect::index_type row_offset = 0;
        constexpr num_collect::index_type column_offset = 0;
        assembler.compute_rows<operator_type>(interior_nodes, nodes,
            column_variables_nearest_neighbor_searcher, triplets, row_offset,
            column_offset);
        for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
            right_vec(i) = test_function_laplacian(interior_nodes[i]);
        }
    } else {
        // RBF-FD with polynomials.
        // Gaussian RBF did not work well, but thin plate spline RBF worked
        // well.
        // TODO Research why Gaussian RBF is not working.
        using assembler_type =
            num_collect::rbf::phs_rbf_fd_polynomial_assembler<variable_type>;
        assembler_type assembler(polynomial_order);
        assembler.num_neighbors(num_neighbors);
        assembler.length_parameter_scale(length_parameter_scale);
        constexpr num_collect::index_type row_offset = 0;
        constexpr num_collect::index_type column_offset = 0;
        assembler.compute_rows<operator_type>(interior_nodes, nodes,
            column_variables_nearest_neighbor_searcher, triplets, row_offset,
            column_offset);
        for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
            right_vec(i) = test_function_laplacian(interior_nodes[i]);
        }
    }

    // Boundary nodes.
    for (num_collect::index_type i = num_interior_nodes; i < nodes.size();
        ++i) {
        const auto index_in_triplet = static_cast<int>(i);
        triplets.emplace_back(index_in_triplet, index_in_triplet, 1.0);
        right_vec(i) = test_function(nodes[i]);
    }

    sparse_matrix_type mat(nodes.size(), nodes.size());
    mat.setFromTriplets(triplets.begin(), triplets.end());

    NUM_COLLECT_LOG_INFO(logger, "Finished assembly of the system.");

    return {mat, right_vec};
}

/*!
 * \brief Solve the system of the Poisson equation.
 *
 * \param[in] mat System matrix.
 * \param[in] right_vec Right-hand side vector.
 * \return Solution vector.
 */
static auto solve_system(const sparse_matrix_type& mat,
    const Eigen::VectorXd& right_vec) -> Eigen::VectorXd {
    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start to solve the system.");

    Eigen::BiCGSTAB<sparse_matrix_type> solver;
    solver.compute(mat);
    Eigen::VectorXd solution = solver.solve(right_vec);

    NUM_COLLECT_LOG_INFO(logger, "Finished to solve the system.");

    return solution;
}

/*!
 * \brief Evaluate the solution.
 *
 * \param[in] nodes Nodes. Generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary in the counter-clockwise order.
 * \param[in] solution Solution vector.
 * \return True values and errors.
 */
static auto evaluate_solution(
    const num_collect::util::vector_view<const variable_type>& nodes,
    const Eigen::VectorXd& solution)
    -> std::tuple<Eigen::VectorXd, Eigen::VectorXd> {
    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start to evaluate the solution.");

    Eigen::VectorXd true_values(nodes.size());
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        true_values(i) = test_function(nodes[i]);
    }
    const Eigen::VectorXd errors = (solution - true_values).cwiseAbs();
    const double max_error = errors.maxCoeff();
    const double mean_error = errors.mean();
    const double max_value = true_values.cwiseAbs().maxCoeff();
    NUM_COLLECT_LOG_INFO(logger,
        "Max error: {:.2e}, Mean error: {:.2e}, Max value: {:.2e}", max_error,
        mean_error, max_value);

    return {true_values, errors};
}

static void write_vtp_file(
    const num_collect::util::vector_view<const variable_type>& nodes,
    const Eigen::VectorXd& solution, const Eigen::VectorXd& true_values,
    const Eigen::VectorXd& errors) {
    num_collect::logging::logger logger;

    vtkNew<vtkPoints> points;
    for (const auto& node : nodes) {
        points->InsertNextPoint(node.x(), node.y(), 0.0);
    }

    vtkNew<vtkPolyData> poly_data_of_points;
    poly_data_of_points->SetPoints(points);

    vtkNew<vtkDelaunay2D> delaunay;
    delaunay->SetInputData(poly_data_of_points);
    delaunay->Update();

    vtkPolyData* poly_data = delaunay->GetOutput();

    vtkNew<vtkDoubleArray> true_values_array;
    true_values_array->SetName("True Values");
    true_values_array->SetNumberOfComponents(1);
    for (double value : true_values) {
        true_values_array->InsertNextValue(value);
    }
    poly_data->GetPointData()->AddArray(true_values_array);

    vtkNew<vtkDoubleArray> solution_array;
    solution_array->SetName("Solution");
    solution_array->SetNumberOfComponents(1);
    for (double value : solution) {
        solution_array->InsertNextValue(value);
    }
    poly_data->GetPointData()->AddArray(solution_array);

    vtkNew<vtkDoubleArray> error_array;
    error_array->SetName("Error");
    error_array->SetNumberOfComponents(1);
    for (double value : errors) {
        error_array->InsertNextValue(value);
    }
    poly_data->GetPointData()->AddArray(error_array);

    vtkNew<vtkXMLPolyDataWriter> writer;
    const std::string file_name = "rbf_fd_poisson_equation_2d.vtp";
    writer->SetFileName(file_name.c_str());
    writer->SetInputData(poly_data);
    writer->SetDataModeToBinary();
    writer->SetCompressorTypeToZLib();
    writer->SetCompressionLevel(9);
    writer->Write();

    NUM_COLLECT_LOG_INFO(logger, "Wrote {}.", file_name);
}

static auto interpolate_data(
    const num_collect::util::vector_view<const variable_type>& nodes,
    const Eigen::VectorXd& origin, const Eigen::VectorXd& x_values,
    const Eigen::VectorXd& y_values) -> Eigen::MatrixXd {
    num_collect::rbf::local_csrbf_interpolator<double(Eigen::Vector2d)>
        interpolator;
    interpolator.compute(nodes, origin);
    Eigen::MatrixXd interpolated_values(y_values.size(), x_values.size());
    for (num_collect::index_type i = 0; i < x_values.size(); ++i) {
        for (num_collect::index_type j = 0; j < y_values.size(); ++j) {
            interpolated_values(j, i) = interpolator.interpolate(
                Eigen::Vector2d(x_values(i), y_values(j)));
        }
    }
    return interpolated_values;
}

static void write_plots(
    const num_collect::util::vector_view<const variable_type>& nodes,
    const Eigen::VectorXd& solution, const Eigen::VectorXd& true_values) {
    num_collect::logging::logger logger;

    const auto resolution =
        std::max(static_cast<num_collect::index_type>(std::sqrt(nodes.size())),
            static_cast<num_collect::index_type>(100));
    const Eigen::VectorXd x_values =
        Eigen::VectorXd::LinSpaced(resolution, 0.0, 1.0);
    const Eigen::VectorXd& y_values = x_values;

    const auto interpolated_solution =
        interpolate_data(nodes, solution, x_values, y_values);
    const auto interpolated_true_values =
        interpolate_data(nodes, true_values, x_values, y_values);
    const Eigen::MatrixXd interpolated_errors =
        (interpolated_solution - interpolated_true_values).cwiseAbs();

    {
        // True and approximated solutions.
        plotly_plotter::figure figure;

        figure.layout().grid().rows(1);
        figure.layout().grid().columns(2);
        figure.layout().grid().pattern("independent");

        constexpr double annotation_x = 0.5;
        constexpr double annotation_y = 1.0;
        constexpr double annotation_y_shift = 30.0;

        auto heatmap = figure.add_heatmap();
        heatmap.z(interpolated_true_values);
        heatmap.x(x_values);
        heatmap.y(y_values);
        heatmap.xaxis("x");
        heatmap.yaxis("y");
        heatmap.color_axis("coloraxis");
        heatmap.z_smooth("best");
        auto annotation = figure.layout().add_annotation();
        annotation.x_ref("x domain");
        annotation.y_ref("y domain");
        annotation.x(annotation_x);
        annotation.y(annotation_y);
        annotation.y_shift(annotation_y_shift);
        annotation.show_arrow(false);
        annotation.align("center");
        annotation.text("True Values");
        figure.layout().yaxis().scale_anchor("x");

        heatmap = figure.add_heatmap();
        heatmap.z(interpolated_solution);
        heatmap.x(x_values);
        heatmap.y(y_values);
        heatmap.xaxis("x2");
        heatmap.yaxis("y2");
        heatmap.color_axis("coloraxis");
        heatmap.z_smooth("best");
        annotation = figure.layout().add_annotation();
        annotation.x_ref("x2 domain");
        annotation.y_ref("y2 domain");
        annotation.x(annotation_x);
        annotation.y(annotation_y);
        annotation.y_shift(annotation_y_shift);
        annotation.show_arrow(false);
        annotation.align("center");
        annotation.text("Solution");
        figure.layout().xaxis(2).matches("x");
        figure.layout().yaxis(2).matches("y");

        figure.layout().color_axis().color_scale(
            plotly_plotter::color_scales::tea());

        figure.title(
            "The Solution of the Poisson Equation in 2D with RBF-FD Method");

        const auto file_path = "rbf_fd_poisson_equation_2d_solution.html";
        plotly_plotter::write_html(file_path, figure);
        NUM_COLLECT_LOG_INFO(logger, "Wrote {}.", file_path);
    }
    {
        // Errors.
        plotly_plotter::figure figure;

        auto heatmap = figure.add_heatmap();
        heatmap.z(interpolated_errors);
        heatmap.x(x_values);
        heatmap.y(y_values);
        heatmap.color_axis("coloraxis");
        heatmap.z_smooth("best");
        figure.layout().yaxis().scale_anchor("x");

        figure.layout().color_axis().color_scale(
            plotly_plotter::color_scales::tea());

        figure.title(
            "Errors of the Solution of the Poisson Equation in 2D with RBF-FD "
            "Method");

        const auto file_path = "rbf_fd_poisson_equation_2d_errors.html";
        plotly_plotter::write_html(file_path, figure);
        NUM_COLLECT_LOG_INFO(logger, "Wrote {}.", file_path);
    }
}

auto main(int argc, const char** argv) -> int {
    std::string_view config_file_path =
        "experiments/pde/rbf_fd_poisson_equation_2d.toml";
    if (argc == 2) {
        config_file_path = argv[1];
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

    toml::table config = toml::parse_file(config_file_path);
    const auto num_interior_nodes =
        config.at_path("rbf_fd_poisson_equation_2d.num_interior_nodes")
            .value<num_collect::index_type>()
            .value();
    const auto num_boundary_nodes_per_edge =
        config.at_path("rbf_fd_poisson_equation_2d.num_boundary_nodes_per_edge")
            .value<num_collect::index_type>()
            .value();
    const auto polynomial_order =
        config.at_path("rbf_fd_poisson_equation_2d.polynomial_order")
            .value<int>()
            .value();
    const auto num_neighbors =
        config.at_path("rbf_fd_poisson_equation_2d.num_neighbors")
            .value<num_collect::index_type>()
            .value();
    const auto length_parameter_scale =
        config.at_path("rbf_fd_poisson_equation_2d.length_parameter_scale")
            .value<double>()
            .value();
    NUM_COLLECT_LOG_INFO(
        logger, "Number of interior nodes: {}", num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Number of boundary nodes per edge: {}",
        num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Polynomial order: {}", polynomial_order);
    NUM_COLLECT_LOG_INFO(logger, "Number of neighbors: {}", num_neighbors);
    NUM_COLLECT_LOG_INFO(
        logger, "Scale of length parameters: {}", length_parameter_scale);

    const auto nodes =
        generate_nodes(num_interior_nodes, num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Generated {} nodes.", nodes.size());

    const auto [mat, right_vec] = assemble_system(nodes, num_interior_nodes,
        polynomial_order, num_neighbors, length_parameter_scale);
    const auto solution = solve_system(mat, right_vec);

    const auto [true_values, errors] = evaluate_solution(nodes, solution);
    write_vtp_file(nodes, solution, true_values, errors);
    write_plots(nodes, solution, true_values);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
