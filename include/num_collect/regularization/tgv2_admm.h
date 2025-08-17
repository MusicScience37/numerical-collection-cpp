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
 * \brief Definition of tgv2_admm class.
 */
#pragma once

#include <algorithm>
#include <limits>
#include <utility>

#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCore>

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/linear/impl/operator_conjugate_gradient.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/regularization/impl/apply_shrinkage_operator.h"
#include "num_collect/regularization/impl/approximate_max_eigen_aat.h"
#include "num_collect/regularization/impl/weak_coeff_param.h"  // IWYU pragma: keep
#include "num_collect/regularization/iterative_regularized_solver_base.h"

namespace num_collect::regularization {

//! Tag of tgv2_admm.
constexpr auto tgv2_admm_tag =
    logging::log_tag_view("num_collect::regularization::tgv2_admm");

/*!
 * \brief Class to solve linear equations with 2nd order total generalized
 * variation (TGV) regularization \cite Bredies2010 using the alternating
 * direction method of multipliers (ADMM) \cite Boyd2010.
 *
 * \tparam Coeff Type of coefficient matrices.
 * \tparam DerivativeMatrix Type of matrices to compute derivatives.
 * \tparam Data Type of data vectors.
 *
 * \note Parallelization in Eigen library is enabled when
 * - `Coeff` is a sparse matrix in row major order, or a dense matrix,
 * - and `DerivativeMatrix` is a sparse matrix in row major order.
 *
 * This class minimizes the following evaluation function \cite Li2016 :
 *
 * \f[
 * E(\boldsymbol{x}, \boldsymbol{z}) =
 * \| A \boldsymbol{x} - \boldsymbol{y} \|_2^2
 * + \lambda (\| D \boldsymbol{x} - \boldsymbol{z} \|_1
 * + \alpha \| E \boldsymbol{z} \|_1)
 * \f]
 *
 * where variables are defined as follows:
 *
 * - \f$A\f$ is a coefficient matrix.
 * - \f$\boldsymbol{x}\f$ is a solution vector.
 * - \f$\boldsymbol{y}\f$ is a data vector.
 * - \f$\boldsymbol{z}\f$ is a component of the 1st order derivative
 *   of the solution which is used to compute the 2nd order derivative.
 * - \f$\lambda\f$ is a regularization parameter.
 * - \f$D\f$ is a matrix to compute the 1st order derivatives.
 * - \f$E\f$ is a matrix to compute the 2nd order derivative
 *   from the 1st order derivative.
 *
 * For solving the above problem using ADMM, this class solves the following
 * optimization problem:
 *
 * \f[
 * \begin{aligned}
 *     \text{minimize} \hspace{1em} & \|A \boldsymbol{x} - \boldsymbol{y}\|_2^2
 *     + \lambda \left( \|\boldsymbol{s}\|_1 + \alpha \|\boldsymbol{t}\|_1
 * \right)
 *     \\
 *     \text{s.t.} \hspace{1em}     & D \boldsymbol{x} - \boldsymbol{z} =
 * \boldsymbol{s} \\ & E \boldsymbol{z} = \boldsymbol{t}
 * \end{aligned}
 * \f]
 *
 * where additional variables are defined as follows:
 *
 * - \f$\boldsymbol{s}\f$ is a component of the 1st order derivative of the
 *   solution for regularization.
 * - \f$\boldsymbol{t}\f$ is the 2nd order derivative of the solution.
 *
 * This class uses the following augmented Lagrangian function:
 *
 * \f[
 *   L_{\rho}(\boldsymbol{x}, \boldsymbol{z}, \boldsymbol{s}, \boldsymbol{t},
 * \boldsymbol{p}, \boldsymbol{u})
 *   \equiv
 *   \|A \boldsymbol{x} - \boldsymbol{y}\|_2^2
 *   + \lambda \left( \|\boldsymbol{s}\|_1 + \alpha \|\boldsymbol{t}\|_1 \right)
 *   + \boldsymbol{p}^\top (D \boldsymbol{x} - \boldsymbol{z} - \boldsymbol{s})
 *   + \boldsymbol{u}^\top (E \boldsymbol{z} - \boldsymbol{t})
 *   + \frac{\rho}{2} \|D \boldsymbol{x} - \boldsymbol{z} - \boldsymbol{s}\|_2^2
 *   + \frac{\rho}{2} \|E \boldsymbol{z} - \boldsymbol{t}\|_2^2
 * \f]
 *
 * where additional variables are defined as follows:
 *
 * - \f$\boldsymbol{p}\f$ is a Lagrange multiplier for the 1st order
 *   derivative constraint.
 * - \f$\boldsymbol{u}\f$ is a Lagrange multiplier for the 2nd order
 *   derivative constraint.
 * - \f$\rho\f$ is a coefficient of the quadratic term for the constraints.
 *
 * This class uses the following update rules:
 *
 * \f[
 * \begin{aligned}
 *     \boldsymbol{x}_{k+1}
 *      & = (2 A^\top A + \rho D^\top D)^{-1} \left(
 *     2 A^\top \boldsymbol{y} - D^\top \boldsymbol{p}_k + \rho D^\top
 * \boldsymbol{z}_k + \rho D^\top \boldsymbol{s}_k
 *     \right)
 *     \\
 *     \boldsymbol{z}_{k+1}
 *      & = (\rho I + \rho E^\top E)^{-1} \left(
 *     \boldsymbol{p}_k - E^\top \boldsymbol{u}_k + \rho D \boldsymbol{x}_{k+1}
 * - \rho \boldsymbol{s}_k + \rho E^\top \boldsymbol{t}_k
 *     \right)
 *     \\
 *     \boldsymbol{s}_{k+1}
 *      & = \mathcal{T}_{\lambda/\rho} \left( D \boldsymbol{x}_{k+1} -
 * \boldsymbol{z}_{k+1} + \frac{\boldsymbol{p}_k}{\rho} \right)
 *     \\
 *     \boldsymbol{t}_{k+1}
 *      & = \mathcal{T}_{\lambda/\rho \alpha} \left( E \boldsymbol{z}_{k+1} +
 * \frac{\boldsymbol{u}_k}{\rho} \right)
 *     \\
 *     \boldsymbol{p}_{k+1}
 *      & = \boldsymbol{p}_k + \rho (D \boldsymbol{x}_{k+1} -
 * \boldsymbol{z}_{k+1} - \boldsymbol{s}_{k+1})
 *     \\
 *     \boldsymbol{u}_{k+1}
 *      & = \boldsymbol{u}_k + \rho (E \boldsymbol{z}_{k+1} -
 * \boldsymbol{t}_{k+1})
 * \end{aligned}
 * \f]
 *
 * where \f$\mathcal{T}_\lambda\f$ is the thresholding operator
 * implemented in \ref
 * num_collect::regularization::impl::apply_shrinkage_operator function.
 *
 * This class uses the following variable names:
 *
 * | In Formula | In C++ |
 * | :--- | :--- |
 * | \f$\boldsymbol{x}\f$ | `solution` |
 * | \f$\boldsymbol{y}\f$ | `data` |
 * | \f$\boldsymbol{z}\f$ | `z` |
 * | \f$\boldsymbol{s}\f$ | `s` |
 * | \f$\boldsymbol{t}\f$ | `t` |
 * | \f$\boldsymbol{p}\f$ | `p` |
 * | \f$\boldsymbol{u}\f$ | `u` |
 * | \f$A\f$ | `coeff` |
 * | \f$D\f$ | `first_derivative_matrix` |
 * | \f$E\f$ | `second_derivative_matrix` |
 * | \f$\lambda\f$ | `param` |
 * | \f$\alpha\f$ | `second_derivative_ratio` |
 * | \f$\rho\f$ | `constraint_coeff` |
 */
template <typename Coeff, typename DerivativeMatrix,
    base::concepts::dense_vector Data>
    requires((base::concepts::sparse_matrix<Coeff> ||
                 base::concepts::dense_matrix<Coeff>) &&
        base::concepts::sparse_matrix<DerivativeMatrix>)
class tgv2_admm : public iterative_regularized_solver_base<
                      tgv2_admm<Coeff, DerivativeMatrix, Data>, Data> {
public:
    //! This type.
    using this_type = tgv2_admm<Coeff, DerivativeMatrix, Data>;

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
    tgv2_admm() : base_type(tgv2_admm_tag) {
        this->configure_child_algorithm_logger_if_exists(
            conjugate_gradient_solution_);
        this->configure_child_algorithm_logger_if_exists(conjugate_gradient_z_);
    }

    /*!
     * \brief Compute internal parameters.
     *
     * \param[in] coeff Coefficient matrix.
     * \param[in] first_derivative_matrix Matrix to compute the first
     * derivative.
     * \param[in] second_derivative_matrix Matrix to compute the second
     * derivative from the first order derivative.
     * \param[in] data Data vector.
     *
     * \note Pointers to the arguments are saved in this object, so don't
     * destruct those arguments.
     * \note Call this before init.
     */
    void compute(const Coeff& coeff,
        const DerivativeMatrix& first_derivative_matrix,
        const DerivativeMatrix& second_derivative_matrix, const Data& data) {
        coeff_ = &coeff;
        first_derivative_matrix_ = &first_derivative_matrix;
        second_derivative_matrix_ = &second_derivative_matrix;
        data_ = &data;
        // Sizes will be checked in init.
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::init
    void init(const scalar_type& param, data_type& solution) {
        (void)param;

        NUM_COLLECT_PRECONDITION(coeff_ != nullptr, this->logger(),
            "Coefficient matrix is not set.");
        NUM_COLLECT_PRECONDITION(first_derivative_matrix_ != nullptr,
            this->logger(), "First order derivative matrix is not set.");
        NUM_COLLECT_PRECONDITION(second_derivative_matrix_ != nullptr,
            this->logger(), "Second order derivative matrix is not set.");
        NUM_COLLECT_PRECONDITION(
            data_ != nullptr, this->logger(), "Data vector is not set.");

        NUM_COLLECT_PRECONDITION(coeff_->rows() == data_->rows(),
            this->logger(),
            "Coefficient matrix and data vector must have the same number of "
            "rows.");
        NUM_COLLECT_PRECONDITION(coeff_->cols() == solution.rows(),
            this->logger(),
            "The number of columns in the coefficient matrix must match the "
            "number of rows in solution vector.");
        NUM_COLLECT_PRECONDITION(data_->cols() == solution.cols(),
            this->logger(),
            "Data and solution must have the same number of columns.");
        NUM_COLLECT_PRECONDITION(
            first_derivative_matrix_->cols() == solution.rows(), this->logger(),
            "The number of columns in the first order derivative matrix must "
            "match the number of rows in solution vector.");
        NUM_COLLECT_PRECONDITION(second_derivative_matrix_->cols() ==
                first_derivative_matrix_->rows(),
            this->logger(),
            "The number of columns in the second order derivative matrix must "
            "match the number of rows in the first order derivative matrix.");

        iterations_ = 0;

        coeff_transpose_ = coeff_->transpose();

        dtd_ = constraint_coeff_ * (*first_derivative_matrix_).transpose() *
            (*first_derivative_matrix_);

        z_coeff_.resize(second_derivative_matrix_->cols(),
            second_derivative_matrix_->cols());
        z_coeff_.setIdentity();
        // Matrix E^\top E. Without this temporary matrix, compilation fails.
        const derivative_matrix_type ete =
            (*second_derivative_matrix_).transpose() *
            (*second_derivative_matrix_);
        z_coeff_ += ete;
        z_coeff_ *= constraint_coeff_;

        // Set variables to one of feasible solutions.
        z_ = data_type::Zero(first_derivative_matrix_->rows());
        s_ = (*first_derivative_matrix_) * solution;
        t_ = data_type::Zero(second_derivative_matrix_->rows());
        p_ = data_type::Zero(first_derivative_matrix_->rows());
        u_ = data_type::Zero(second_derivative_matrix_->rows());

        // Pre-allocate temporary vectors.
        temp_solution_ = data_type::Zero(solution.rows());
        temp_data_ = data_type::Zero(data_->rows());
        temp_z_ = data_type::Zero(z_.rows());
        temp_t_ = data_type::Zero(t_.rows());
        previous_solution_ = data_type::Zero(solution.rows());
        previous_z_ = data_type::Zero(z_.rows());
        previous_s_ = data_type::Zero(s_.rows());
        previous_t_ = data_type::Zero(t_.rows());
        p_update_ = data_type::Zero(p_.rows());
        u_update_ = data_type::Zero(u_.rows());

        residual_ = (*coeff_) * solution - (*data_);
        update_rate_ = std::numeric_limits<scalar_type>::infinity();

        const scalar_type conjugate_gradient_tolerance_rate =
            rate_of_cg_tol_rate_to_tol_update_rate_ * tol_update_rate_;
        conjugate_gradient_solution_.tolerance_rate(
            conjugate_gradient_tolerance_rate);
        conjugate_gradient_z_.tolerance_rate(conjugate_gradient_tolerance_rate);
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::iterate
    void iterate(const scalar_type& param, data_type& solution) {
        update_rate_ = static_cast<scalar_type>(0);

        update_solution(param, solution);
        update_z(param, solution);
        update_s(param, solution);
        update_t(param, solution);
        update_p(param, solution);
        update_u(param, solution);

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

    //! \copydoc num_collect::regularization::regularized_solver_base::residual_norm
    [[nodiscard]] auto residual_norm(const data_type& solution) const
        -> scalar_type {
        return ((*coeff_) * solution - (*data_)).squaredNorm();
    }

    //! \copydoc num_collect::regularization::regularized_solver_base::regularization_term
    [[nodiscard]] auto regularization_term(const data_type& solution) const
        -> scalar_type {
        return (*first_derivative_matrix_ * solution).template lpNorm<1>();
    }

    //! \copydoc num_collect::regularization::regularized_solver_base::change_data
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
            (*first_derivative_matrix_ * approx_order_of_solution)
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
    auto max_iterations(index_type value) -> this_type& {
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
    auto tol_update_rate(scalar_type value) -> this_type& {
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "Tolerance of update rate of the solution must be a positive "
            "value.");
        tol_update_rate_ = value;
        return *this;
    }

private:
    /*!
     * \brief Update the solution.
     *
     * \param[in] param Regularization parameter.
     * \param[in,out] solution Solution vector.
     */
    void update_solution(const scalar_type& param, data_type& solution) {
        (void)param;  // Not used for update of solution.

        temp_z_ = -p_ + constraint_coeff_ * z_ + constraint_coeff_ * s_;
        temp_solution_.noalias() =
            static_cast<scalar_type>(2) * (*coeff_).transpose() * (*data_);
        temp_solution_.noalias() +=
            (*first_derivative_matrix_).transpose() * temp_z_;
        previous_solution_ = solution;
        conjugate_gradient_solution_.solve(
            [this](const data_type& target, data_type& result) {
                result.noalias() = dtd_ * target;
                temp_data_.noalias() = (*coeff_) * target;
                result.noalias() +=
                    static_cast<scalar_type>(2) * coeff_transpose_ * temp_data_;
            },
            temp_solution_, solution);
        update_rate_ += (solution - previous_solution_).norm() /
            (solution.norm() + std::numeric_limits<scalar_type>::epsilon());
        residual_.noalias() = (*coeff_) * solution;
        residual_ -= (*data_);
    }

    /*!
     * \brief Update z_.
     *
     * \param[in] param Regularization parameter.
     * \param[in] solution Solution vector.
     */
    void update_z(const scalar_type& param, const data_type& solution) {
        (void)param;  // Not used for update of z_.

        temp_z_ = p_;
        temp_z_.noalias() -= (*second_derivative_matrix_).transpose() * u_;
        temp_z_.noalias() +=
            constraint_coeff_ * (*first_derivative_matrix_) * solution;
        temp_z_.noalias() -= constraint_coeff_ * s_;
        temp_z_.noalias() +=
            constraint_coeff_ * (*second_derivative_matrix_).transpose() * t_;
        previous_z_ = z_;
        conjugate_gradient_z_.solve(
            [this](const data_type& target, data_type& result) {
                result.noalias() = z_coeff_ * target;
            },
            temp_z_, z_);
        update_rate_ += (z_ - previous_z_).norm() /
            (z_.norm() + std::numeric_limits<scalar_type>::epsilon());
    }

    /*!
     * \brief Update s_.
     *
     * \param[in] param Regularization parameter.
     * \param[in] solution Solution vector.
     */
    void update_s(const scalar_type& param, const data_type& solution) {
        previous_s_ = s_;
        s_.noalias() = (*first_derivative_matrix_) * solution;
        s_ += -z_ + p_ / constraint_coeff_;
        impl::apply_shrinkage_operator(s_, param / constraint_coeff_);
        update_rate_ += (s_ - previous_s_).norm() /
            (s_.norm() + std::numeric_limits<scalar_type>::epsilon());
    }

    /*!
     * \brief Update t_.
     *
     * \param[in] param Regularization parameter.
     * \param[in] solution Solution vector.
     */
    void update_t(const scalar_type& param, const data_type& solution) {
        (void)solution;  // Not used for update of t_.

        previous_t_ = t_;
        t_.noalias() = (*second_derivative_matrix_) * z_;
        t_ += u_ / constraint_coeff_;
        impl::apply_shrinkage_operator(
            t_, param / constraint_coeff_ / second_derivative_ratio_);
        update_rate_ += (t_ - previous_t_).norm() /
            (t_.norm() + std::numeric_limits<scalar_type>::epsilon());
    }

    /*!
     * \brief Update p_.
     *
     * \param[in] param Regularization parameter.
     * \param[in] solution Solution vector.
     */
    void update_p(const scalar_type& param, const data_type& solution) {
        (void)param;  // Not used for update of p_.

        p_update_.noalias() =
            constraint_coeff_ * (*first_derivative_matrix_) * solution;
        p_update_ += constraint_coeff_ * (-z_ - s_);
        p_ += p_update_;
        update_rate_ += p_update_.norm() /
            (p_.norm() + std::numeric_limits<scalar_type>::epsilon());
    }

    /*!
     * \brief Update u_.
     *
     * \param[in] param Regularization parameter.
     * \param[in] solution Solution vector.
     */
    void update_u(const scalar_type& param, const data_type& solution) {
        (void)param;     // Not used for update of u_.
        (void)solution;  // Not used for update of u_.

        u_update_.noalias() =
            constraint_coeff_ * (*second_derivative_matrix_) * z_;
        u_update_ -= constraint_coeff_ * t_;
        u_ += u_update_;
        update_rate_ += u_update_.norm() /
            (u_.norm() + std::numeric_limits<scalar_type>::epsilon());
    }

    //! Coefficient matrix to compute data vector.
    const coeff_type* coeff_{nullptr};

    //! Matrix to compute the first order derivative.
    const derivative_matrix_type* first_derivative_matrix_{nullptr};

    //! Matrix to compute the second order derivative.
    const derivative_matrix_type* second_derivative_matrix_{nullptr};

    //! Data vector.
    const data_type* data_{nullptr};

    //! Number of iterations.
    index_type iterations_{};

    //! Transposed coefficient matrix.
    coeff_type coeff_transpose_{};

    //! Matrix \f$D^\top D\f$.
    derivative_matrix_type dtd_{};

    //! Coefficient matrix to solve the linear equation of z_.
    derivative_matrix_type z_coeff_{};

    //! Component of the 1st order derivative.
    data_type z_{};

    //! Component of the 1st order derivative for regularization.
    data_type s_{};

    //! 2nd order derivative of the solution.
    data_type t_{};

    //! Lagrange multiplier for the 1st order derivative constraint.
    data_type p_{};

    //! Lagrange multiplier for the 2nd order derivative constraint.
    data_type u_{};

    //! Temporary vector for the update of the solution.
    data_type temp_solution_{};

    //! Temporary vector with the size of data.
    data_type temp_data_{};

    //! Temporary vector for the update of the 1st order derivative.
    data_type temp_z_{};

    //! Temporary vector for the update of the 2nd order derivative.
    data_type temp_t_{};

    //! Previous solution.
    data_type previous_solution_{};

    //! Previous value of z_.
    data_type previous_z_{};

    //! Previous value of s_.
    data_type previous_s_{};

    //! Previous value of t_.
    data_type previous_t_{};

    //! Update of p_.
    data_type p_update_{};

    //! Update of u_.
    data_type u_update_{};

    //! Residual vector.
    data_type residual_{};

    //! Rate of norm of the update of the variables in the last iteration.
    scalar_type update_rate_{};

    //! Conjugate gradient solver for update of the solution.
    linear::impl::operator_conjugate_gradient<data_type>
        conjugate_gradient_solution_{};

    //! Conjugate gradient solver for update of the 1st order derivative.
    linear::impl::operator_conjugate_gradient<data_type>
        conjugate_gradient_z_{};

    //! Default ratio of regularization term for the 2nd order derivative.
    static constexpr auto default_second_derivative_ratio =
        static_cast<scalar_type>(1);

    //! Ratio of regularization term for the 2nd order derivative.
    scalar_type second_derivative_ratio_{default_second_derivative_ratio};

    //! Default coefficient of the constraint for the derivative.
    static constexpr auto default_constraint_coeff =
        static_cast<scalar_type>(1);

    //! Coefficient of the constraint for the derivative.
    scalar_type constraint_coeff_{default_constraint_coeff};

    //! Default maximum number of iterations.
    static constexpr index_type default_max_iterations = 10000;

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
