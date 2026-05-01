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
 * \brief Definition of memory_step_size_controller class.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/ode/concepts/problem.h"
#include "num_collect/ode/step_size_controller.h"
#include "num_collect/util/assert.h"

namespace num_collect::ode {

/*!
 * \brief Class of the strategy to calculate next step sizes with memory
 * written in \cite Hairer1999.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class memory_step_size_strategy {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Constructor.
     *
     * \param[in] method_order Order of the method.
     */
    explicit memory_step_size_strategy(index_type method_order)
        : method_order_(method_order) {
        NUM_COLLECT_PRECONDITION(method_order_ > 0,
            "Order of the method must be a positive integer.");
    }

    /*!
     * \brief Initialize the internal state.
     *
     * \note No operation is needed for this strategy.
     */
    void init() {
        previous_step_error_norm_.reset();
        previous_step_size_.reset();
        is_previous_step_size_accepted_ = true;
    }

    /*!
     * \brief Calculate the next step size.
     *
     * \param[in,out] step_size Step size.
     * \param[in] error_norm Norm of error estimates calculated by
     * calc_norm function in
     * \ref num_collect::ode::error_tolerances class.
     */
    void calc_next(scalar_type& step_size, const scalar_type& error_norm) {
        using std::isfinite;
        // If step size is invalid, a bug exists in this library and no
        // operation should be done anymore.
        NUM_COLLECT_ASSERT(isfinite(step_size));
        NUM_COLLECT_ASSERT(step_size > static_cast<scalar_type>(0));

        if (!isfinite(error_norm) ||
            error_norm <= static_cast<scalar_type>(0)) {
            // No change in step size.
            return;
        }

        // First, calculate the factor of step sizes using the current error
        // norm with heuristics to prevent division by zeros.
        const scalar_type error_exponent = static_cast<scalar_type>(1) /
            static_cast<scalar_type>(method_order_ + 1);
        constexpr scalar_type small_error = static_cast<scalar_type>(1e+3) *
            std::numeric_limits<scalar_type>::epsilon();
        using std::pow;
        scalar_type factor =
            pow(std::max(error_norm, small_error), -error_exponent);

        // Calculate the factor using memory if the previous step size and error
        // norm are available.
        if (previous_step_size_ && previous_step_error_norm_) {
            // Note that ratios of step sizes and error norms are calculated in
            // the different orders.
            const scalar_type step_size_ratio_factor =
                step_size / *previous_step_size_;
            const scalar_type error_norm_ratio_factor =
                pow(std::max(*previous_step_error_norm_, small_error) /
                        std::max(error_norm, small_error),
                    error_exponent);
            const scalar_type additional_factor =
                step_size_ratio_factor * error_norm_ratio_factor;
            // Minimum of the classic factor and the new factor is used for
            // robustness.
            factor *= std::min(static_cast<scalar_type>(1), additional_factor);
        }

        // Change the factor for safety.
        factor *= step_size_factor_safety_coeff_;
        const scalar_type max_step_size_factor = is_previous_step_size_accepted_
            ? max_step_size_factor_
            : static_cast<scalar_type>(1);
        if (factor > max_step_size_factor) {
            factor = max_step_size_factor;
        } else if (factor < min_step_size_factor_) {
            factor = min_step_size_factor_;
        }

        // Finally, multiply the factor to the step size.
        previous_step_size_ = step_size;
        step_size *= factor;

        // Prepare for the next step.
        previous_step_error_norm_ = error_norm;
        is_previous_step_size_accepted_ = true;
    }

    /*!
     * \brief Notify that the previous step size was rejected.
     */
    void notify_previous_step_size_rejected() {
        previous_step_error_norm_.reset();
        previous_step_size_.reset();
        is_previous_step_size_accepted_ = false;
    }

    /*!
     * \brief Set the safety coefficient for factors of step sizes.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto step_size_factor_safety_coeff(const scalar_type& val)
        -> memory_step_size_strategy& {
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
        -> memory_step_size_strategy& {
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
        -> memory_step_size_strategy& {
        NUM_COLLECT_PRECONDITION(
            static_cast<scalar_type>(0) < val && val < max_step_size_factor_,
            "0 < min_step_size_factor < max_step_size_factor must be "
            "satisfied.");
        min_step_size_factor_ = val;
        return *this;
    }

private:
    //! Error norm of the previous time step.
    std::optional<scalar_type> previous_step_error_norm_{};

    //! Step size of the previous time step.
    std::optional<scalar_type> previous_step_size_{};

    //! Whether the previous step size is accepted.
    bool is_previous_step_size_accepted_{true};

    //! Order of the method.
    index_type method_order_;

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
 * \brief Class to control step sizes with memory written in \cite Hairer1999.
 *
 * \tparam Problem Type of problems.
 */
template <concepts::problem Problem>
using memory_step_size_controller = step_size_controller<Problem,
    memory_step_size_strategy<typename Problem::scalar_type>>;

}  // namespace num_collect::ode
