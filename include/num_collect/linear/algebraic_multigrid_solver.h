/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of algebraic_multigrid_solver class.
 */
#pragma once

#include <cmath>
#include <cstddef>
#include <deque>
#include <optional>
#include <vector>

#include <Eigen/Cholesky>
#include <Eigen/Core>

#include "num_collect/base/concepts/dense_vector_of.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/linear/impl/amg/build_first_coarse_grid_candidate.h"
#include "num_collect/linear/impl/amg/compute_strong_connection_list.h"
#include "num_collect/linear/impl/amg/create_prolongation_matrix.h"
#include "num_collect/linear/impl/amg/tune_coarse_grid_selection.h"
#include "num_collect/linear/iterative_solver_base.h"
#include "num_collect/linear/parallel_symmetric_successive_over_relaxation.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/util/scoped_eigen_no_malloc.h"

namespace num_collect::linear {

template <base::concepts::sparse_matrix Matrix>
class algebraic_multigrid_solver;

//! Log tag for num_collect::linear::algebraic_multigrid_solver.
constexpr auto algebraic_multigrid_solver_tag =
    logging::log_tag_view("num_collect::linear::algebraic_multigrid_solver");

namespace impl {

/*!
 * \brief Traits of algebraic_multigrid_solver class.
 *
 * \tparam Matrix Type of the matrix.
 */
template <base::concepts::sparse_matrix Matrix>
struct iterative_solver_traits<algebraic_multigrid_solver<Matrix>> {
    //! Type of the matrix.
    using matrix_type = Matrix;
};

}  // namespace impl

/*!
 * \brief Class to solve linear equations using algebraic multigrid method
 * \cite Ruge1987.
 *
 * \tparam Matrix Type of the matrix.
 */
template <base::concepts::sparse_matrix Matrix>
class algebraic_multigrid_solver
    : public iterative_solver_base<algebraic_multigrid_solver<Matrix>>,
      public logging::logging_mixin {
    static_assert(base::concepts::real_scalar<typename Matrix::Scalar>,
        "Complex matrices are not supported.");

public:
    //! Type of this class.
    using this_type = algebraic_multigrid_solver<Matrix>;

    //! Type of the base class.
    using base_type = iterative_solver_base<algebraic_multigrid_solver<Matrix>>;

    using typename base_type::matrix_type;
    using typename base_type::real_scalar_type;
    using typename base_type::scalar_type;
    using typename base_type::storage_index_type;

protected:
    using base_type::coeff;

public:
    /*!
     * \brief Constructor.
     */
    algebraic_multigrid_solver()
        : logging::logging_mixin(algebraic_multigrid_solver_tag) {}

    /*!
     * \brief Prepare to solve.
     *
     * \tparam InputMatrix Type of the matrix.
     * \param[in] matrix Coefficient matrix.
     * \return This object.
     */
    template <typename InputMatrix>
    auto compute(const Eigen::EigenBase<InputMatrix>& matrix) -> this_type& {
        base_type::compute(matrix);

        constexpr index_type smoother_iterations = 1;

        std::optional<Eigen::Ref<const matrix_type>> current_matrix;
        current_matrix.emplace(coeff());
        std::optional<Eigen::Ref<const matrix_type>> current_prolongation;
        index_type next_matrix_size = current_matrix->cols();

        intermidiate_layers_.clear();
        while (next_matrix_size > maximum_directly_solved_matrix_size_) {
            NUM_COLLECT_LOG_TRACE(
                this->logger(), "AMG layer size {}", next_matrix_size);
            intermidiate_layer_data& next_layer =
                intermidiate_layers_.emplace_back(*current_matrix);
            compute_prolongation_matrix(
                next_layer.prolongation_matrix, next_layer.coeff_matrix);
            next_layer.smoother.compute(next_layer.coeff_matrix);
            next_layer.smoother.max_iterations(smoother_iterations);

            current_prolongation.emplace(next_layer.prolongation_matrix);

            matrix_type& next_matrix = matrix_buffers_.emplace_back();
            next_matrix = (*current_prolongation).transpose() *
                (*current_matrix) * (*current_prolongation);
            current_matrix.emplace(next_matrix);

            next_matrix_size = current_matrix->cols();
        }

        // Initialization of the final layer.
        NUM_COLLECT_LOG_TRACE(this->logger(), "AMG layer size {} (final layer)",
            next_matrix_size);
        final_layer_.coeff_matrix = (*current_matrix);
        final_layer_.solver.compute(final_layer_.coeff_matrix);

        // Initialization of buffers.
        if (!intermidiate_layers_.empty()) {
            residual_buffers_.resize(intermidiate_layers_.size());
            residual_buffers_before_prolongation_.resize(
                intermidiate_layers_.size());
            solution_buffers_.resize(intermidiate_layers_.size());
            residual_buffers_before_prolongation_.front() =
                dense_vector_type::Zero(coeff().cols());
            for (std::size_t i = 1; i < intermidiate_layers_.size(); ++i) {
                const index_type size =
                    intermidiate_layers_[i].coeff_matrix.cols();
                residual_buffers_[i - 1U] = dense_vector_type::Zero(size);
                solution_buffers_[i - 1U] = dense_vector_type::Zero(size);
                residual_buffers_before_prolongation_[i] =
                    dense_vector_type::Zero(size);
            }
            {
                const index_type size = final_layer_.coeff_matrix.cols();
                residual_buffers_.back() = dense_vector_type::Zero(size);
                solution_buffers_.back() = dense_vector_type::Zero(size);
            }
        }

        return *this;
    }

    /*!
     * \brief Iterate repeatedly until stop criterion is satisfied for a vector.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \tparam Solution Type of the solution vector.
     * \param[in] right Right-hand-side vector.
     * \param[in,out] solution Solution vector.
     */
    template <base::concepts::dense_vector_of<scalar_type> Right,
        base::concepts::dense_vector_of<scalar_type> Solution>
    void solve_vector_in_place(const Right& right, Solution& solution) const {
        const auto& coeff_ref = coeff();

        NUM_COLLECT_PRECONDITION(coeff_ref.rows() == coeff_ref.cols(),
            this->logger(), "Coefficient matrix must be a square matrix.");
        NUM_COLLECT_PRECONDITION(right.rows() == coeff_ref.cols(),
            this->logger(),
            "Right-hand-side vector must have the number of elements same as "
            "the size of the coefficient matrix.");
        NUM_COLLECT_PRECONDITION(solution.rows() == coeff_ref.cols(),
            this->logger(),
            "Solution vector must have the number of elements same as the size "
            "of the coefficient matrix.");

        util::scoped_eigen_no_malloc no_malloc;

        iterations_ = 0;
        const index_type max_iterations = base_type::max_iterations();
        while (iterations_ < max_iterations) {
            iterate(right, solution);
            ++iterations_;
            using std::sqrt;
            if (residual_rate() < base_type::tolerance()) {
                break;
            }
        }

        NUM_COLLECT_LOG_SUMMARY(this->logger(),
            "Solved a linear equation with {} iterations. (Residual rate: {})",
            iterations_, residual_rate());
    }

    /*!
     * \brief Set the maximum size of matrices to solve directly.
     *
     * \param[in] value Value.
     * \return This.
     */
    auto maximum_directly_solved_matrix_size(index_type value)
        -> algebraic_multigrid_solver& {
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "The maximum size of matrices to solve directly must be a positive "
            "integer.");
        maximum_directly_solved_matrix_size_ = value;
        return *this;
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return iterations_;
    }

