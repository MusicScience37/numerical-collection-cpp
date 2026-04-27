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
 * \brief Definition of inexact_newton_full_update_equation_solver class.
 */
#pragma once

#include <cmath>
#include <optional>

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/LU>

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/iterative_solver_base.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/differentiable_problem.h"
#include "num_collect/ode/concepts/mass_problem.h"
#include "num_collect/ode/concepts/multi_variate_differentiable_problem.h"
#include "num_collect/ode/concepts/single_variate_differentiable_problem.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/ode_errors.h"

namespace num_collect::ode::runge_kutta {

//! Log tag.
constexpr auto inexact_newton_full_update_equation_solver_tag =
    logging::log_tag_view(
        "num_collect::ode::runge_kutta::inexact_newton_full_update_equation_"
        "solver");

/*!
 * \brief Class to solve equations of implicit updates in full implicit
 * Runge-Kutta methods using inexact Newton method.
 *
 * \tparam Problem Type of the problem.
 * \tparam NumStages Number of stages of the formula.
 *
 * This class solves following equation using the stop criterion written in
 * \cite Hairer1991 :
 *
 * \f[
 * \boldsymbol{z}_i = h \sum_{j=1}^s a_{ij}
 * \boldsymbol{f}\left(t + b_i h, \boldsymbol{y}(t) + \boldsymbol{z}_j \right)
 * \f]
 *
 * \note This class does not support changing mass.
 * If the problem has mass, the mass is assumed to be constant.
 */
template <concepts::differentiable_problem Problem, int NumStages>
class inexact_newton_full_update_equation_solver;

/*!
 * \brief Class to solve equations of implicit updates in full implicit
 * Runge-Kutta methods using inexact Newton method.
 *
 * \tparam Problem Type of the problem.
 * \tparam NumStages Number of stages of the formula.
 *
 * This class solves following equation using the stop criterion written in
 * \cite Hairer1991 :
 *
 * \f[
 * \boldsymbol{z}_i = h \sum_{j=1}^s a_{ij}
 * \boldsymbol{f}\left(t + b_i h, \boldsymbol{y}(t) + \boldsymbol{z}_j \right)
 * \f]
 *
 * \note This class does not support changing mass.
 * If the problem has mass, the mass is assumed to be constant.
 */
template <concepts::single_variate_differentiable_problem Problem,
    int NumStages>
class inexact_newton_full_update_equation_solver<Problem, NumStages>
    : public iterative_solver_base<
          inexact_newton_full_update_equation_solver<Problem, NumStages>> {
public:
    //! This class.
    using this_type =
        inexact_newton_full_update_equation_solver<Problem, NumStages>;

    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(base::concepts::real_scalar<scalar_type>,
        "Current implementation only supports real scalars.");

    //! Type of the matrix of coefficients of intermediate slopes.
    using slope_coeff_matrix_type =
        Eigen::Matrix<scalar_type, NumStages, NumStages>;

    //! Type of the vector of intermediate updates.
    using update_coeff_vector_type = Eigen::Vector<scalar_type, NumStages>;

    //! Type of the vector of intermediate updates.
    using update_vector_type = Eigen::Vector<scalar_type, NumStages>;

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] slope_coeffs Coefficients of intermediate slopes in the
     * formula.
     * \param[in] time_coeffs Coefficients of time in the formula.
     * \param[in] update_coeffs Coefficients of intermediate updates in the
     * formula.
     */
    inexact_newton_full_update_equation_solver(
        const slope_coeff_matrix_type& slope_coeffs,
        const update_coeff_vector_type& time_coeffs,
        const update_coeff_vector_type& update_coeffs)
        : iterative_solver_base<
              inexact_newton_full_update_equation_solver<Problem, NumStages>>(
              inexact_newton_full_update_equation_solver_tag),
          slope_coeffs_(slope_coeffs),
          time_coeffs_(time_coeffs),
          update_coeffs_(update_coeffs) {}

