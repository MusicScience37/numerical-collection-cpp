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
 * \brief Definition of pi_step_size_controller class.
 */
#pragma once

#include <cmath>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/formula.h"
#include "num_collect/ode/error_tolerances.h"  // IWYU pragma: keep
#include "num_collect/ode/impl/get_least_known_order.h"
#include "num_collect/ode/step_size_controller_base.h"
#include "num_collect/ode/step_size_limits.h"  // IWYU pragma: keep

namespace num_collect::ode {

//! Log tag.
constexpr auto pi_step_size_controller_log_tag =
    logging::log_tag_view("num_collect::ode::pi_step_size_controller");

/*!
 * \brief Class to control step sizes using PI controller \cite Gustafsson1991.
 *
 * \tparam Formula Type of the formula.
 */
template <concepts::formula Formula>
class pi_step_size_controller
    : public step_size_controller_base<pi_step_size_controller<Formula>,
          Formula> {
public:
    //! Base class.
    using base_type =
        step_size_controller_base<pi_step_size_controller<Formula>, Formula>;

    using typename base_type::formula_type;
    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    //! Order of the formula used in exponents.
    static constexpr index_type formula_order_for_exponent =
        impl::get_least_known_order<formula_type>();

    /*!
     * \brief Default exponent of the error of the current time step.
     *
     * This is a value proposed by \cite Gustafsson1991.
     */
    static constexpr scalar_type default_current_step_error_exponent =
        static_cast<scalar_type>(0.7) /
        static_cast<scalar_type>(formula_order_for_exponent + 1);

    /*!
     * \brief Default exponent of the error of the previous time step.
     *
     * This is a value proposed by \cite Gustafsson1991.
     */
    static constexpr scalar_type default_previous_step_error_exponent =
        static_cast<scalar_type>(0.4) /
        static_cast<scalar_type>(formula_order_for_exponent + 1);

    /*!
     * \brief Constructor.
     */
    pi_step_size_controller() : base_type(pi_step_size_controller_log_tag) {}

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
     * \brief Set the exponent of the error of the current time step.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto current_step_error_exponent(
        const scalar_type& val) -> pi_step_size_controller& {
        if (val < previous_step_error_exponent_) {
            throw invalid_argument(
                "0 <= previous_step_error_exponent <= "
                "current_step_error_exponent must be satisfied.");
        }
        current_step_error_exponent_ = val;
        return *this;
    }

    /*!
     * \brief Set the exponent of the error of the previous time step.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto previous_step_error_exponent(
        const scalar_type& val) -> pi_step_size_controller& {
        if (val < static_cast<scalar_type>(0) ||
            current_step_error_exponent_ < val) {
            throw invalid_argument(
                "0 <= previous_step_error_exponent <= "
                "current_step_error_exponent must be satisfied.");
        }
        previous_step_error_exponent_ = val;
        return *this;
    }

    /*!
     * \brief Set the safety coefficient for factors of step sizes.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto step_size_factor_safety_coeff(
        const scalar_type& val) -> pi_step_size_controller& {
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
        const scalar_type& val) -> pi_step_size_controller& {
        if (val <= min_step_size_factor_) {
            throw invalid_argument(
                "0 < min_step_size_factor < max_step_size_factor must be "
                "satisfied.");
        }
        max_step_size_factor_ = val;
        return *this;
    }

    /*!
     * \brief Set the minimum factor of step sizes.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto min_step_size_factor(
        const scalar_type& val) -> pi_step_size_controller& {
        if (val <= static_cast<scalar_type>(0) ||
            max_step_size_factor_ <= val) {
            throw invalid_argument(
                "0 < min_step_size_factor < max_step_size_factor must be "
                "satisfied.");
        }
        min_step_size_factor_ = val;
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
        scalar_type factor = pow(error_norm, -current_step_error_exponent_) *
            pow(previous_step_error_, previous_step_error_exponent_);

        // Secondly, change the factor for safety.
        using std::isfinite;
        factor *= step_size_factor_safety_coeff_;
        if (!isfinite(factor)) {
            factor = static_cast<scalar_type>(1);
        }
        if (factor > max_step_size_factor_) {
            factor = max_step_size_factor_;
        } else if (factor < min_step_size_factor_) {
            factor = min_step_size_factor_;
        }

        // Finally, multiply the factor to the step size.
        step_size *= factor;
        step_size = this->limits().apply(step_size);

        // Prepare for the next step.
        previous_step_error_ = error_norm;
    }

    //! Error of the previous time step.
    scalar_type previous_step_error_{static_cast<scalar_type>(1)};

    //! Exponent of the error of the current time step.
    scalar_type current_step_error_exponent_{
        default_current_step_error_exponent};

    //! Exponent of the error of the previous time step.
    scalar_type previous_step_error_exponent_{
        default_previous_step_error_exponent};

    //! Default safety coefficient for factors of step sizes.
    static constexpr auto default_step_size_factor_safety_coeff =
        static_cast<scalar_type>(0.9);

    //! Safety coefficient for factors of step sizes.
    scalar_type step_size_factor_safety_coeff_{
        default_step_size_factor_safety_coeff};

    //! Default maximum factor of step sizes.
    static constexpr auto default_max_step_size_factor =
        static_cast<scalar_type>(2);

    //! Maximum factor of step sizes.
    scalar_type max_step_size_factor_{default_max_step_size_factor};

    //! Default minimum factor of step sizes.
    static constexpr auto default_min_step_size_factor =
        static_cast<scalar_type>(0.1);

    //! Minimum factor of step sizes.
    scalar_type min_step_size_factor_{default_min_step_size_factor};
};

}  // namespace num_collect::ode
