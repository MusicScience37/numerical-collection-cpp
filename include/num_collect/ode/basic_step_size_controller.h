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
#include <string_view>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/formula.h"  // IWYU pragma: keep
#include "num_collect/ode/error_tolerances.h"  // IWYU pragma: keep
#include "num_collect/ode/impl/get_least_known_order.h"
#include "num_collect/ode/step_size_controller_base.h"
#include "num_collect/ode/step_size_limits.h"  // IWYU pragma: keep
#include "num_collect/util/assert.h"

namespace num_collect::ode {

//! Log tag.
constexpr auto basic_step_size_controller_log_tag =
    logging::log_tag_view("num_collect::ode::basic_step_size_controller");

/*!
 * \brief Class to control step sizes using well-known method \cite Hairer1993.
 *
 * \tparam Formula Type of the formula.
 */
template <concepts::formula Formula>
class basic_step_size_controller
    : public step_size_controller_base<basic_step_size_controller<Formula>,
          Formula> {
public:
    //! Base class.
    using base_type =
        step_size_controller_base<basic_step_size_controller<Formula>, Formula>;

    using typename base_type::formula_type;
    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    /*!
     * \brief Constructor.
     */
    basic_step_size_controller()
        : base_type(basic_step_size_controller_log_tag) {}

    /*!
     * \brief Initialize.
     */
    void init() {
        // no operation.
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
        if (this->reduce_if_needed(step_size, variable, error)) {
            return false;
        }
        calc_next(step_size, variable, error);
        return true;
    }

    /*!
     * \brief Set the safety coefficient for factors of step sizes.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto step_size_factor_safety_coeff(
        const scalar_type& val) -> basic_step_size_controller& {
        if (val <= static_cast<scalar_type>(0)) {
            throw invalid_argument(
                "Safety coefficient for factors of step sizes must be a "
                "positive value.");
        }
        step_size_factor_safety_coeff_ = val;
        return *this;
    }

    /*!
     * \brief Set the maximum factor of step sizes.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_step_size_factor(
        const scalar_type& val) -> basic_step_size_controller& {
        if (val <= static_cast<scalar_type>(0)) {
            throw invalid_argument(
                "Maximum factor of step sizes must be a positive value.");
        }
        max_step_size_factor_ = val;
        return *this;
    }

private:
    /*!
     * \brief Calculate the next step size.
     *
     * \param[in,out] step_size Step size.
     * \param[in] variable Variable.
     * \param[in] error Error estimate.
     */
    void calc_next(scalar_type& step_size, const variable_type& variable,
        const variable_type& error) {
        // First, calculate factor of step size using a formula in the
        // reference.
        const scalar_type error_norm =
            this->tolerances().calc_norm(variable, error);
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
        step_size = this->limits().apply(step_size);
    }

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