    /*!
     * \brief Initialize the solver to calculate updates.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variables.
     * \param[in,out] solution Solution of updates. The initial values are used
     * as the initial guess of the solution.
     */
    void init(problem_type& problem, scalar_type time, scalar_type step_size,
        const variable_type& variable, update_vector_type& solution) {
        problem_ = &problem;
        time_ = time;
        step_size_ = step_size;
        variable_ = variable;
        solution_ = &solution;

        problem_->evaluate_on(time_, variable_,
            evaluation_type{
                .diff_coeff = true, .jacobian = true, .mass = use_mass});

        if constexpr (use_mass) {
            coeff_matrix_ =
                update_vector_type::Constant(problem.mass()).asDiagonal();
        } else {
            coeff_matrix_ = slope_coeff_matrix_type::Identity();
        }
        coeff_matrix_ -= step_size_ * problem_->jacobian() * slope_coeffs_;
        solver_.compute(coeff_matrix_);

        update_norm_.reset();
        if (update_reduction_rate_) {
            constexpr auto exponent = static_cast<scalar_type>(0.8);
            constexpr auto min_rate = static_cast<scalar_type>(0.5);
            using std::pow;
            *update_reduction_rate_ = pow(*update_reduction_rate_, exponent);
            if (*update_reduction_rate_ < min_rate) {
                *update_reduction_rate_ = min_rate;
            }
        }
        iterations_ = 0;
    }

    /*!
     * \brief Iterate the algorithm once.
     *
     * \warning Any required initializations (with init
     * functions) are assumed to have been done.
     */
    void iterate() {
        NUM_COLLECT_PRECONDITION(problem_ != nullptr && solution_ != nullptr,
            this->logger(), "Initialization must be done before iterations.");

        for (int i = 0; i < NumStages; ++i) {
            temp_variable_ = variable_ + (*solution_)(i);
            problem_->evaluate_on(time_ + time_coeffs_(i) * step_size_,
                temp_variable_, evaluation_type{.diff_coeff = true});
            slopes_(i) = problem_->diff_coeff();
        }
        if constexpr (use_mass) {
            residual_ = problem_->mass() * (*solution_);
        } else {
            residual_ = (*solution_);
        }
        residual_.noalias() -= step_size_ * slope_coeffs_ * slopes_;
        update_ = solver_.solve(residual_);
        if (!update_.array().isFinite().all()) {
            NUM_COLLECT_ODE_THROW_LINEAR_SOLVER_FAILURE(this->logger(),
                "Failed to solve an equation. step_size={}.", step_size_);
        }
        *solution_ -= update_;

        scalar_type update_norm = static_cast<scalar_type>(0);
        for (int i = 0; i < NumStages; ++i) {
            const scalar_type stage_update_norm =
                tolerances().calc_norm(variable_, update_(i));
            update_norm += stage_update_norm * stage_update_norm;
        }
        update_norm = std::sqrt(update_norm);
        if (update_norm_) {
            update_reduction_rate_ = update_norm / (*update_norm_);
        }
        update_norm_ = update_norm;

        ++iterations_;
    }

    /*!
     * \brief Determine if stopping criteria of the algorithm are satisfied.
     *
     * \return If stopping criteria of the algorithm are satisfied.
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        bool converged = false;
        if (update_norm_ && update_reduction_rate_ &&
            *update_reduction_rate_ < static_cast<scalar_type>(1)) {
            converged =
                (*update_reduction_rate_ /
                    (static_cast<scalar_type>(1) - *update_reduction_rate_)) *
                    (*update_norm_) <=
                tolerance_rate_;
        }

        constexpr index_type max_iterations = 1000;  // safe guard
        return converged || (iterations_ > max_iterations);
    }

    /*!
     * \brief Configure an iteration logger.
     *
     * \param[in] iteration_logger Iteration logger.
     */
    void configure_iteration_logger(
        logging::iterations::iteration_logger<this_type>& iteration_logger)
        const {
        iteration_logger.template append<index_type>(
            "Iter.", &this_type::iterations);
        iteration_logger.template append<scalar_type>(
            "Update", &this_type::update_norm);
    }

    /*!
     * \brief Get the norm of update.
     *
     * \return Norm of update.
     */
    [[nodiscard]] auto update_norm() const -> scalar_type {
        if (!update_norm_) {
            return static_cast<scalar_type>(0);
        }
        return *update_norm_;
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const -> index_type { return iterations_; }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val)
        -> inexact_newton_full_update_equation_solver& {
        tolerances_ = val;
        return *this;
    }

