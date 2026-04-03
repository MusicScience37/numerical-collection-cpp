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
 * \brief Experiments to solve 2D diffusion equation using RBF-FD method.
 */
#include <cmath>
#include <filesystem>
#include <string>
#include <string_view>

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCore>
#include <fmt/format.h>
#include <fmt/ostream.h>
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
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/rosenbrock/rodaspr_formula.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/generate_rectangle_boundary_nodes.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"

using variable_type = Eigen::Vector2d;
using sparse_matrix_type = Eigen::SparseMatrix<double,
    Eigen::RowMajor>;  // BiCGstab works better with row-major format.

static constexpr std::string_view output_directory =
    "rbf_fd_diffusion_equation_2d";

static auto test_function(const variable_type& position, double time,
    double diffusion_coefficient) -> double {
    return std::exp(-2.0 * diffusion_coefficient * num_collect::pi<double> *
               num_collect::pi<double> * time) *
        (position.array() * num_collect::pi<double>).sin().prod();
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
 * \brief Class of ODE problems written using a matrix.
 *
 * This class defines a problem as follows:
 *
 * \f[
 * \frac{d}{dt} \boldsymbol{u} = A \boldsymbol{u} + \boldsymbol{b}
 * \f]
 */
class matrix_ode_problem {
public:
    //! Type of variables.
    using variable_type = Eigen::VectorXd;

    //! Type of coefficients matrix.
    using matrix_type = sparse_matrix_type;

    //! Type of scalars.
    using scalar_type = double;

    //! Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{.diff_coeff = true};

    /*!
     * \brief Constructor.
     *
     * \param[in] variable_coefficients Coefficients for variables.
     * \param[in] constant_term Constant term.
     */
    matrix_ode_problem(const matrix_type& variable_coefficients,
        const variable_type& constant_term)
        : variable_coefficients_(variable_coefficients),
          constant_term_(constant_term) {}

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(scalar_type /*time*/, const variable_type& variable,
        num_collect::ode::evaluation_type /*evaluations*/) {
        diff_coeff_ = constant_term_;
        diff_coeff_.noalias() += variable_coefficients_ * variable;
    }

    /*!
     * \brief Get the differential coefficient.
     *
     * \return Differential coefficient.
     */
    [[nodiscard]] auto diff_coeff() const noexcept -> const variable_type& {
        return diff_coeff_;
    }

private:
    //! Coefficients for variables.
    matrix_type variable_coefficients_;

    //! Constant term.
    variable_type constant_term_;

    //! Differential coefficient.
    variable_type diff_coeff_;
};

/*!
 * \brief Assemble the system of the diffusion equation.
 *
 * \param[in] nodes Nodes. Generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary in the counter-clockwise order.
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 * \param[in] polynomial_order Order of polynomials used in RBF-FD. -1 for no
 * polynomials.
 * \param[in] num_neighbors Number of neighbors used in RBF-FD.
 * \param[in] diffusion_coefficient Diffusion coefficient.
 * \return ODE problem to solve.
 */
static auto assemble_system(
    num_collect::util::vector_view<const variable_type> nodes,
    num_collect::index_type num_interior_nodes, int polynomial_order,
    num_collect::index_type num_neighbors, double diffusion_coefficient)
    -> matrix_ode_problem {
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<variable_type>;

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Start assembly of the system.");

    num_collect::util::vector<Eigen::Triplet<double>> triplets;

    using assembler_type =
        num_collect::rbf::phs_rbf_fd_polynomial_assembler<variable_type>;
    assembler_type assembler(polynomial_order);
    assembler.num_neighbors(num_neighbors);

    const auto interior_nodes = nodes.first(num_interior_nodes);
    const num_collect::util::nearest_neighbor_searcher<variable_type>
        column_variables_nearest_neighbor_searcher(nodes);
    constexpr num_collect::index_type row_offset = 0;
    constexpr num_collect::index_type column_offset = 0;
    assembler.compute_rows<operator_type>(interior_nodes, nodes,
        column_variables_nearest_neighbor_searcher, triplets, row_offset,
        column_offset);

    sparse_matrix_type whole_coefficients(num_interior_nodes, nodes.size());
    whole_coefficients.setFromTriplets(triplets.begin(), triplets.end());
    whole_coefficients *= diffusion_coefficient;

    const sparse_matrix_type variable_coefficients =
        whole_coefficients.leftCols(num_interior_nodes);
    const Eigen::VectorXd constant_term =
        Eigen::VectorXd::Zero(num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished assembly of the system.");

    return matrix_ode_problem(variable_coefficients, constant_term);
}

/*!
 * \brief Write a VTP file for visualization in ParaView.
 *
 * \param[in] nodes Nodes. Generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary in the counter-clockwise order.
 * \param[in] file_path File path for the VTP file.
 * \param[in] solution Solution values at the nodes.
 * \param[in] true_values True values at the nodes.
 * \param[in] errors Errors at the nodes.
 */
static void write_vtp_file(
    num_collect::util::vector_view<const variable_type> nodes,
    const std::string& file_path, const Eigen::VectorXd& solution,
    const Eigen::VectorXd& true_values, const Eigen::VectorXd& errors) {
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
    writer->SetFileName(file_path.c_str());
    writer->SetInputData(poly_data);
    writer->SetDataModeToBinary();
    writer->SetCompressorTypeToZLib();
    writer->SetCompressionLevel(9);
    writer->Write();
}

/*!
 * \brief Solve the assembled system.
 *
 * \param[in] problem ODE problem to solve.
 * \param[in] diffusion_coefficient Diffusion coefficient.
 * \param[in] time_step_size Time step size.
 * \param[in] final_time Final time.
 * \param[in] nodes Nodes. Generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary in the counter-clockwise order.
 * \param[in] num_interior_nodes Number of nodes in the interior of the domain.
 */
static void solve_system(const matrix_ode_problem& problem,
    double diffusion_coefficient, double time_step_size, double final_time,
    num_collect::util::vector_view<const variable_type> nodes,
    num_collect::index_type num_interior_nodes) {
    num_collect::logging::logger logger;

    std::filesystem::create_directories(output_directory);

    num_collect::index_type time_index = 0;
    double time = 0.0;
    Eigen::VectorXd whole_variable = Eigen::VectorXd::Zero(nodes.size());
    for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
        whole_variable(i) =
            test_function(nodes[i], time, diffusion_coefficient);
    }
    Eigen::VectorXd true_values = whole_variable;
    Eigen::VectorXd errors = Eigen::VectorXd::Zero(nodes.size());
    std::string file_path =
        fmt::format("{}/rbf_fd_diffusion_equation_2d_{:04d}.vtp",
            output_directory, time_index);
    write_vtp_file(nodes, file_path, whole_variable, true_values, errors);

    num_collect::util::vector<double> time_list;
    time_list.push_back(time);

    using solver_type =
        num_collect::ode::rosenbrock::rodaspr_solver<matrix_ode_problem>;
    solver_type solver(problem);
    const Eigen::VectorXd initial_variable =
        whole_variable.head(num_interior_nodes);
    solver.init(time, initial_variable);
    while (time < final_time) {
        double next_time = time + time_step_size;
        constexpr double time_threshold = 1e-4;
        if (next_time > final_time - time_threshold) {
            next_time = final_time;
        }
        solver.solve_till(next_time);

        time = solver.time();
        time_list.push_back(time);
        ++time_index;

        whole_variable.head(num_interior_nodes) = solver.variable();
        for (num_collect::index_type i = 0; i < num_interior_nodes; ++i) {
            true_values(i) =
                test_function(nodes[i], time, diffusion_coefficient);
        }
        errors = (whole_variable - true_values).cwiseAbs();

        const double max_error = errors.maxCoeff();
        const double mean_error = errors.head(num_interior_nodes).mean();
        const double max_value = true_values.cwiseAbs().maxCoeff();
        NUM_COLLECT_LOG_INFO(logger,
            "Time: {:.2e}, Max error: {:.2e}, Mean error: {:.2e}, Max value: "
            "{:.2e}",
            time, max_error, mean_error, max_value);

        file_path = fmt::format("{}/rbf_fd_diffusion_equation_2d_{:04d}.vtp",
            output_directory, time_index);
        write_vtp_file(nodes, file_path, whole_variable, true_values, errors);
    }

    const std::string para_view_data_file_path =
        fmt::format("{}/rbf_fd_diffusion_equation_2d.pvd", output_directory);
    std::ofstream para_view_data_file(para_view_data_file_path);
    fmt::print(para_view_data_file, R"(<?xml version="1.0"?>
<VTKFile type="Collection" version="0.1" byte_order="LittleEndian">
  <Collection>
)");
    for (num_collect::index_type i = 0; i < time_list.size(); ++i) {
        fmt::print(para_view_data_file,
            R"(    <DataSet timestep="{:.2e}" file="rbf_fd_diffusion_equation_2d_{:04d}.vtp"/>
)",
            time_list[i], i);
    }
    fmt::print(para_view_data_file,
        R"(  </Collection>
</VTKFile>)");
}

