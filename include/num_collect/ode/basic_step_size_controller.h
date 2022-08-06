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
 * \brief Definition of basic_step_size_controller class.
 */
#pragma once

#include <cmath>
#include <optional>
#include <string_view>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/ode/concepts/embedded_formula.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/formula.h"           // IWYU pragma: keep
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/step_size_limits.h"
#include "num_collect/util/assert.h"

namespace num_collect::ode {

//! Log tag.
constexpr auto basic_step_size_controller_log_tag =
    logging::log_tag_view("num_collect::ode::basic_step_size_controller");

namespace impl {

/*!
 * \brief Get the least known order of a formula.
 *
 * \tparam Formula Type of the formula.
 * \return Order.
 */
template <concepts::formula Formula>
constexpr auto get_least_known_order() {
    if constexpr (requires() {
                      {
                          Formula::lesser_order
                          } -> base::concepts::decayed_to<index_type>;
                  }) {
        return Formula::lesser_order;
    } else {
        return Formula::order;
    }
}

}  // namespace impl

/*!
 * \brief Class to control step sizes using well-known method \cite Hairer1993.
 *
 * \tparam Formula Type of the formula.
 */
template <concepts::embedded_formula Formula>
class basic_step_size_controller : public logging::logging_mixin {
public:
    //! Type of formula.
    using formula_type = Formula;

    //! Type of problem.
    using problem_type = typename formula_type::problem_type;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    /*!
     * \brief Constructor.
     */
    basic_step_size_controller()
        : logging::logging_mixin(basic_step_size_controller_log_tag) {}

    /*!
     * \brief Initialize.
     *
     * \param[in] reference Reference variable. (For determining the size of
     * variables.)
     */
    void init(const variable_type& reference) {
        if (!tolerances_) {
            tolerances_.emplace(reference);
        }
    }

    /*!
     * \brief Check the error estimate and calculate the next step size.
     *
     * \param[in,out] step_size Step size.
     * \param[in] variable Variable.
     * \param[in] error Error estimate.
     * \retval true Given error satisfies tolerances.
     * \retval false Given error doesn't satisfy tolerances.
     */
    [[nodiscard]] auto check_and_calc_next(scalar_type& step_size,
        const variable_type& variable, const variable_type& error) -> bool {
        const bool tolerance_satisfied = tolerances().check(variable, error);
        if (!tolerance_satisfied) {
            if (step_size > limits_.lower_limit()) {
                this->logger().trace()(
                    "Error tolerance not satisfied with step size {}.",
                    step_size);
                step_size *= reduction_rate_;
                step_size = limits_.apply(step_size);
                return false;
            }
            this->logger().warning()(
                "Error tolerance not satisfied even with the lowest step size "
                "{}.",
                step_size);
        }

        // Select next step size from here.
        // First, calculate factor of step size using a formula in the
        // reference.
        const scalar_type error_norm = tolerances_->calc_norm(variable, error);
        constexpr index_type order_for_exponent =
            impl::get_least_known_order<Formula>();
        constexpr scalar_type exponent = -static_cast<scalar_type>(1) /
            static_cast<scalar_type>(order_for_exponent + 1);
        using std::pow;
        scalar_type factor = pow(error_norm, exponent);

        // Secondly, change the factor for safety.
        using std::isfinite;
        factor *= step_size_factor_safety_coeff_;
        if (factor > max_step_size_factor_ || !isfinite(factor)) {
            factor = max_step_size_factor_;
        }

        // Finally, multiply the factor to the step size.
        step_size *= factor;
        step_size = limits_.apply(step_size);

        return true;
    }

    /*!
     * \brief Set the limits of step sizes.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto limits(const step_size_limits<scalar_type>& val)
        -> basic_step_size_controller& {
        limits_ = val;
        return *this;
    }

    /*!
     * \brief Get the limits of step sizes.
     *
     * \return Value.
     */
    [[nodiscard]] auto limits() const -> const step_size_limits<scalar_type>& {
        return limits_;
    }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val)
        -> basic_step_size_controller& {
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
        if (!tolerances_) {
            throw precondition_not_satisfied("Error tolerance is not set yet.");
        }
        return *tolerances_;
    }

    /*!
     * \brief Set the rate to reduce step sizes when error is large.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto reduction_rate(const scalar_type& val) -> basic_step_size_controller& {
        NUM_COLLECT_ASSERT(val > static_cast<scalar_type>(0));
        reduction_rate_ = val;
        return *this;
    }

    /*!
     * \brief Set the safety coefficient for factors of step sizes.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto step_size_factor_safety_coeff(const scalar_type& val)
        -> basic_step_size_controller& {
        NUM_COLLECT_ASSERT(val > static_cast<scalar_type>(0));
        step_size_factor_safety_coeff_ = val;
        return *this;
    }

    /*!
     * \brief Set the maximum factor of step sizes.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_step_size_factor(const scalar_type& val)
        -> basic_step_size_controller& {
        NUM_COLLECT_ASSERT(val > static_cast<scalar_type>(0));
        max_step_size_factor_ = val;
        return *this;
    }

private:
    //! Limits of step sizes.
    step_size_limits<scalar_type> limits_{};

    //! Error tolerances.
    std::optional<error_tolerances<variable_type>> tolerances_{};

    //! Default rate to reduce step sizes when error is large.
    static constexpr auto default_reduction_rate =
        static_cast<scalar_type>(0.5);

    //! Rate to reduce step sizes when error is large.
    scalar_type reduction_rate_{default_reduction_rate};

    //! Default safety coefficient for factors of step sizes.
    static constexpr auto default_step_size_factor_safety_coeff =
        static_cast<scalar_type>(0.8);

    //! Safety coefficient for factors of step sizes.
    scalar_type step_size_factor_safety_coeff_{
        default_step_size_factor_safety_coeff};

    //! Default maximum factor of step sizes.
    static constexpr auto default_max_step_size_factor =
        static_cast<scalar_type>(2);

    //! Maximum factor of step sizes.
    scalar_type max_step_size_factor_{default_max_step_size_factor};
};

}  // namespace num_collect::ode