    /*!
     * \brief Get the error tolerances.
     *
     * \return Error tolerances.
     */
    [[nodiscard]] auto tolerances() const
        -> const error_tolerances<variable_type>& {
        return tolerances_;
    }

private:
    //! Coefficients of intermediate slopes.
    slope_coeff_matrix_type slope_coeffs_{};

    //! Coefficients of time.
    update_coeff_vector_type time_coeffs_{};

    //! Coefficients of intermediate updates.
    update_coeff_vector_type update_coeffs_{};

    //! Pointer to the problem.
    problem_type* problem_{nullptr};

    //! Time.
    scalar_type time_{};

    //! Step size.
    scalar_type step_size_{};

    //! Variable.
    variable_type variable_{};

    //! Solution of updates.
    update_vector_type* solution_{nullptr};

    //! Buffer of coefficient matrix.
    slope_coeff_matrix_type coeff_matrix_{};

    //! Solver of the current coefficient matrix.
    Eigen::PartialPivLU<slope_coeff_matrix_type> solver_{};

    //! Temporary variable.
    variable_type temp_variable_{};

    //! Buffer of slopes.
    update_vector_type slopes_{};

    //! Residual vector.
    update_vector_type residual_{};

    //! Newton update vector.
    update_vector_type update_{};

    //! Norm of update.
    std::optional<scalar_type> update_norm_{};

    //! Rate in which update is reduced from the previous step.
    std::optional<scalar_type> update_reduction_rate_{};

    //! Default rate of tolerance in this solver.
    static constexpr auto default_tolerance_rate =
        static_cast<scalar_type>(1e-2);

    //! Rate of tolerance in this solver.
    scalar_type tolerance_rate_{default_tolerance_rate};

    //! Number of iterations.
    index_type iterations_{0};

    //! Error tolerances.
    error_tolerances<variable_type> tolerances_{};
};

/*!
 * \brief Class to solve equations of implicit updates in full implicit
 * Runge-Kutta methods using inexact Newton method.
 *
 * \tparam Problem Type of the problem.
 * \tparam NumStages Number of stages of the formula.
 *
 * This class solves following equation using the stop criterion written in
 * \cite Hairer1991 :
 *
 * \f[
 * \boldsymbol{z}_i = h \sum_{j=1}^s a_{ij}
 * \boldsymbol{f}\left(t + b_i h, \boldsymbol{y}(t) + \boldsymbol{z}_j \right)
 * \f]
 *
 * \note This class does not support changing mass.
 * If the problem has mass, the mass is assumed to be constant.
 * \note Sparse Jacobian matrices are not supported due to the limitation of
 * Eigen's sparse matrices which do not support block operations.
 */
template <concepts::multi_variate_differentiable_problem Problem, int NumStages>
    requires base::concepts::dense_matrix<typename Problem::jacobian_type>
