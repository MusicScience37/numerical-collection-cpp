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
 * \brief Definition of embedded_solver class.
 */
#pragma once

#include <cmath>
#include <limits>
#include <type_traits>

#include "num_collect/constants/one.h"
#include "num_collect/constants/zero.h"
#include "num_collect/ode/runge_kutta/solver_base.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/is_eigen_vector.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of solvers using embedded formulas in Runge-Kutta method.
 *
 * \tparam Formula Type of formula.
 */
template <typename Formula>
class embedded_solver : public solver_base<embedded_solver<Formula>, Formula> {
public:
    //! This type.
    using this_type = embedded_solver<Formula>;

    //! Type of base class.
    using base_type = solver_base<embedded_solver<Formula>, Formula>;

    using typename base_type::formula_type;
    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = formula_type::lesser_order;

    using base_type::base_type;
    using base_type::formula;
    using base_type::problem;

    //! \copydoc runge_kutta::solver_base::init
    void init(scalar_type time, const variable_type& variable) {
        time_ = time;
        variable_ = variable;
        steps_ = 0;
    }

    //! \copydoc runge_kutta::solver_base::step
    void step() {
        prev_variable_ = variable_;
        const scalar_type tol_error =
            std::max(tol_abs_error_, tol_rel_error_ * norm(variable_));

        formula().step_embedded(
            time_, step_size_, prev_variable_, variable_, error_);
        constexpr index_type max_retry = 10000;  // safe guard
        for (index_type i = 0; i < max_retry; ++i) {
            if (norm(error_) < tol_error) {
                break;
            }
            step_size_ *= step_size_reduction_rate_;
            formula().step_embedded(
                time_, step_size_, prev_variable_, variable_, error_);
        }
        time_ += step_size_;
        last_step_size_ = step_size_;

        scalar_type error_norm = norm(error_);
        using std::abs;
        using std::isinf;
        if (isinf(error_norm) || abs(error_norm)) {
            error_norm = std::numeric_limits<scalar_type>::min();
        }
        constexpr scalar_type exponent = static_cast<scalar_type>(1) /
            static_cast<scalar_type>(lesser_order);
        step_size_ *= std::pow(tol_error / error_norm, exponent);
        if (step_size_ > max_step_size_) {
            step_size_ = max_step_size_;
        }

        ++steps_;
    }

    //! \copydoc runge_kutta::solver_base::set_info_to
    void set_info_to(iteration_logger& logger) const {
        logger["Steps"] = steps();
        logger["Time"] = time();
        logger["StepSize"] = last_step_size();
        logger["Error"] = error_norm();
    }

    //! \copydoc runge_kutta::solver_base::time
    [[nodiscard]] auto time() const -> scalar_type { return time_; }

    //! \copydoc runge_kutta::solver_base::variable
    [[nodiscard]] auto variable() const -> const variable_type& {
        return variable_;
    }

    //! \copydoc runge_kutta::solver_base::step_size()
    [[nodiscard]] auto step_size() const -> scalar_type { return step_size_; }

    /*!
     * \brief Get the step size used in the last step.
     *
     * \return Step size used in the last step.
     */
    [[nodiscard]] auto last_step_size() const -> scalar_type {
        return last_step_size_;
    }

    /*!
     * \brief Get the estimate of error in the current variable.
     *
     * \return Estimate of error.
     */
    [[nodiscard]] auto error_norm() const -> scalar_type {
        return norm(error_);
    }

    //! \copydoc runge_kutta::solver_base::steps
    [[nodiscard]] auto steps() const -> index_type { return steps_; }

    //! \copydoc runge_kutta::solver_base::step_size(scalar_type)
    auto step_size(scalar_type val) -> this_type& {
        NUM_COLLECT_ASSERT(val > constants::zero<scalar_type>);
        step_size_ = val;
        return *this;
    }

    /*!
     * \brief Set tolerance of relative error.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_rel_error(scalar_type val) {
        NUM_COLLECT_ASSERT(val > constants::zero<scalar_type>);
        tol_rel_error_ = val;
        return *this;
    }

    /*!
     * \brief Set tolerance of absolute error.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_abs_error(scalar_type val) {
        NUM_COLLECT_ASSERT(val > constants::zero<scalar_type>);
        tol_abs_error_ = val;
        return *this;
    }

    /*!
     * \brief Set rate of reduction of step size.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto step_size_reduction_rate(scalar_type val) {
        NUM_COLLECT_ASSERT(val > constants::zero<scalar_type>);
        NUM_COLLECT_ASSERT(val < constants::one<scalar_type>);
        step_size_reduction_rate_ = val;
        return *this;
    }

    /*!
     * \brief Set maximum step size.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_step_size(scalar_type val) {
        NUM_COLLECT_ASSERT(val > constants::zero<scalar_type>);
        max_step_size_ = val;
        return *this;
    }

private:
    /*!
     * \brief Get the norm of a variable.
     *
     * \param[in] var Variable.
     * \return Norm.
     */
    [[nodiscard]] static auto norm(const variable_type& var) -> scalar_type {
        if constexpr (is_eigen_vector_v<variable_type>) {
            return var.norm();
        } else {
            using std::abs;
            return abs(var);
        }
    }

    //! Previous variable.
    variable_type prev_variable_{};

    //! Variable.
    variable_type variable_{};

    //! Default step size.
    static constexpr auto default_step_size = static_cast<scalar_type>(1e-2);

    //! Step size used in the next step.
    scalar_type step_size_{default_step_size};

    //! Step size used in the last step.
    scalar_type last_step_size_{std::numeric_limits<scalar_type>::quiet_NaN()};

    //! Estimate of error.
    variable_type error_{};

    //! Norm of the estimate of error.
    scalar_type error_norm_{};

    //! Default tolerance of relative error.
    static constexpr auto default_tol_rel_error =
        static_cast<scalar_type>(1e-6);

    //! Tolerance of relative error.
    scalar_type tol_rel_error_{default_tol_rel_error};

    //! Default tolerance of absolute error.
    static constexpr auto default_tol_abs_error =
        static_cast<scalar_type>(1e-6);

    //! Tolerance of absolute error.
    scalar_type tol_abs_error_{default_tol_abs_error};

    //! Default rate of reduction of step size.
    static constexpr auto default_step_size_reduction_rate =
        static_cast<scalar_type>(0.5);

    //! Rate of reduction of step size.
    scalar_type step_size_reduction_rate_{default_step_size_reduction_rate};

    //! Default maximum step size.
    static constexpr auto default_max_step_size =
        static_cast<scalar_type>(1e-2);

    //! Maximum step size.
    scalar_type max_step_size_{default_max_step_size};

    //! Time.
    scalar_type time_{};

    //! Number of steps.
    index_type steps_{};
};

}  // namespace num_collect::ode::runge_kutta
