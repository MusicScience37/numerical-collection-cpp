/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of gmres_rosenbrock_equation_solver class.
 */
#pragma once

#include <cmath>  // IWYU pragma: keep
#include <limits>
#include <optional>

#include <Eigen/Core>  // IWYU pragma: keep

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/functions/root.h"
#include "num_collect/linear/diagonal_estimator.h"
#include "num_collect/linear/functional_gmres.h"
#include "num_collect/ode/concepts/mass_problem.h"
#include "num_collect/ode/concepts/multi_variate_problem.h"
#include "num_collect/ode/concepts/time_differentiable_problem.h"
#include "num_collect/ode/evaluation_type.h"

namespace num_collect::ode::rosenbrock {

/*!
 * \brief Class to solve equations in Rosenbrock methods using GMRES.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::multi_variate_problem Problem>
class gmres_rosenbrock_equation_solver;

/*!
 * \brief Class to solve equations in Rosenbrock methods using GMRES.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::multi_variate_problem Problem>
class gmres_rosenbrock_equation_solver {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Whether to use partial derivative with respect to time.
    static constexpr bool use_time_derivative =
        concepts::time_differentiable_problem<problem_type>;

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] inverted_jacobian_coeff Coefficient multiplied to Jacobian
     * matrices in inverted matrices.
     */
    explicit gmres_rosenbrock_equation_solver(
        const scalar_type& inverted_jacobian_coeff)
        : inverted_jacobian_coeff_(inverted_jacobian_coeff) {
        gmres_.tolerance(gmres_tolerance_rate);
    }

    /*!
     * \brief Update Jacobian matrix and internal parameters.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variable.
     */
    void evaluate_and_update_jacobian(problem_type& problem,
        const scalar_type& time, const scalar_type& step_size,
        const variable_type& variable) {
        problem_ = &problem;
        time_ = time;
        step_size_ = step_size;
        variable_ = variable;

        diagonal_estimator_.estimate(
            [this](const variable_type& input, variable_type& output) {
                this->apply_jacobian(input, output);
            },
            variable.size(), jacobian_diagonal_estimate_);

        problem.evaluate_on(time, variable,
            evaluation_type{
                .diff_coeff = true, .time_derivative = use_time_derivative});
        if constexpr (use_time_derivative) {
            time_derivative_ = problem.time_derivative();
        }
        if constexpr (use_mass) {
            coeff_matrix_diagonal_estimate_ = problem_->mass().diagonal();
        } else {
            coeff_matrix_diagonal_estimate_.resize(variable_.size());
            coeff_matrix_diagonal_estimate_.setOnes();
        }
        coeff_matrix_diagonal_estimate_.noalias() -=
            step_size_ * inverted_jacobian_coeff_ * jacobian_diagonal_estimate_;
        gmres_.prepare_preconditioner(coeff_matrix_diagonal_estimate_);
    }

    /*!
     * \brief Multiply Jacobian matrix to a vector.
     *
     * \param[in] target Target.
     * \param[out] result Result.
     */
    template <base::concepts::real_scalar_dense_vector Target,
        base::concepts::real_scalar_dense_vector Result>
    void apply_jacobian(const Target& target, Result& result) {
        NUM_COLLECT_PRECONDITION(
            problem_ != nullptr, "evaluate_and_update_jacobian is not called.");

        const scalar_type target_norm = target.norm();
        const scalar_type variable_norm = variable_.norm();
        if (target_norm < variable_norm * epsilon) {
            result = variable_type::Zero(target.size());
            return;
        }
        const scalar_type diff_width =
            std::max(jacobian_diff_width * variable_norm / target_norm,
                jacobian_diff_width);

        variable_buffer_ = variable_ + diff_width * target;
        problem_->evaluate_on(
            time_, variable_buffer_, evaluation_type{.diff_coeff = true});
        result = problem_->diff_coeff();

        variable_buffer_ = variable_ - diff_width * target;
        problem_->evaluate_on(
            time_, variable_buffer_, evaluation_type{.diff_coeff = true});
        result -= problem_->diff_coeff();
        result /= static_cast<scalar_type>(2) * diff_width;
    }

    /*!
     * \brief Add a term of partial derivative with respect to time.
     *
     * \param[in] step_size Step size.
     * \param[in] coeff Coefficient in formula.
     * \param[in,out] target Target variable.
     */
    void add_time_derivative_term(const scalar_type& step_size,
        const scalar_type& coeff, variable_type& target) {
        if constexpr (use_time_derivative) {
            if (time_derivative_) {
                target += step_size * coeff * (*time_derivative_);
            }
        }
    }

    /*!
     * \brief Solve a linear equation.
     *
     * \param[in] rhs Right-hand-side value.
     * \param[out] result Result.
     */
    void solve(const variable_type& rhs, variable_type& result) {
        const auto coeff_function = [this](const auto& target, auto& result) {
            this->apply_jacobian(target, result);
            result *= -step_size_ * inverted_jacobian_coeff_;
            if constexpr (use_mass) {
                result.noalias() += problem_->mass() * target;
            } else {
                result += target;
            }
        };
        result = variable_type::Zero(rhs.size());
        gmres_.solve(coeff_function, rhs, result);
    }

    /*!
     * \brief Set the maximum number of dimensions of subspace used in GMRES.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_subspace_dim(index_type val) -> gmres_rosenbrock_equation_solver& {
        gmres_.max_subspace_dim(val);
        return *this;
    }

    /*!
     * \brief Access to the logger.
     *
     * \return Logger.
     */
    [[nodiscard]] auto logger() const noexcept
        -> const num_collect::logging::logger& {
        return gmres_.logger();
    }

    /*!
     * \brief Access to the logger.
     *
     * \return Logger.
     */
    [[nodiscard]] auto logger() noexcept -> num_collect::logging::logger& {
        return gmres_.logger();
    }

private:
    //! Machine epsilon.
    static constexpr scalar_type epsilon =
        std::numeric_limits<scalar_type>::epsilon();

    //! Width of finite difference for Jacobian application.
    static constexpr scalar_type jacobian_diff_width =
        functions::root(epsilon, 3);

    //! Expected precision of finite difference for Jacobian application.
    static constexpr scalar_type jacobian_diff_precision =
        jacobian_diff_width * jacobian_diff_width;

    //! Tolerance rate for GMRES to prevent numerical instability due to the error of finite difference.
    static constexpr scalar_type gmres_tolerance_rate =
        100 * jacobian_diff_precision;

    //! Problem.
    problem_type* problem_{nullptr};

    //! Time.
    scalar_type time_{};

    //! Step size.
    scalar_type step_size_{};

    //! Variable.
    variable_type variable_{};

    //! Buffer of a variable for finite difference.
    variable_type variable_buffer_{};

    //! Partial derivative with respect to time.
    std::optional<variable_type> time_derivative_{};

    //! GMRES solver.
    linear::functional_gmres<variable_type> gmres_{};

    //! Estimator of diagonal elements.
    linear::diagonal_estimator<variable_type> diagonal_estimator_{};

    //! Estimated diagonal elements of Jacobian.
    variable_type jacobian_diagonal_estimate_{};

    //! Estimated diagonal elements of the coefficient matrix.
    variable_type coeff_matrix_diagonal_estimate_{};

    //! Coefficient multiplied to Jacobian matrices in inverted matrices.
    scalar_type inverted_jacobian_coeff_{static_cast<scalar_type>(1)};
};

}  // namespace num_collect::ode::rosenbrock