class inexact_newton_full_update_equation_solver<Problem, NumStages>
    : public iterative_solver_base<
          inexact_newton_full_update_equation_solver<Problem, NumStages>> {
public:
    //! This class.
    using this_type =
        inexact_newton_full_update_equation_solver<Problem, NumStages>;

    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(base::concepts::real_scalar<scalar_type>,
        "Current implementation only supports real scalars.");

    //! Type of the matrix of coefficients of intermediate slopes.
    using slope_coeff_matrix_type =
        Eigen::Matrix<scalar_type, NumStages, NumStages>;

    //! Type of the vector of intermediate updates.
    using update_coeff_vector_type = Eigen::Vector<scalar_type, NumStages>;

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    //! Number of dimension at compile-time.
    static constexpr int num_dimensions_at_compile_time =
        variable_type::SizeAtCompileTime;

    //! Size of the vectors of intermediate updates at compile-time.
    static constexpr int update_vector_size_at_compile_time =
        (num_dimensions_at_compile_time == Eigen::Dynamic)
        ? Eigen::Dynamic
        : NumStages * num_dimensions_at_compile_time;

    //! Type of the vector of intermediate updates.
    using update_vector_type =
        Eigen::Vector<scalar_type, update_vector_size_at_compile_time>;

    //! Type of matrices of coefficients of linear equations of Newton updates.
    using newton_coeff_matrix_type = Eigen::Matrix<scalar_type,
        update_vector_size_at_compile_time, update_vector_size_at_compile_time>;

    /*!
     * \brief Constructor.
     *
     * \param[in] slope_coeffs Coefficients of intermediate slopes in the
     * formula.
     * \param[in] time_coeffs Coefficients of time in the formula.
     * \param[in] update_coeffs Coefficients of intermediate updates in the
     * formula.
     */
    inexact_newton_full_update_equation_solver(
        const slope_coeff_matrix_type& slope_coeffs,
        const update_coeff_vector_type& time_coeffs,
        const update_coeff_vector_type& update_coeffs)
        : iterative_solver_base<
              inexact_newton_full_update_equation_solver<Problem, NumStages>>(
              inexact_newton_full_update_equation_solver_tag),
          slope_coeffs_(slope_coeffs),
          time_coeffs_(time_coeffs),
          update_coeffs_(update_coeffs) {}

    /*!
     * \brief Initialize the solver to calculate updates.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variables.
     * \param[in,out] solution Solution of updates. The initial values are used
     * as the initial guess of the solution.
     */
    void init(problem_type& problem, scalar_type time, scalar_type step_size,
        const variable_type& variable, update_vector_type& solution) {
        problem_ = &problem;
        time_ = time;
        step_size_ = step_size;
        variable_ = variable;
        solution_ = &solution;

        problem_->evaluate_on(time_, variable_,
            evaluation_type{
                .diff_coeff = true, .jacobian = true, .mass = use_mass});

        const index_type problem_dimension = variable.size();
        const index_type update_vector_dimension = solution.size();
        NUM_COLLECT_PRECONDITION(
            update_vector_dimension == NumStages * problem_dimension,
            this->logger(),
            "The dimension of the update vector must be equal to the number of "
            "stages multiplied by the dimension of the problem.");

        coeff_matrix_.resize(update_vector_dimension, update_vector_dimension);
        if constexpr (use_mass) {
            coeff_matrix_.setZero();
            for (int i = 0; i < NumStages; ++i) {
                coeff_matrix_.block(i * problem_dimension,
                    i * problem_dimension, problem_dimension,
                    problem_dimension) = problem.mass();
            }
        } else {
            coeff_matrix_.setIdentity();
        }
        for (int i = 0; i < NumStages; ++i) {
            for (int j = 0; j < NumStages; ++j) {
                coeff_matrix_.block(i * problem_dimension,
                    j * problem_dimension, problem_dimension,
                    problem_dimension) -=
                    step_size_ * slope_coeffs_(i, j) * problem_->jacobian();
            }
        }
        solver_.compute(coeff_matrix_);

        slopes_.resize(update_vector_dimension);
        residual_.resize(update_vector_dimension);
        update_.resize(update_vector_dimension);

        update_norm_.reset();
        if (update_reduction_rate_) {
            constexpr auto exponent = static_cast<scalar_type>(0.8);
            constexpr auto min_rate = static_cast<scalar_type>(0.5);
            using std::pow;
            *update_reduction_rate_ = pow(*update_reduction_rate_, exponent);
            if (*update_reduction_rate_ < min_rate) {
                *update_reduction_rate_ = min_rate;
            }
        }
        iterations_ = 0;
    }

    /*!
     * \brief Iterate the algorithm once.
     *
     * \warning Any required initializations (with init
     * functions) are assumed to have been done.
     */
    void iterate() {
        NUM_COLLECT_PRECONDITION(problem_ != nullptr && solution_ != nullptr,
            this->logger(), "Initialization must be done before iterations.");

        const index_type problem_dimension = variable_.size();

        for (int i = 0; i < NumStages; ++i) {
            temp_variable_ = variable_ +
                (*solution_).segment(i * problem_dimension, problem_dimension);
            problem_->evaluate_on(time_ + time_coeffs_(i) * step_size_,
                temp_variable_, evaluation_type{.diff_coeff = true});
            slopes_.segment(i * problem_dimension, problem_dimension) =
                problem_->diff_coeff();
        }
        if constexpr (use_mass) {
            for (int i = 0; i < NumStages; ++i) {
                residual_.segment(i * problem_dimension, problem_dimension) =
                    problem_->mass() *
                    (*solution_)
                        .segment(i * problem_dimension, problem_dimension);
            }
        } else {
            residual_ = *solution_;
        }
        for (int i = 0; i < NumStages; ++i) {
            for (int j = 0; j < NumStages; ++j) {
                residual_.segment(i * problem_dimension, problem_dimension) -=
                    step_size_ * slope_coeffs_(i, j) *
                    slopes_.segment(j * problem_dimension, problem_dimension);
            }
        }
        update_ = solver_.solve(residual_);
        if (!update_.array().isFinite().all()) {
            NUM_COLLECT_ODE_THROW_LINEAR_SOLVER_FAILURE(this->logger(),
                "Failed to solve an equation. step_size={}.", step_size_);
        }
        *solution_ -= update_;

        scalar_type update_norm = static_cast<scalar_type>(0);
        for (int i = 0; i < NumStages; ++i) {
            const scalar_type stage_update_norm =
                tolerances().calc_norm(variable_,
                    update_.segment(i * problem_dimension, problem_dimension));
            update_norm += stage_update_norm * stage_update_norm;
        }
        update_norm = std::sqrt(update_norm);
        if (update_norm_) {
            update_reduction_rate_ = update_norm / (*update_norm_);
        }
        update_norm_ = update_norm;

        ++iterations_;
    }

    /*!
     * \brief Determine if stopping criteria of the algorithm are satisfied.
     *
     * \return If stopping criteria of the algorithm are satisfied.
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        bool converged = false;
        if (update_norm_ && update_reduction_rate_ &&
            *update_reduction_rate_ < static_cast<scalar_type>(1)) {
            converged =
                (*update_reduction_rate_ /
                    (static_cast<scalar_type>(1) - *update_reduction_rate_)) *
                    (*update_norm_) <=
                tolerance_rate_;
        }

        constexpr index_type max_iterations = 1000;  // safe guard
        return converged || (iterations_ > max_iterations);
    }

    /*!
     * \brief Configure an iteration logger.
     *
     * \param[in] iteration_logger Iteration logger.
     */
    void configure_iteration_logger(
        logging::iterations::iteration_logger<this_type>& iteration_logger)
        const {
        iteration_logger.template append<index_type>(
            "Iter.", &this_type::iterations);
        iteration_logger.template append<scalar_type>(
            "Update", &this_type::update_norm);
    }

    /*!
     * \brief Get the norm of update.
     *
     * \return Norm of update.
     */
    [[nodiscard]] auto update_norm() const -> scalar_type {
        if (!update_norm_) {
            return static_cast<scalar_type>(0);
        }
        return *update_norm_;
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const -> index_type { return iterations_; }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val)
        -> inexact_newton_full_update_equation_solver& {
        tolerances_ = val;
        return *this;
    }

    /*!
     * \brief Get the error tolerances.
     *
     * \return Error tolerances.
     */
    [[nodiscard]] auto tolerances() const
        -> const error_tolerances<variable_type>& {
        return tolerances_;
    }

private:
    //! Coefficients of intermediate slopes.
    slope_coeff_matrix_type slope_coeffs_{};

    //! Coefficients of time.
    update_coeff_vector_type time_coeffs_{};

    //! Coefficients of intermediate updates.
    update_coeff_vector_type update_coeffs_{};

    //! Pointer to the problem.
    problem_type* problem_{nullptr};

    //! Time.
    scalar_type time_{};

    //! Step size.
    scalar_type step_size_{};

    //! Variable.
    variable_type variable_{};

    //! Solution of updates.
    update_vector_type* solution_{nullptr};

    //! Buffer of coefficient matrix.
    newton_coeff_matrix_type coeff_matrix_{};

    //! Solver of the current coefficient matrix.
    Eigen::PartialPivLU<newton_coeff_matrix_type> solver_{};

    //! Temporary variable.
    variable_type temp_variable_{};

    //! Buffer of slopes.
    update_vector_type slopes_{};

    //! Residual vector.
    update_vector_type residual_{};

    //! Newton update vector.
    update_vector_type update_{};

    //! Norm of update.
    std::optional<scalar_type> update_norm_{};

    //! Rate in which update is reduced from the previous step.
    std::optional<scalar_type> update_reduction_rate_{};

    //! Default rate of tolerance in this solver.
    static constexpr auto default_tolerance_rate =
        static_cast<scalar_type>(1e-2);

    //! Rate of tolerance in this solver.
    scalar_type tolerance_rate_{default_tolerance_rate};

    //! Number of iterations.
    index_type iterations_{0};

    //! Error tolerances.
    error_tolerances<variable_type> tolerances_{};
};

}  // namespace num_collect::ode::runge_kutta
