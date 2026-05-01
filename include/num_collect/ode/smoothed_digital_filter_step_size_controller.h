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
 * \brief Definition of smoothed_digital_filter_step_size_controller class.
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
 * \brief Class of the strategy to calculate next step sizes with a digital
 * filter and a smooth limiter \cite Blom2013.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class smoothed_digital_filter_step_size_strategy {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Constructor.
     *
     * \param[in] method_order Order of the method.
     */
    explicit smoothed_digital_filter_step_size_strategy(index_type method_order)
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
        using std::atan;
        using std::isfinite;
        using std::pow;

        // If step size is invalid, a bug exists in this library and no
        // operation should be done anymore.
        NUM_COLLECT_ASSERT(isfinite(step_size));
        NUM_COLLECT_ASSERT(step_size > static_cast<scalar_type>(0));

        if (!isfinite(error_norm) || error_norm < static_cast<scalar_type>(0)) {
            // No change in step size.
            return;
        }

        // Heuristics to prevent division by zeros.
        constexpr scalar_type small_error = static_cast<scalar_type>(1e+3) *
            std::numeric_limits<scalar_type>::epsilon();
        const scalar_type current_error_ratio =
            std::max(error_norm, small_error) / error_norm_safety_coeff_;

        if (previous_step_error_norm_ && is_previous_step_size_accepted_) {
            const scalar_type previous_error_ratio =
                std::max(*previous_step_error_norm_, small_error) /
                error_norm_safety_coeff_;
            const scalar_type error_ratio_exponent =
                static_cast<scalar_type>(-0.25) /
                static_cast<scalar_type>(method_order_ + 1);
            constexpr scalar_type previous_step_size_factor_exponent =
                static_cast<scalar_type>(-0.25);
            step_size_factor_without_limiter_ =
                pow(current_error_ratio, error_ratio_exponent) *
                pow(previous_error_ratio, error_ratio_exponent) *
                pow(step_size_factor_without_limiter_,
                    previous_step_size_factor_exponent);

            NUM_COLLECT_DEBUG_ASSERT(
                isfinite(step_size_factor_without_limiter_));
            NUM_COLLECT_DEBUG_ASSERT(step_size_factor_without_limiter_ >
                static_cast<scalar_type>(0));
        } else {
            const scalar_type error_ratio_exponent =
                static_cast<scalar_type>(-1) /
                static_cast<scalar_type>(method_order_ + 1);
            step_size_factor_without_limiter_ =
                pow(current_error_ratio, error_ratio_exponent);

            NUM_COLLECT_DEBUG_ASSERT(
                isfinite(step_size_factor_without_limiter_));
            NUM_COLLECT_DEBUG_ASSERT(step_size_factor_without_limiter_ >
                static_cast<scalar_type>(0));
        }
        previous_step_error_norm_ = error_norm;

        const scalar_type limited_factor = static_cast<scalar_type>(1) +
            atan(step_size_factor_without_limiter_ -
                static_cast<scalar_type>(1));
        step_size *= limited_factor;

        // When the factor is small, the next calculation won't use memory.
        // Threshold is given in the literature.
        constexpr scalar_type step_size_factor_threshold = 0.9;
        is_previous_step_size_accepted_ =
            limited_factor > step_size_factor_threshold;
    }

    /*!
     * \brief Notify that the previous step size was rejected.
     */
    void notify_previous_step_size_rejected() {
        previous_step_error_norm_.reset();
        is_previous_step_size_accepted_ = false;
    }

    /*!
     * \brief Get whether the previous step size is accepted.
     *
     * \retval true The previous step size is accepted.
     * \retval false The previous step size is rejected.
     */
    [[nodiscard]] auto is_previous_step_size_accepted() const noexcept -> bool {
        return is_previous_step_size_accepted_;
    }

    /*!
     * \brief Set the safety coefficient for error norms.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto error_norm_safety_coeff(const scalar_type& val)
        -> smoothed_digital_filter_step_size_strategy& {
        NUM_COLLECT_PRECONDITION(val > static_cast<scalar_type>(0),
            "Safety coefficient for error norms must be a positive number.");
        NUM_COLLECT_PRECONDITION(val < static_cast<scalar_type>(1),
            "Safety coefficient for error norms must be less than 1.");
        error_norm_safety_coeff_ = val;
        return *this;
    }

private:
    //! Error norm of the previous time step.
    std::optional<scalar_type> previous_step_error_norm_{};

    //! Factor of step size without limiter.
    scalar_type step_size_factor_without_limiter_{};

    //! Whether the previous step size is accepted.
    bool is_previous_step_size_accepted_{true};

    //! Order of the method.
    index_type method_order_;

    //! Default safety coefficient for error norms.
    static constexpr auto default_error_norm_safety_coeff =
        static_cast<scalar_type>(0.9);

    //! Safety coefficient for error norms.
    scalar_type error_norm_safety_coeff_{default_error_norm_safety_coeff};
};

/*!
 * \brief Class to control step sizes with a digital
 * filter and a smooth limiter \cite Blom2013.
 *
 * \tparam Problem Type of problems.
 */
template <concepts::problem Problem>
using smoothed_digital_filter_step_size_controller = step_size_controller<
    Problem,
    smoothed_digital_filter_step_size_strategy<typename Problem::scalar_type>>;

}  // namespace num_collect::ode