auto main(int argc, const char** argv) -> int {
    std::string_view config_file_path =
        "experiments/pde/rbf_fd_diffusion_equation_2d.toml";
    if (argc == 2) {
        config_file_path = argv[1];
    }
    num_collect::logging::load_logging_config_file(config_file_path);
    num_collect::logging::logger logger;

    toml::table config = toml::parse_file(config_file_path);
    const auto num_interior_nodes =
        config.at_path("rbf_fd_diffusion_equation_2d.num_interior_nodes")
            .value<num_collect::index_type>()
            .value();
    const auto num_boundary_nodes_per_edge =
        config
            .at_path("rbf_fd_diffusion_equation_2d.num_boundary_nodes_per_edge")
            .value<num_collect::index_type>()
            .value();
    const auto polynomial_order =
        config.at_path("rbf_fd_diffusion_equation_2d.polynomial_order")
            .value<int>()
            .value();
    const auto num_neighbors =
        config.at_path("rbf_fd_diffusion_equation_2d.num_neighbors")
            .value<num_collect::index_type>()
            .value();
    const auto diffusion_coefficient =
        config.at_path("rbf_fd_diffusion_equation_2d.diffusion_coefficient")
            .value<double>()
            .value();
    const auto time_step_size =
        config.at_path("rbf_fd_diffusion_equation_2d.time_step_size")
            .value<double>()
            .value();
    const auto final_time =
        config.at_path("rbf_fd_diffusion_equation_2d.final_time")
            .value<double>()
            .value();
    NUM_COLLECT_LOG_INFO(
        logger, "Number of interior nodes: {}", num_interior_nodes);
    NUM_COLLECT_LOG_INFO(logger, "Number of boundary nodes per edge: {}",
        num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Polynomial order: {}", polynomial_order);
    NUM_COLLECT_LOG_INFO(logger, "Number of neighbors: {}", num_neighbors);
    NUM_COLLECT_LOG_INFO(
        logger, "Diffusion coefficient: {}", diffusion_coefficient);
    NUM_COLLECT_LOG_INFO(logger, "Time step size: {}", time_step_size);
    NUM_COLLECT_LOG_INFO(logger, "Final time: {}", final_time);

    const auto nodes =
        generate_nodes(num_interior_nodes, num_boundary_nodes_per_edge);
    NUM_COLLECT_LOG_INFO(logger, "Generated {} nodes.", nodes.size());

    const auto problem = assemble_system(nodes, num_interior_nodes,
        polynomial_order, num_neighbors, diffusion_coefficient);
    solve_system(problem, diffusion_coefficient, time_step_size, final_time,
        nodes, num_interior_nodes);

    NUM_COLLECT_LOG_INFO(logger, "Finished.");

    return 0;
}