    /*!
     * \brief Get the rate of the last residual.
     *
     * \note This value is updated when a iteration is performed.
     *
     * \return Rate of the last residual.
     */
    [[nodiscard]] auto residual_rate() const noexcept -> scalar_type {
        if (intermidiate_layers_.empty()) {
            return std::numeric_limits<scalar_type>::epsilon();
        }
        return intermidiate_layers_.front().smoother.residual_rate();
    }

private:
    /*!
     * \brief Iterate once.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \tparam Solution Type of the solution vector.
     * \param[in] right Right-hand-side vector.
     * \param[in,out] solution Solution vector.
     */
    template <base::concepts::dense_vector_of<scalar_type> Right,
        base::concepts::dense_vector_of<scalar_type> Solution>
    void iterate(const Right& right, Solution& solution) const {
        if (intermidiate_layers_.empty()) {
            solution = final_layer_.solver.solve(right);
            return;
        }

        // First layer.
        intermidiate_layers_.front().smoother.solve_vector_in_place(
            right, solution);
        residual_buffers_before_prolongation_.front() = right;
        residual_buffers_before_prolongation_.front().noalias() -=
            coeff() * solution;
        residual_buffers_.front().noalias() =
            intermidiate_layers_.front().prolongation_matrix.transpose() *
            residual_buffers_before_prolongation_.front();

        // Intermidiate layers.
        for (std::size_t i = 1; i < intermidiate_layers_.size(); ++i) {
            const intermidiate_layer_data& layer = intermidiate_layers_[i];
            solution_buffers_[i - 1U] =
                dense_vector_type::Zero(layer.coeff_matrix.cols());
            layer.smoother.solve_vector_in_place(
                residual_buffers_[i - 1U], solution_buffers_[i - 1U]);
            residual_buffers_before_prolongation_[i] =
                residual_buffers_[i - 1U];
            residual_buffers_before_prolongation_[i].noalias() -=
                layer.coeff_matrix * solution_buffers_[i - 1];
            residual_buffers_[i].noalias() =
                layer.prolongation_matrix.transpose() *
                residual_buffers_before_prolongation_[i];
        }

        // Final layer.
        solution_buffers_.back() =
            final_layer_.solver.solve(residual_buffers_.back());

        // Intermidiate layers.
        for (std::size_t i = intermidiate_layers_.size() - 1; i > 0; --i) {
            const intermidiate_layer_data& layer = intermidiate_layers_[i];
            solution_buffers_[i - 1U].noalias() +=
                layer.prolongation_matrix * solution_buffers_[i];
            layer.smoother.solve_vector_in_place(
                residual_buffers_[i - 1U], solution_buffers_[i - 1U]);
        }

        // First layer.
        solution.noalias() += intermidiate_layers_.front().prolongation_matrix *
            solution_buffers_.front();
        intermidiate_layers_.front().smoother.solve_vector_in_place(
            right, solution);
    }

