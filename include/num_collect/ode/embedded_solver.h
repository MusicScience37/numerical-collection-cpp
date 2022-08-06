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
#include <optional>
#include <type_traits>  // IWYU pragma: keep

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/one.h"   // IWYU pragma: keep
#include "num_collect/constants/zero.h"  // IWYU pragma: keep
#include "num_collect/logging/iteration_logger.h"
#include "num_collect/ode/basic_step_size_controller.h"
#include "num_collect/ode/concepts/embedded_formula.h"      // IWYU pragma: keep
#include "num_collect/ode/concepts/step_size_controller.h"  // IWYU pragma: keep
#include "num_collect/ode/initial_step_size_calculator.h"
#include "num_collect/ode/solver_base.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/is_eigen_vector.h"  // IWYU pragma: keep

namespace num_collect::ode {

/*!
 * \brief Class of solvers of ODEs using embedded formulas.
 *
 * \tparam Formula Type of formula.
 * \tparam StepSizeController Type of the controller of step sizes.
 */
template <concepts::embedded_formula Formula,
    concepts::step_size_controller StepSizeController =
        basic_step_size_controller<Formula>>
class embedded_solver
    : public solver_base<embedded_solver<Formula, StepSizeController>,
          Formula> {
public:
    //! This type.
    using this_type = embedded_solver<Formula, StepSizeController>;

    //! Type of base class.
    using base_type =
        solver_base<embedded_solver<Formula, StepSizeController>, Formula>;

    using typename base_type::formula_type;
    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    //! Type of the controller of step sizes.
    using step_size_controller_type = StepSizeController;

    static_assert(std::is_same_v<formula_type,
        typename step_size_controller_type::formula_type>);

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = formula_type::lesser_order;

    using base_type::base_type;
    using base_type::formula;
    using base_type::problem;

    //! \copydoc ode::solver_base::init
    void init(scalar_type time, const variable_type& variable) {
        time_ = time;
        variable_ = variable;
        steps_ = 0;

        step_size_controller_.init(variable_);

        if (step_size_) {
            this->logger().trace()(
                "Using user-specified initial step size {}.", *step_size_);
        } else {
            this->logger().trace()(
                "Automatically calculate initial step size.");
            step_size_ = initial_step_size_calculator<formula_type>().calculate(
                this->problem(), time_, variable_,
                step_size_controller_.limits(),
                step_size_controller_.tolerances());
            this->logger().trace()(
                "Automatically selected initial step size {}.", *step_size_);
        }
    }

    //! \copydoc ode::solver_base::step
    void step() {
        if (!step_size_) {
            throw precondition_not_satisfied(
                "Step size is not set yet. You may forget to call init "
                "function.");
        }

        prev_variable_ = variable_;

        formula().step_embedded(
            time_, *step_size_, prev_variable_, variable_, error_);
        constexpr index_type max_retry = 10000;  // safe guard
        for (index_type i = 0; i < max_retry; ++i) {
            const scalar_type last_step_size = *step_size_;
            if (step_size_controller_.check_and_calc_next(
                    *step_size_, variable_, error_)) {
                time_ += last_step_size;
                ++steps_;
                last_step_size_ = last_step_size;
                return;
            }
            formula().step_embedded(
                time_, *step_size_, prev_variable_, variable_, error_);
        }
    }

    //! \copydoc ode::solver_base::configure_iteration_logger
    void configure_iteration_logger(
        logging::iteration_logger& iteration_logger) const {
        iteration_logger.append<index_type>(
            "Steps", [this] { return steps(); });
        iteration_logger.append<scalar_type>("Time", [this] { return time(); });
        iteration_logger.append<scalar_type>(
            "StepSize", [this] { return last_step_size(); });
        iteration_logger.append<scalar_type>(
            "EstError", [this] { return error_norm(); });
    }

    //! \copydoc ode::solver_base::time
    [[nodiscard]] auto time() const -> scalar_type { return time_; }

    //! \copydoc ode::solver_base::variable
    [[nodiscard]] auto variable() const -> const variable_type& {
        return variable_;
    }

    //! \copydoc ode::solver_base::step_size()
    [[nodiscard]] auto step_size() const -> scalar_type {
        if (!step_size_) {
            return std::numeric_limits<scalar_type>::quiet_NaN();
        }
        return step_size_.value();
    }

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

    //! \copydoc ode::solver_base::steps
    [[nodiscard]] auto steps() const -> index_type { return steps_; }

    //! \copydoc ode::solver_base::step_size(scalar_type)
    auto step_size(scalar_type val) -> this_type& {
        NUM_COLLECT_ASSERT(val > constants::zero<scalar_type>);
        step_size_ = val;
        return *this;
    }

    /*!
     * \brief Access the controller of step sizes.
     *
     * \return Reference of the controller.
     */
    auto step_size_controller() -> step_size_controller_type& {
        return step_size_controller_;
    }

    /*!
     * \brief Access the controller of step sizes.
     *
     * \return Reference of the controller.
     */
    auto step_size_controller() const -> const step_size_controller_type& {
        return step_size_controller_;
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

    //! Step size used in the next step.
    std::optional<scalar_type> step_size_{};

    //! Step size used in the last step.
    scalar_type last_step_size_{std::numeric_limits<scalar_type>::quiet_NaN()};

    //! Estimate of error.
    variable_type error_{};

    //! Controller of step sizes.
    step_size_controller_type step_size_controller_{};

    //! Time.
    scalar_type time_{};

    //! Number of steps.
    index_type steps_{};
};

}  // namespace num_collect::ode
