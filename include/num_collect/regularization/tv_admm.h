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
#include "num_collect/base/precondition.h"
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
 *
 * This class minimizes the following evaluation function:
 *
 * \f[
 * \| A \boldsymbol{x} - \boldsymbol{y} \|_2^2
 * + \lambda \| D \boldsymbol{x} \|_1
 * \f]
 *
 * where variables are defined as follows:
 *
 * - \f$A\f$ is a coefficient matrix.
 * - \f$\boldsymbol{x}\f$ is a solution vector.
 * - \f$\boldsymbol{y}\f$ is a data vector.
 * - \f$\lambda\f$ is a regularization parameter.
 * - \f$D\f$ is a matrix to compute derivatives.
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

        NUM_COLLECT_PRECONDITION(coeff_->rows() == data_->rows(),
            this->logger(),
            "Coefficient matrix and data vector must have the same number of "
            "rows.");
        NUM_COLLECT_PRECONDITION(derivative_matrix_->cols() == coeff_->cols(),
            this->logger(),
            "The number of columns in the derivative matrix must match the "
            "number of columns in the coefficient matrix.");

        coeff_transpose_ = coeff_->transpose();
        dtd_ = (*derivative_matrix_).transpose() * (*derivative_matrix_);
        medium_constraint_coeff_ = impl::approximate_max_eigen_aat(*coeff_) /
            (impl::approximate_max_eigen_aat(*derivative_matrix_) +
                std::numeric_limits<scalar_type>::epsilon());
        NUM_COLLECT_LOG_TRACE(this->logger(), "medium_constraint_coeff={}",
            medium_constraint_coeff_);
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::init
    void init(const scalar_type& param, data_type& solution) {
        (void)param;

        NUM_COLLECT_PRECONDITION(coeff_->cols() == solution.rows(),
            this->logger(),
            "The number of columns in the coefficient matrix must match the "
            "number of rows in solution vector.");
        NUM_COLLECT_PRECONDITION(data_->cols() == solution.cols(),
            this->logger(),
            "Data and solution must have the same number of columns.");

        constraint_coeff_ = std::max(
            param_to_constraint_coeff_ * param, medium_constraint_coeff_);
        limit_constraint_coeff();
        NUM_COLLECT_LOG_TRACE(this->logger(), "param={}, constraint_coeff={}",
            param, constraint_coeff_);

        iterations_ = 0;
        derivative_ = (*derivative_matrix_) * solution;
        lagrange_multiplier_ = data_type::Zero(derivative_matrix_->rows());
        temp_solution_ = solution;
        temp_data_ = data_type::Zero(data_->rows());
        temp_derivative_ = data_type::Zero(derivative_matrix_->rows());
        residual_ = (*coeff_) * solution - (*data_);
        update_rate_ = std::numeric_limits<scalar_type>::infinity();
        primal_residual_ = std::numeric_limits<scalar_type>::infinity();
        dual_residual_ = std::numeric_limits<scalar_type>::infinity();
        primal_residual_rate_ = std::numeric_limits<scalar_type>::infinity();
        dual_residual_rate_ = std::numeric_limits<scalar_type>::infinity();

        const scalar_type conjugate_gradient_tolerance_rate =
            rate_of_cg_tol_rate_to_tol_update_rate_ * tol_update_rate_;
        conjugate_gradient_.tolerance_rate(conjugate_gradient_tolerance_rate);
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::iterate
    void iterate(const scalar_type& param, data_type& solution) {
        // Update solution.
        temp_solution_.noalias() =
            static_cast<scalar_type>(2) * coeff_transpose_ * (*data_);
        temp_derivative_ =
            -lagrange_multiplier_ + constraint_coeff_ * derivative_;
        temp_solution_.noalias() +=
            (*derivative_matrix_).transpose() * temp_derivative_;
        previous_solution_ = solution;
        conjugate_gradient_.solve(
            [this](const data_type& target, data_type& result) {
                temp_data_.noalias() = (*coeff_) * target;
                result.noalias() =
                    static_cast<scalar_type>(2) * coeff_transpose_ * temp_data_;
                result.noalias() += constraint_coeff_ * dtd_ * target;
            },
            temp_solution_, solution);
        update_rate_ = (solution - previous_solution_).norm() /
            (solution.norm() + std::numeric_limits<scalar_type>::epsilon());
        residual_.noalias() = (*coeff_) * solution;
        residual_ -= (*data_);

        // Update derivative.
        previous_derivative_ = derivative_;
        derivative_.noalias() = (*derivative_matrix_) * solution;
        derivative_ += lagrange_multiplier_ / constraint_coeff_;
        const scalar_type derivative_shrinkage_threshold =
            param / constraint_coeff_;
        impl::apply_shrinkage_operator(
            derivative_, derivative_shrinkage_threshold);
        update_rate_ += (derivative_ - previous_derivative_).norm() /
            (derivative_.norm() + std::numeric_limits<scalar_type>::epsilon());

        // Update lagrange multiplier.
        lagrange_multiplier_update_.noalias() =
            constraint_coeff_ * (*derivative_matrix_) * solution;
        lagrange_multiplier_update_ -= constraint_coeff_ * derivative_;
        lagrange_multiplier_ += lagrange_multiplier_update_;
        update_rate_ += lagrange_multiplier_update_.norm() /
            (lagrange_multiplier_.norm() +
                std::numeric_limits<scalar_type>::epsilon());

        // Update constraint coefficient.
        primal_residual_ =
            lagrange_multiplier_update_.norm() / constraint_coeff_;
        temp_derivative_ = derivative_ - previous_derivative_;
        temp_solution_ = (*derivative_matrix_).transpose() * temp_derivative_;
        dual_residual_ = constraint_coeff_ * temp_solution_.norm();
        if (primal_residual_ >
            tol_primal_dual_residuals_ratio_ * dual_residual_) {
            constraint_coeff_ *= constraint_coeff_change_ratio_;
            limit_constraint_coeff();
            NUM_COLLECT_LOG_TRACE(this->logger(),
                "Increased constraint_coeff: {}", constraint_coeff_);
        } else if (dual_residual_ >
            tol_primal_dual_residuals_ratio_ * primal_residual_) {
            constraint_coeff_ /= constraint_coeff_change_ratio_;
            limit_constraint_coeff();
            NUM_COLLECT_LOG_TRACE(this->logger(),
                "Decreased constraint_coeff: {}", constraint_coeff_);
        }
        primal_residual_rate_ = primal_residual_ /
            (derivative_.norm() + std::numeric_limits<scalar_type>::epsilon());
        temp_solution_ =
            (*derivative_matrix_).transpose() * lagrange_multiplier_;
        dual_residual_rate_ = dual_residual_ /
            (temp_solution_.norm() +
                std::numeric_limits<scalar_type>::epsilon());

        ++iterations_;
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::is_stop_criteria_satisfied
    [[nodiscard]] auto is_stop_criteria_satisfied(
        const data_type& solution) const -> bool {
        (void)solution;
        return (iterations() > max_iterations()) ||
            ((primal_residual_ < absolute_tol_ ||
                 primal_residual_rate_ < relative_tol_) &&
                (dual_residual_ < absolute_tol_ ||
                    dual_residual_rate_ < relative_tol_)) ||
            (update_rate_ < tol_update_rate_);
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
        iteration_logger.template append<scalar_type>(
            "Pri.Res.", &this_type::primal_residual_rate);
        iteration_logger.template append<scalar_type>(
            "Dual.Res.", &this_type::dual_residual_rate);
    }

    //! \copydoc num_collect::regularization::regularized_solver_base::residual_norm
    [[nodiscard]] auto residual_norm(const data_type& solution) const
        -> scalar_type {
        return ((*coeff_) * solution - (*data_)).squaredNorm();
    }

    //! \copydoc num_collect::regularization::regularized_solver_base::regularization_term
    [[nodiscard]] auto regularization_term(const data_type& solution) const
        -> scalar_type {
        return (*derivative_matrix_ * solution).template lpNorm<1>();
    }

    //! \copydoc num_collect::regularization::regularized_solver_base::change_data
    void change_data(const data_type& data) { data_ = &data; }

    //! \copydoc num_collect::regularization::regularized_solver_base::calculate_data_for
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
        const data_type approx_order_of_derivative =
            *derivative_matrix_ * approx_order_of_solution;
        const scalar_type approx_order_of_param = (*data_).squaredNorm() /
            approx_order_of_derivative.cwiseAbs().sum();
        const scalar_type minimum_param =
            approx_order_of_derivative.cwiseAbs().maxCoeff() * tol_update_rate_;
        NUM_COLLECT_LOG_INFO(this->logger(),
            "approx_order_of_param={}, minimum_param={}", approx_order_of_param,
            minimum_param);
        // Experimentally selected parameters.
        constexpr auto coeff_min_param = static_cast<scalar_type>(1e-6);
        constexpr auto coeff_max_param = static_cast<scalar_type>(1e+2);
        return {
            std::max(approx_order_of_param * coeff_min_param, minimum_param),
            approx_order_of_param * coeff_max_param};
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
     * \brief Get the primal residual.
     *
     * \return Primal residual.
     */
    [[nodiscard]] auto primal_residual() const noexcept -> scalar_type {
        return primal_residual_;
    }

    /*!
     * \brief Get the dual residual.
     *
     * \return Dual residual.
     */
    [[nodiscard]] auto dual_residual() const noexcept -> scalar_type {
        return dual_residual_;
    }

    /*!
     * \brief Get the rate of the primal residual.
     *
     * \return Rate of the primal residual.
     */
    [[nodiscard]] auto primal_residual_rate() const noexcept -> scalar_type {
        return primal_residual_rate_;
    }

    /*!
     * \brief Get the rate of the dual residual.
     *
     * \return Rate of the dual residual.
     */
    [[nodiscard]] auto dual_residual_rate() const noexcept -> scalar_type {
        return dual_residual_rate_;
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
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "Maximum number of iterations must be a positive integer.");
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
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "Tolerance of update rate of the solution must be a positive "
            "value.");
        tol_update_rate_ = value;
        return *this;
    }