    /*!
     * \brief Compute prolongation matrix from a coefficient matrix.
     *
     * \param[out] prolongation_matrix Prolongation matrix.
     * \param[in] coeff_matrix Coefficient matrix.
     */
    void compute_prolongation_matrix(matrix_type& prolongation_matrix,
        const Eigen::Ref<const matrix_type>& coeff_matrix) {
        const auto connections = impl::amg::compute_strong_connection_list(
            coeff_matrix, strong_coeff_rate_threshold_);
        const auto transposed_connections = connections.transpose();
        auto node_classification = impl::amg::build_first_coarse_grid_candidate(
            connections, transposed_connections);
        impl::amg::tune_coarse_grid_selection(
            connections, transposed_connections, node_classification);
        impl::amg::create_prolongation_matrix(
            prolongation_matrix, transposed_connections, node_classification);
    }

    //! Type of the dense matrix used in this algorithm.
    using dense_matrix_type = Eigen::MatrixX<scalar_type>;

    //! Type of the dense vector used in this algorithm.
    using dense_vector_type = Eigen::VectorX<scalar_type>;

    //! Struct of internal data for the intermidiate layers.
    struct intermidiate_layer_data {
        //! Coefficient matrix in this layer.
        Eigen::Ref<const matrix_type> coeff_matrix;

        //! Prolongation matrix from the next layer.
        matrix_type prolongation_matrix;

        //! Smoother in this layer.
        parallel_symmetric_successive_over_relaxation<matrix_type> smoother;

        /*!
         * \brief Constructor.
         *
         * \param[in] input_coeff_matrix Coefficient matrix in this layer.
         */
        explicit intermidiate_layer_data(
            Eigen::Ref<const matrix_type> input_coeff_matrix)
            : coeff_matrix(input_coeff_matrix) {}
    };

    //! Struct of internal data for the final layer.
    struct final_layer_data {
        //! Coefficient matrix in this layer.
        dense_matrix_type coeff_matrix;

        //! Solver of the final coefficient matrix.
        Eigen::LLT<dense_matrix_type> solver;
    };

    /*!
     * \brief Data of the intermidiate layers.
     *
     * \note deque is required because intermidiate_layer_data is not copyable.
     */
    std::deque<intermidiate_layer_data> intermidiate_layers_{};

    //! Data of the final layer.
    final_layer_data final_layer_{};

    /*!
     * \brief Buffers of coefficient matrices in layers.
     *
     * \note deque is used to prevent invalidation of references when layers are
     * added.
     */
    std::deque<matrix_type> matrix_buffers_{};

    //! Buffers of residuals in layers except for the first layer.
    mutable std::vector<dense_vector_type> residual_buffers_{};

    //! Buffers of residuals before prolongation.
    mutable std::vector<dense_vector_type>
        residual_buffers_before_prolongation_{};

    //! Buffers of solutions in layers except for the first layer.
    mutable std::vector<dense_vector_type> solution_buffers_{};

    //! Number of iterations.
    mutable index_type iterations_{};

    //! Default value of the threshold of the rate of coefficients to determine strong connections.
    static constexpr auto default_strong_coeff_rate_threshold =
        static_cast<real_scalar_type>(0.25);

    //! Threshold of the rate of coefficients to determine strong connections.
    real_scalar_type strong_coeff_rate_threshold_{
        default_strong_coeff_rate_threshold};

    //! Default value of the maximum size of matrices to solve directly.
    static constexpr index_type default_maximum_directly_solved_matrix_size =
        500;

    //! Maximum size of matrices to solve directly.
    index_type maximum_directly_solved_matrix_size_{
        default_maximum_directly_solved_matrix_size};
};

}  // namespace num_collect::linear
