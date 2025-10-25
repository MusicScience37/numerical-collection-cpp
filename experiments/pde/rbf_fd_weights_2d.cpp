/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Experiments to calculate weights of nodes using RBF-FD in 2D.
 *
 * This calculates one stencil to approximate the Laplacian operator
 * at [0.5, 0.5] using Halton nodes.
 */
#include <cstdlib>
#include <string_view>

#include <Eigen/Core>
#include <Eigen/LU>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/rbf/compute_kernel_matrix.h"
#include "num_collect/rbf/compute_polynomial_term_matrix.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/impl/differentiate_polynomial_term.h"
#include "num_collect/rbf/length_parameter_calculators/global_length_parameter_calculator.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/polynomial_term_generator.h"
#include "num_collect/rbf/rbfs/gaussian_m1_rbf.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/format_dense_vector.h"
#include "num_collect/util/vector_view.h"

using variable_type = Eigen::Vector2d;

constexpr num_collect::index_type num_nodes = 50;

[[nodiscard]] static auto compute_rbf_fd_weights(
    num_collect::util::vector_view<const variable_type> nodes,
    const Eigen::Vector2d& center) -> Eigen::VectorXd {
    using distance_function_type =
        num_collect::rbf::distance_functions::euclidean_distance_function<
            variable_type>;
    using rbf_type = num_collect::rbf::rbfs::gaussian_rbf<double>;
    using length_parameter_calculator_type =
        num_collect::rbf::length_parameter_calculators::
            global_length_parameter_calculator<distance_function_type>;
    using kernel_matrix_type = Eigen::MatrixXd;
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<variable_type>;
    using operator_evaluator_type =
        num_collect::rbf::operators::operator_evaluator<operator_type, rbf_type,
            distance_function_type>;

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(
        logger, "Computing weights with RBF-FD without polynomials.");

    const distance_function_type distance_function;
    const rbf_type rbf;
    length_parameter_calculator_type length_parameter_calculator;
    kernel_matrix_type kernel_matrix;
    num_collect::rbf::compute_kernel_matrix(distance_function, rbf,
        length_parameter_calculator, nodes, kernel_matrix);

    const operator_type op(center);
    Eigen::VectorXd right_hand_side(nodes.size());
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        right_hand_side(i) =
            operator_evaluator_type::evaluate_for_one_sample(distance_function,
                rbf, length_parameter_calculator.length_parameter_at(0), op,
                nodes[i], 1.0);
    }

    Eigen::PartialPivLU<kernel_matrix_type> lu_decomposition;
    lu_decomposition.compute(kernel_matrix);
    Eigen::VectorXd weights = lu_decomposition.solve(right_hand_side);

    NUM_COLLECT_LOG_TRACE(logger, "Weights: {:.2e}",
        num_collect::util::format_dense_vector(weights));

    return weights;
}

