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
 * \brief Definition of rbf_fd_row_calculator class.
 */
#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

#include <Eigen/Cholesky>
#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/rbf/concepts/length_parameter_calculator.h"
#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/rbf/concepts/rbf_fd_operator_with.h"
#include "num_collect/rbf/impl/compute_kernel_matrix_serial.h"
#include "num_collect/rbf/operators/operator_evaluator.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"

namespace num_collect::rbf::impl {

/*!
 * \brief Class to calculate rows of the system matrix in RBF-FD method.
 *
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 *
 * \note This class is not thread safe. For parallel processing,
 * create one instance of this class for each thread.
 */
template <concepts::length_parameter_calculator LengthParameterCalculator>
class rbf_fd_row_calculator {
public:
    //! Type of the calculator of length parameters.
    using length_parameter_calculator_type = LengthParameterCalculator;

    //! Type of variables.
    using variable_type = typename LengthParameterCalculator::variable_type;

    //! Type of scalars.
    using scalar_type = typename LengthParameterCalculator::scalar_type;

    //! Type of matrices.
    using matrix_type = Eigen::MatrixX<scalar_type>;

    //! Type of vectors.
    using vector_type = Eigen::VectorX<scalar_type>;

    //! Type of distance functions.
    using distance_function_type =
        typename length_parameter_calculator_type::distance_function_type;

    /*!
     * \brief Constructor.
     *
     * \note Internal resources are allocated in the first call of
     * compute_row().
     */
    rbf_fd_row_calculator() = default;

    /*!
     * \brief Compute a row of the system matrix.
     *
     * \tparam RBF Type of the RBF.
     * \tparam Operator Type of the operator to assemble.
     * \tparam StorageIndex Type of indices in the system matrix.
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     * \param[in] target_operator Operator to assemble. The variable used in
     * this operator is assumed to be same as row_variable.
     * \param[in] row_variable Variable corresponding to the row to calculate.
     * \param[in] column_variables Variables corresponding to the columns to
     * calculate.
     * \param[in] column_variables_nearest_neighbor_searcher Nearest neighbor
     * searcher for the column variables.
     * \param[in] num_neighbors Number of neighbors to use in RBF-FD.
     * \param[out] triplets Output vector of triplets to set values in the
     * system matrix.
     * \param[in] row_index Row index to calculate.
     * \param[in] column_offset Offset of column indices in the system matrix.
     *
     * \note Existing triplets are not cleared in this function
     * because this function will be called multiple times to assemble the whole
     * system matrix.
     */
    template <concepts::rbf RBF,
        concepts::rbf_fd_operator_with<RBF, distance_function_type,
            length_parameter_calculator_type>
            Operator,
        std::signed_integral StorageIndex>
        requires std::is_same_v<typename RBF::scalar_type, scalar_type>
    void compute_row(const distance_function_type& distance_function,
        const RBF& rbf, const Operator& target_operator,
        const variable_type& row_variable,
        util::vector_view<const variable_type> column_variables,
        const util::nearest_neighbor_searcher<variable_type>&
            column_variables_nearest_neighbor_searcher,
        index_type num_neighbors,
        util::vector<Eigen::Triplet<scalar_type, StorageIndex>>& triplets,
        index_type row_index, index_type column_offset) {
        search_neighbors(row_variable, column_variables,
            column_variables_nearest_neighbor_searcher, num_neighbors);
        create_linear_system(
            distance_function, rbf, target_operator, num_neighbors);
        solve_linear_system(row_index);

        for (index_type i = 0; i < num_neighbors; ++i) {
            triplets.emplace_back(static_cast<StorageIndex>(row_index),
                static_cast<StorageIndex>(
                    column_offset + neighbor_indices_and_distances_[i].first),
                weights_(i));
        }
    }