private:
    /*!
     * \brief Limit the coefficient of the constraint to a reasonable range.
     */
    void limit_constraint_coeff() {
        constexpr auto scale = static_cast<scalar_type>(1e+4);
        const scalar_type max_constraint_coeff =
            medium_constraint_coeff_ * scale;
        const scalar_type min_constraint_coeff =
            medium_constraint_coeff_ / scale;
        constraint_coeff_ = std::min(constraint_coeff_, max_constraint_coeff);
        constraint_coeff_ = std::max(constraint_coeff_, min_constraint_coeff);
    }

    //! Coefficient matrix to compute data vector.
    const coeff_type* coeff_{nullptr};

    //! Matrix to compute derivative.
    const derivative_matrix_type* derivative_matrix_{nullptr};

    //! Data vector.
    const data_type* data_{nullptr};

    //! Number of iterations.
    index_type iterations_{};

    //! Transposed coefficient matrix.
    coeff_type coeff_transpose_{};

    //! Matrix \f$D^\top D\f$.
    derivative_matrix_type dtd_{};

    //! Derivative.
    data_type derivative_{};

    //! Lagrange multiplier.
    data_type lagrange_multiplier_{};

    //! Temporary vector for the update of the solution.
    data_type temp_solution_{};

    //! Temporary vector with the size of data.
    data_type temp_data_{};

    //! Temporary vector with the size of the derivative.
    data_type temp_derivative_{};

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

    /*!
     * \brief Default ratio of coefficient of the constraint to regularization
     * parameter. (Value in \cite Li2016.)
     */
    static constexpr auto default_param_to_constraint_coeff =
        static_cast<scalar_type>(10);

    //! Coefficient of the constraint to regularization parameter.
    scalar_type param_to_constraint_coeff_{default_param_to_constraint_coeff};

    //! Medium value of the coefficient of the constraint.
    scalar_type medium_constraint_coeff_{};

    //! Coefficient of the constraint.
    scalar_type constraint_coeff_{};

    //! Default tolerance of ratio of primal and dual residuals.
    static constexpr auto default_tol_primal_dual_residuals_ratio =
        static_cast<scalar_type>(10);

    //! Tolerance of ratio of primal and dual residuals.
    scalar_type tol_primal_dual_residuals_ratio_{
        default_tol_primal_dual_residuals_ratio};

    //! Default ratio to change the constraint coefficient.
    static constexpr auto default_constraint_coeff_change_ratio =
        static_cast<scalar_type>(2);

    //! Ratio to change the constraint coefficient.
    scalar_type constraint_coeff_change_ratio_{
        default_constraint_coeff_change_ratio};

    //! Primal residual.
    scalar_type primal_residual_{};

    //! Rate of the primal residual.
    scalar_type primal_residual_rate_{};

    //! Dual residual.
    scalar_type dual_residual_{};

    //! Rate of the dual residual.
    scalar_type dual_residual_rate_{};

    //! Default maximum number of iterations.
    static constexpr index_type default_max_iterations = 10000;

    //! Maximum number of iterations.
    index_type max_iterations_{default_max_iterations};

    //! Default tolerance of update rate of the solution.
    static constexpr auto default_tol_update_rate =
        static_cast<scalar_type>(1e-4);

    //! Tolerance of update rate of the solution.
    scalar_type tol_update_rate_{default_tol_update_rate};

    //! Default absolute tolerance.
    static constexpr auto default_absolute_tol = static_cast<scalar_type>(1e-4);

    //! Absolute tolerance.
    scalar_type absolute_tol_{default_absolute_tol};

    //! Default relative tolerance.
    static constexpr auto default_relative_tol = static_cast<scalar_type>(1e-4);

    //! Relative tolerance.
    scalar_type relative_tol_{default_relative_tol};

    //! Default value of the ratio of the rate of tolerance in CG method to the tolerance of update rate of the solution.
    static constexpr auto default_rate_of_cg_tol_rate_to_tol_update_rate =
        static_cast<scalar_type>(1e-2);

    //! Rate of the ratio of the rate of tolerance in CG method to the tolerance of update rate of the solution.
    scalar_type rate_of_cg_tol_rate_to_tol_update_rate_{
        default_rate_of_cg_tol_rate_to_tol_update_rate};
};

}  // namespace num_collect::regularization
