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
 * \brief Definition of bicgstab_rosenbrock_equation_solver class.
 */
#pragma once

// IWYU pragma: no_include <Eigen/Core>
// IWYU pragma: no_include <Eigen/SparseCore>

#include <cmath>  // IWYU pragma: keep
#include <limits>
#include <optional>

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/exception.h"
#include "num_collect/ode/concepts/mass_problem.h"
#include "num_collect/ode/concepts/multi_variate_problem.h"
#include "num_collect/ode/concepts/time_differentiable_problem.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/impl/bicgstab.h"

namespace num_collect::ode::rosenbrock {

/*!
 * \brief Class to solve equations in Rosenbrock methods using BiCGstab.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::multi_variate_problem Problem>
class bicgstab_rosenbrock_equation_solver {
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
    explicit bicgstab_rosenbrock_equation_solver(
        const scalar_type& inverted_jacobian_coeff)
        : inverted_jacobian_coeff_(inverted_jacobian_coeff) {}

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

        problem.evaluate_on(time, variable,
            evaluation_type{.diff_coeff = true,
                .time_derivative = use_time_derivative,
                .mass = use_mass});
        if constexpr (use_time_derivative) {
            time_derivative_ = problem.time_derivative();
        }
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
        if (problem_ == nullptr) {
            throw precondition_not_satisfied(
                "evaluate_and_update_jacobian is not called.");
        }

        const scalar_type target_norm = target.norm();
        if (target_norm < std::numeric_limits<scalar_type>::min()) {
            result = variable_type::Zero(target.size());
            return;
        }
        const scalar_type diff_width =
            std::sqrt(std::numeric_limits<scalar_type>::epsilon()) /
            target_norm;

        problem_->evaluate_on(time_, variable_ + diff_width * target,
            evaluation_type{.diff_coeff = true});
        result = problem_->diff_coeff();

        problem_->evaluate_on(time_, variable_ - diff_width * target,
            evaluation_type{.diff_coeff = true});
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
                result += problem_->mass() * target;
            } else {
                result += target;
            }
        };
        result = variable_type::Zero(rhs.size());
        bicgstab_.solve(coeff_function, rhs, result);
    }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val)
        -> bicgstab_rosenbrock_equation_solver& {
        bicgstab_.tolerances(val);
        return *this;
    }

private:
    //! Problem.
    problem_type* problem_{nullptr};

    //! Time.
    scalar_type time_{};

    //! Step size.
    scalar_type step_size_{};

    //! Variable.
    variable_type variable_{};

    //! Partial derivative with respect to time.
    std::optional<variable_type> time_derivative_{};

    //! BiCGstab solver.
    impl::bicgstab<variable_type> bicgstab_{};

    //! Coefficient multiplied to Jacobian matrices in inverted matrices.
    scalar_type inverted_jacobian_coeff_{static_cast<scalar_type>(1)};
};

}  // namespace num_collect::ode::rosenbrock
