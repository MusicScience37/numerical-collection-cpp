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

#include <algorithm>
#include <cmath>
#include <limits>

#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/ode/error_tolerances.h"  // IWYU pragma: keep
#include "num_collect/ode/step_size_controller.h"
#include "num_collect/ode/step_size_limits.h"  // IWYU pragma: keep

namespace num_collect::ode {

/*!
 * \brief Class of the strategy to calculate next step sizes using PI controller
 * \cite Gustafsson1991.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class pi_step_size_strategy {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Constructor.
     *
     * \param[in] method_order Order of the method.
     */
    explicit pi_step_size_strategy(index_type method_order)
        : method_order_(method_order) {
        NUM_COLLECT_PRECONDITION(method_order_ > 0,
            "Order of the method must be a positive integer.");
    }

    /*!
     * \brief Initialize the internal state.
     *
     * \note No operation is needed for this strategy.
     */
    void init() { previous_step_error_ = static_cast<scalar_type>(1); }

    /*!
     * \brief Calculate the next step size.
     *
     * \param[in,out] step_size Step size.
     * \param[in] error_norm Norm of error estimates calculated by
     * calc_norm function in
     * \ref num_collect::ode::error_tolerances class.
     */
    void calc_next(scalar_type& step_size, const scalar_type& error_norm) {
        // First, calculate factor of step size using a formula in the
        // reference with heuristics to prevent division by zeros.
        const scalar_type current_step_error_exponent =
            -current_step_error_exponent_coeff_ /
            static_cast<scalar_type>(method_order_ + 1);
        const scalar_type previous_step_error_exponent =
            previous_step_error_exponent_coeff_ /
            static_cast<scalar_type>(method_order_ + 1);
        const scalar_type small_error = static_cast<scalar_type>(1e+3) *
            std::numeric_limits<scalar_type>::epsilon();
        scalar_type factor = pow(std::max(error_norm, small_error),
                                 current_step_error_exponent) *
            pow(std::max(previous_step_error_, small_error),
                previous_step_error_exponent);

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

        // Prepare for the next step.
        previous_step_error_ = error_norm;
    }

    /*!
     * \brief Set the coefficient of the exponent of the error of the current
     * time step.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto current_step_error_exponent_coeff(const scalar_type& val)
        -> pi_step_size_strategy& {
        NUM_COLLECT_PRECONDITION(val >= previous_step_error_exponent_coeff_,
            "0 <= previous_step_error_exponent_coeff <= "
            "current_step_error_exponent_coeff must be satisfied.");
        current_step_error_exponent_coeff_ = val;
        return *this;
    }

    /*!
     * \brief Set the coefficient of the exponent of the error of the previous
     * time step.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto previous_step_error_exponent_coeff(const scalar_type& val)
        -> pi_step_size_strategy& {
        NUM_COLLECT_PRECONDITION(static_cast<scalar_type>(0) <= val &&
                val <= current_step_error_exponent_coeff_,
            "0 <= previous_step_error_exponent_coeff <= "
            "current_step_error_exponent_coeff must be satisfied.");
        previous_step_error_exponent_coeff_ = val;
        return *this;
    }

    /*!
     * \brief Set the safety coefficient for factors of step sizes.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto step_size_factor_safety_coeff(const scalar_type& val)
        -> pi_step_size_strategy& {
        NUM_COLLECT_PRECONDITION(val > static_cast<scalar_type>(0),
            "Safety coefficient for factors of step sizes must be a positive "
            "value.");
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
        -> pi_step_size_strategy& {
        NUM_COLLECT_PRECONDITION(val > min_step_size_factor_,
            "0 < min_step_size_factor < max_step_size_factor must be "
            "satisfied.");
        max_step_size_factor_ = val;
        return *this;
    }

    /*!
     * \brief Set the minimum factor of step sizes.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto min_step_size_factor(const scalar_type& val)
        -> pi_step_size_strategy& {
        NUM_COLLECT_PRECONDITION(
            static_cast<scalar_type>(0) < val && val < max_step_size_factor_,
            "0 < min_step_size_factor < max_step_size_factor must be "
            "satisfied.");
        min_step_size_factor_ = val;
        return *this;
    }

private:
    //! Order of the method.
    index_type method_order_;

    //! Error of the previous time step.
    scalar_type previous_step_error_{static_cast<scalar_type>(1)};

    /*!
     * \brief Default coefficient of the exponent of the error of the current
     * time step.
     *
     * This is a value proposed by \cite Gustafsson1991.
     */
    static constexpr scalar_type default_current_step_error_exponent_coeff =
        static_cast<scalar_type>(0.7);

    //! Coefficient of the exponent of the error of the current time step.
    scalar_type current_step_error_exponent_coeff_{
        default_current_step_error_exponent_coeff};

    /*!
     * \brief Default coefficient of the exponent of the error of the previous
     * time step.
     *
     * This is a value proposed by \cite Gustafsson1991.
     */
    static constexpr scalar_type default_previous_step_error_exponent_coeff =
        static_cast<scalar_type>(0.4);

    //! Coefficient of the exponent of the error of the previous time step.
    scalar_type previous_step_error_exponent_coeff_{
        default_previous_step_error_exponent_coeff};

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

/*!
 * \brief Class to control step sizes using PI controller \cite Gustafsson1991.
 *
 * \tparam Problem Type of problems.
 */
template <concepts::problem Problem>
using pi_step_size_controller = step_size_controller<Problem,
    pi_step_size_strategy<typename Problem::scalar_type>>;

}  // namespace num_collect::ode