    /*!
     * \brief Set the scale of length parameters.
     *
     * \param[in] value Value.
     */
    void length_parameter_scale(scalar_type value) {
        length_parameter_calculator_.scale(value);
    }

private:
    /*!
     * \brief Search neighbors.
     *
     * \param[in] row_variable Variable corresponding to the row to calculate.
     * \param[in] column_variables Variables corresponding to the columns to
     * calculate.
     * \param[in] column_variables_nearest_neighbor_searcher Nearest neighbor
     * searcher for the column variables.
     * \param[in] num_neighbors Number of neighbors to use in RBF-FD.
     */
    void search_neighbors(const variable_type& row_variable,
        util::vector_view<const variable_type> column_variables,
        const util::nearest_neighbor_searcher<variable_type>&
            column_variables_nearest_neighbor_searcher,
        index_type num_neighbors) {
        NUM_COLLECT_PRECONDITION(
            num_neighbors > 0, "Number of neighbors must be positive.");
        NUM_COLLECT_PRECONDITION(num_neighbors <= column_variables.size(),
            "Number of neighbors must be less than or equal to the number of "
            "column variables.");

        neighbor_indices_and_distances_.clear();
        neighbor_indices_and_distances_.reserve(num_neighbors);
        column_variables_nearest_neighbor_searcher.find_k_nearest_neighbors(
            num_neighbors, row_variable, neighbor_indices_and_distances_);
        neighbor_variables_.clear();
        neighbor_variables_.reserve(num_neighbors);
        for (const auto& [index, distance] : neighbor_indices_and_distances_) {
            (void)distance;  // not used.
            neighbor_variables_.push_back(column_variables[index]);
        }
    }

    /*!
     * \brief Create a linear system to calculate weights.
     *
     * \tparam RBF Type of the RBF.
     * \tparam Operator Type of the operator to assemble.
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     * \param[in] target_operator Operator to assemble.
     * \param[in] num_neighbors Number of neighbors to use in RBF-FD.
     */
    template <concepts::rbf RBF,
        concepts::rbf_fd_operator_with<RBF, distance_function_type,
            length_parameter_calculator_type>
            Operator>
    void create_linear_system(const distance_function_type& distance_function,
        const RBF& rbf, const Operator& target_operator,
        index_type num_neighbors) {
        using operator_evaluator_type = operators::operator_evaluator<Operator,
            RBF, distance_function_type>;

        compute_kernel_matrix_serial(distance_function, rbf,
            length_parameter_calculator_, neighbor_variables_, kernel_matrix_);

        right_hand_side_.resize(num_neighbors);
        for (index_type i = 0; i < num_neighbors; ++i) {
            constexpr auto kernel_coeff = static_cast<scalar_type>(1);
            right_hand_side_(i) =
                operator_evaluator_type::evaluate_for_one_sample(
                    distance_function, rbf,
                    length_parameter_calculator_.length_parameter_at(i),
                    target_operator, neighbor_variables_[i], kernel_coeff);
        }
    }

    /*!
     * \brief Solve the linear system to calculate weights.
     *
     * \param[in] row_index Row index to calculate.
     */
    void solve_linear_system(index_type row_index) {
        matrix_solver_.compute(kernel_matrix_);
        weights_ = matrix_solver_.solve(right_hand_side_);
        if (!weights_.allFinite()) {
            const scalar_type cond = matrix_solver_.rcond();
            NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
                "Failed to solve a linear system for RBF-FD weights. "
                "(row index: {}, estimated reciprocal of condition number: "
                "{:.2e})",
                row_index, cond);
        }
    }

    //! Calculator of length parameters.
    length_parameter_calculator_type length_parameter_calculator_;

    //! Buffer of indices and distances of neighbors.
    util::vector<std::pair<index_type, scalar_type>>
        neighbor_indices_and_distances_;

    //! Buffer of variables of neighbors.
    util::vector<variable_type> neighbor_variables_;

    //! Kernel matrix.
    matrix_type kernel_matrix_;

    //! Right-hand-side of the linear equation.
    vector_type right_hand_side_;

    //! Solver of the linear equation of kernel matrix.
    Eigen::LDLT<matrix_type> matrix_solver_;

    //! Buffer of calculated weights.
    vector_type weights_;
};

}  // namespace num_collect::rbf::impl
