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
 * \brief Definition of tv_admm class.
 */
#pragma once

#include <algorithm>
#include <limits>
#include <utility>

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/linear/impl/operator_conjugate_gradient.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/regularization/impl/apply_shrinkage_operator.h"
#include "num_collect/regularization/impl/approximate_max_eigen_aat.h"
#include "num_collect/regularization/impl/weak_coeff_param.h"  // IWYU pragma: keep
#include "num_collect/regularization/iterative_regularized_solver_base.h"

namespace num_collect::regularization {

//! Tag of tv_admm.
constexpr auto tv_admm_tag =
    logging::log_tag_view("num_collect::regularization::tv_admm");

/*!
 * \brief Class to solve linear equations with total variation (TV)
 * regularization using the alternating direction method of multipliers (ADMM)
 * \cite Boyd2010.
 *
 * \tparam Coeff Type of coefficient matrices.
 * \tparam DerivativeMatrix Type of matrices to compute derivatives.
 * \tparam Data Type of data vectors.
 */
template <typename Coeff, typename DerivativeMatrix,
    base::concepts::dense_vector Data>
    requires((base::concepts::sparse_matrix<Coeff> ||
                 base::concepts::dense_matrix<Coeff>) &&
        (base::concepts::sparse_matrix<DerivativeMatrix> ||
            base::concepts::dense_matrix<DerivativeMatrix>))
class tv_admm : public iterative_regularized_solver_base<
                    tv_admm<Coeff, DerivativeMatrix, Data>, Data> {
public:
    //! This type.
    using this_type = tv_admm<Coeff, DerivativeMatrix, Data>;

    //! Type of the base class.
    using base_type = iterative_regularized_solver_base<this_type, Data>;

    using typename base_type::data_type;
    using typename base_type::scalar_type;

    //! Type of coefficient matrices.
    using coeff_type = Coeff;

    //! Type of matrices to compute derivatives.
    using derivative_matrix_type = DerivativeMatrix;

    /*!
     * \brief Constructor.
     */
    tv_admm() : base_type(tv_admm_tag) {
        this->configure_child_algorithm_logger_if_exists(conjugate_gradient_);
    }

    /*!
     * \brief Compute internal parameters.
     *
     * \param[in] coeff Coefficient matrix.
     * \param[in] data Data vector.
     * \param[in] derivative_matrix Matrix to compute derivative.
     *
     * \note Pointers to the arguments are saved in this object, so don't
     * destruct those arguments.
     * \note Call this before init.
     */
    void compute(const Coeff& coeff, const DerivativeMatrix& derivative_matrix,
        const Data& data) {
        coeff_ = &coeff;
        derivative_matrix_ = &derivative_matrix;
        data_ = &data;
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::init
    void init(const scalar_type& param, data_type& solution) {
        (void)param;

        if (coeff_->rows() != data_->rows()) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "Coefficient matrix and data vector must have the same number "
                "of rows.");
        }
        if (coeff_->cols() != solution.rows()) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "The number of columns in the coefficient matrix must match "
                "the number of rows in solution vector.");
        }
        if (data_->cols() != solution.cols()) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "Data and solution must have the same number of columns.");
        }
        if (derivative_matrix_->cols() != solution.rows()) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "The number of columns in the derivative matrix must match the "
                "number of rows in solution vector.");
        }

        iterations_ = 0;
        derivative_ = (*derivative_matrix_) * solution;
        lagrange_multiplier_ = data_type::Zero(derivative_matrix_->rows());
        temp_solution_ = solution;
        residual_ = (*coeff_) * solution - (*data_);
        update_rate_ = std::numeric_limits<scalar_type>::infinity();

        const scalar_type conjugate_gradient_tolerance_rate =
            rate_of_cg_tol_rate_to_tol_update_rate_ * tol_update_rate_;
        conjugate_gradient_.tolerance_rate(conjugate_gradient_tolerance_rate);
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::iterate
    void iterate(const scalar_type& param, data_type& solution) {
        // Update solution.
        temp_solution_ =
            static_cast<scalar_type>(2) * (*coeff_).transpose() * (*data_) -
            (*derivative_matrix_).transpose() * lagrange_multiplier_ +
            derivative_constraint_coeff_ * (*derivative_matrix_).transpose() *
                derivative_;
        previous_solution_ = solution;
        conjugate_gradient_.solve(
            [this](const data_type& target, data_type& result) {
                result = static_cast<scalar_type>(2) * (*coeff_).transpose() *
                    (*coeff_) * target;
                result += derivative_constraint_coeff_ *
                    (*derivative_matrix_).transpose() * (*derivative_matrix_) *
                    target;
            },
            temp_solution_, solution);
        update_rate_ = (solution - previous_solution_).norm() /
            (solution.norm() + std::numeric_limits<scalar_type>::epsilon());
        residual_ = (*coeff_) * solution - (*data_);

        // Update derivative.
        previous_derivative_ = derivative_;
        derivative_ = (*derivative_matrix_) * solution +
            lagrange_multiplier_ / derivative_constraint_coeff_;
        const scalar_type derivative_shrinkage_threshold =
            param / derivative_constraint_coeff_;
        impl::apply_shrinkage_operator(
            derivative_, derivative_shrinkage_threshold);
        update_rate_ += (derivative_ - previous_derivative_).norm() /
            (derivative_.norm() + std::numeric_limits<scalar_type>::epsilon());

        // Update lagrange multiplier.
        lagrange_multiplier_update_ = derivative_constraint_coeff_ *
            ((*derivative_matrix_) * solution - derivative_);
        lagrange_multiplier_ += lagrange_multiplier_update_;
        update_rate_ += lagrange_multiplier_update_.norm() /
            (lagrange_multiplier_.norm() +
                std::numeric_limits<scalar_type>::epsilon());

        ++iterations_;
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::is_stop_criteria_satisfied
    [[nodiscard]] auto is_stop_criteria_satisfied(
        const data_type& solution) const -> bool {
        (void)solution;
        return (iterations() > max_iterations()) ||
            (update_rate() < tol_update_rate());
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::configure_iteration_logger
    void configure_iteration_logger(
        logging::iterations::iteration_logger<this_type>& iteration_logger)
        const {
        iteration_logger.template append<index_type>(
            "Iter.", &this_type::iterations);
        iteration_logger.template append<scalar_type>(
            "UpdateRate", &this_type::update_rate);
        iteration_logger.template append<scalar_type>(
            "Res.Rate", &this_type::residual_norm_rate);
    }

    //! \copydoc num_collect::regularization::implicit_regularized_solver_base::residual_norm
    [[nodiscard]] auto residual_norm(
        const data_type& solution) const -> scalar_type {
        return ((*coeff_) * solution - (*data_)).squaredNorm();
    }

    //! \copydoc num_collect::regularization::implicit_regularized_solver_base::regularization_term
    [[nodiscard]] auto regularization_term(
        const data_type& solution) const -> scalar_type {
        return (*derivative_matrix_ * solution).template lpNorm<1>();
    }

    //! \copydoc num_collect::regularization::implicit_regularized_solver_base::change_data
    void change_data(const data_type& data) { data_ = &data; }

    //! \copydoc num_collect::regularization::implicit_regularized_solver_base::calculate_data_for
    void calculate_data_for(const data_type& solution, data_type& data) const {
        data = (*coeff_) * solution;
    }

    //! \copydoc num_collect::regularization::regularized_solver_base::data_size
    [[nodiscard]] auto data_size() const -> index_type { return data_->size(); }

    //! \copydoc num_collect::regularization::regularized_solver_base::param_search_region
    [[nodiscard]] auto param_search_region() const
        -> std::pair<scalar_type, scalar_type> {
        const data_type approx_order_of_solution = coeff_->transpose() *
            (*data_) / impl::approximate_max_eigen_aat(*coeff_);
        const scalar_type approx_order_of_param =
            (*derivative_matrix_ * approx_order_of_solution)
                .cwiseAbs()
                .maxCoeff();
        NUM_COLLECT_LOG_TRACE(
            this->logger(), "approx_order_of_param={}", approx_order_of_param);
        constexpr auto tol_update_coeff_multiplier =
            static_cast<scalar_type>(10);
        return {approx_order_of_param *
                std::max(impl::weak_coeff_min_param<scalar_type>,
                    tol_update_coeff_multiplier * tol_update_rate_),
            approx_order_of_param * impl::weak_coeff_max_param<scalar_type>};
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Value.
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return iterations_;
    }

    /*!
     * \brief Get the rate of the norm of the update of the solution in the last
     * iteration.
     *
     * \return Value.
     */
    [[nodiscard]] auto update_rate() const noexcept -> scalar_type {
        return update_rate_;
    }

    /*!
     * \brief Get the rate of the last residual norm.
     *
     * \return Rate of the residual norm.
     */
    [[nodiscard]] auto residual_norm_rate() const -> scalar_type {
        return residual_.squaredNorm() / data_->squaredNorm();
    }

    /*!
     * \brief Get the maximum number of iterations.
     *
     * \return Value.
     */
    [[nodiscard]] auto max_iterations() const -> index_type {
        return max_iterations_;
    }

    /*!
     * \brief Set the maximum number of iterations.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto max_iterations(index_type value) -> tv_admm& {
        if (value <= 0) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "Maximum number of iterations must be a positive integer.");
        }
        max_iterations_ = value;
        return *this;
    }

    /*!
     * \brief Get the tolerance of update rate of the solution.
     *
     * \return Value.
     */
    [[nodiscard]] auto tol_update_rate() const -> scalar_type {
        return tol_update_rate_;
    }

    /*!
     * \brief Set the tolerance of update rate of the solution.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto tol_update_rate(scalar_type value) -> tv_admm& {
        if (value <= static_cast<scalar_type>(0)) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "Tolerance of update rate of the solution must be a positive "
                "value.");
        }
        tol_update_rate_ = value;
        return *this;
    }

private:
    //! Coefficient matrix to compute data vector.
    const coeff_type* coeff_{nullptr};

    //! Matrix to compute derivative.
    const derivative_matrix_type* derivative_matrix_{nullptr};

    //! Data vector.
    const data_type* data_{nullptr};

    //! Number of iterations.
    index_type iterations_{};

    //! Derivative.
    data_type derivative_{};

    //! Lagrange multiplier.
    data_type lagrange_multiplier_{};

    //! Temporary vector for the update of the solution.
    data_type temp_solution_{};

    //! Previous solution.
    data_type previous_solution_{};

    //! Previous derivative.
    data_type previous_derivative_{};

    //! Update of lagrange multiplier.
    data_type lagrange_multiplier_update_{};

    //! Residual vector.
    data_type residual_{};

    //! Rate of norm of the update of the solution in the last iteration.
    scalar_type update_rate_{};

    //! Conjugate gradient solver.
    linear::impl::operator_conjugate_gradient<data_type> conjugate_gradient_{};

    //! Default coefficient of the constraint for the derivative.
    static constexpr auto default_derivative_constraint_coeff =
        static_cast<scalar_type>(1);

    //! Coefficient of the constraint for the derivative.
    scalar_type derivative_constraint_coeff_{
        default_derivative_constraint_coeff};

    //! Default maximum number of iterations.
    static constexpr index_type default_max_iterations = 1000;

    //! Maximum number of iterations.
    index_type max_iterations_{default_max_iterations};

    //! Default tolerance of update rate of the solution.
    static constexpr auto default_tol_update_rate =
        static_cast<scalar_type>(1e-4);

    //! Tolerance of update rate of the solution.
    scalar_type tol_update_rate_{default_tol_update_rate};

    //! Default value of the ratio of the rate of tolerance in CG method to the tolerance of update rate of the solution.
    static constexpr auto default_rate_of_cg_tol_rate_to_tol_update_rate =
        static_cast<scalar_type>(1e-2);

    //! Rate of the ratio of the rate of tolerance in CG method to the tolerance of update rate of the solution.
    scalar_type rate_of_cg_tol_rate_to_tol_update_rate_{
        default_rate_of_cg_tol_rate_to_tol_update_rate};
};

}  // namespace num_collect::regularization