[[nodiscard]] static auto compute_rbf_fd_weights_with_polynomials(
    num_collect::util::vector_view<const variable_type> nodes,
    const Eigen::Vector2d& center, int degree) -> Eigen::VectorXd {
    using polynomial_term_generator_type =
        num_collect::rbf::polynomial_term_generator<2>;
    using distance_function_type =
        num_collect::rbf::distance_functions::euclidean_distance_function<
            variable_type>;
    using rbf_type = num_collect::rbf::rbfs::gaussian_m1_rbf<double>;
    using length_parameter_calculator_type =
        num_collect::rbf::length_parameter_calculators::
            global_length_parameter_calculator<distance_function_type>;
    using kernel_matrix_type = Eigen::MatrixXd;
    using operator_type =
        num_collect::rbf::operators::laplacian_operator<variable_type>;
    using operator_evaluator_type =
        num_collect::rbf::operators::operator_evaluator<operator_type, rbf_type,
            distance_function_type>;

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger,
        "Computing weights with RBF-FD with polynomials of degree {}.", degree);

    polynomial_term_generator_type polynomial_term_generator(degree);
    NUM_COLLECT_LOG_DEBUG(logger, "Number of polynomial terms: {}",
        polynomial_term_generator.terms().size());
    NUM_COLLECT_ASSERT(polynomial_term_generator.terms().size() < nodes.size());

    const distance_function_type distance_function;
    const rbf_type rbf;
    length_parameter_calculator_type length_parameter_calculator;
    kernel_matrix_type kernel_matrix;
    kernel_matrix_type polynomial_term_matrix;
    num_collect::rbf::compute_kernel_matrix(distance_function, rbf,
        length_parameter_calculator, nodes, kernel_matrix);
    num_collect::rbf::compute_polynomial_term_matrix(
        nodes, polynomial_term_matrix, polynomial_term_generator);

    kernel_matrix_type augmented_matrix(
        nodes.size() + polynomial_term_generator.terms().size(),
        nodes.size() + polynomial_term_generator.terms().size());
    augmented_matrix.setZero();
    augmented_matrix.topLeftCorner(nodes.size(), nodes.size()) = kernel_matrix;
    augmented_matrix.topRightCorner(nodes.size(),
        polynomial_term_generator.terms().size()) = polynomial_term_matrix;
    augmented_matrix.bottomLeftCorner(polynomial_term_generator.terms().size(),
        nodes.size()) = polynomial_term_matrix.transpose();

    const operator_type op(center);
    Eigen::VectorXd right_hand_side(
        nodes.size() + polynomial_term_generator.terms().size());
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        right_hand_side(i) =
            operator_evaluator_type::evaluate_for_one_sample(distance_function,
                rbf, length_parameter_calculator.length_parameter_at(0), op,
                nodes[i], 1.0);
    }
    const auto operator_differentiations =
        operator_evaluator_type::differentiations();
    for (num_collect::index_type i = 0;
        i < polynomial_term_generator.terms().size(); ++i) {
        const auto& term = polynomial_term_generator.terms()[i];
        double value = 0.0;
        for (const auto& orders : operator_differentiations) {
            const auto differentiation_result =
                num_collect::rbf::impl::differentiate_polynomial_term<double>(
                    term, orders);
            if (differentiation_result) {
                value += differentiation_result->first(center) *
                    differentiation_result->second;
            }
            right_hand_side(nodes.size() + i) = value;
        }
    }

    Eigen::PartialPivLU<kernel_matrix_type> lu_decomposition;
    lu_decomposition.compute(augmented_matrix);
    Eigen::VectorXd augmented_weights = lu_decomposition.solve(right_hand_side);
    Eigen::VectorXd weights = augmented_weights.head(nodes.size());

    NUM_COLLECT_LOG_TRACE(logger, "Weights: {:.2e}",
        num_collect::util::format_dense_vector(weights));

    return weights;
}

static void evaluate_weights(
    num_collect::util::vector_view<const variable_type> nodes,
    const Eigen::Vector2d& center, const Eigen::VectorXd& weights) {
    const auto evaluated_function = [](const Eigen::Vector2d& variable) {
        return variable.array().sin().prod();
    };
    const auto laplacian_of_function = [](const Eigen::Vector2d& variable) {
        return -2.0 * variable.array().sin().prod();
    };

    double approximated_value = 0.0;
    for (num_collect::index_type i = 0; i < nodes.size(); ++i) {
        approximated_value += weights(i) * evaluated_function(nodes[i]);
    }

    const double exact_value = laplacian_of_function(center);

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger,
        "Approx: {:.10e}, Exact: {:.10e}, Error: {:.2e}", approximated_value,
        exact_value, std::abs(approximated_value - exact_value));
}

auto main(int argc, const char** argv) -> int {
    std::string_view config_file_path =
        "experiments/pde/rbf_fd_weights_2d.toml";
    if (argc == 2) {
        config_file_path = argv[1];
    }
    num_collect::logging::load_logging_config_file(config_file_path);

    const auto nodes =
        num_collect::rbf::generate_halton_nodes<double, 2>(num_nodes);
    const Eigen::Vector2d center(0.5, 0.5);
    Eigen::VectorXd weights = compute_rbf_fd_weights(nodes, center);
    evaluate_weights(nodes, center, weights);
    const int max_degree = 8;
    for (int degree = 0; degree <= max_degree; ++degree) {
        weights =
            compute_rbf_fd_weights_with_polynomials(nodes, center, degree);
        evaluate_weights(nodes, center, weights);
    }

    return 0;
}
