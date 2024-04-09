/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of tanh_finite_integrator class.
 */
#pragma once

#include <cmath>

#include "num_collect/base/concepts/invocable_as.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar.h"   // IWYU pragma: keep
#include "num_collect/base/index_type.h"
#include "num_collect/base/isfinite.h"
#include "num_collect/constants/half.h"
#include "num_collect/constants/one.h"
#include "num_collect/constants/zero.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/kahan_adder.h"

namespace num_collect::integration {

//! Tag of de_finite_integrator.
inline constexpr auto tanh_finite_integrator_tag =
    logging::log_tag_view("num_collect::integration::tanh_finite_integrator");

namespace impl {

/*!
 * \brief Helper class of constants for use in tanh_finite_integrator class.
 *
 * \tparam Variable Type of variables.
 */
template <typename Variable>
struct tanh_finite_integrator_traits;

/*!
 * \brief Implementation of tanh_finite_integrator_traits for float.
 */
template <>
struct tanh_finite_integrator_traits<float> {
public:
    //! Default maximum point in changed variable.
    static constexpr double default_max_point = 7.0F;
};

/*!
 * \brief Implementation of tanh_finite_integrator_traits for double.
 */
template <>
struct tanh_finite_integrator_traits<double> {
public:
    //! Default maximum point in changed variable.
    static constexpr double default_max_point = 16.0;
};

}  // namespace impl

/*!
 * \brief Class to perform numerical integration on finite range using TANH
 * rule.
 *
 * \tparam Signature Function signature.
 */
template <typename Signature>
class tanh_finite_integrator;

/*!
 * \brief Class to perform numerical integration on finite range using TANH
 * rule.
 *
 * \tparam Result Type of results.
 * \tparam Variable Type of variables.
 */
template <typename Result, base::concepts::real_scalar Variable>
class tanh_finite_integrator<Result(Variable)> : public logging::logging_mixin {
public:
    //! Type of variables.
    using variable_type = std::decay_t<Variable>;

    //! Type of results.
    using result_type = std::decay_t<Result>;

    /*!
     * \brief Constructor.
     */
    tanh_finite_integrator()
        : logging::logging_mixin(tanh_finite_integrator_tag) {
        calculate_coefficients();
    }

    /*!
     * \brief Integrate a function.
     *
     * \tparam Function Type of function.
     * \param[in] function Function.
     * \param[in] left Left boundary.
     * \param[in] right Right boundary.
     * \return Result.
     */
    template <base::concepts::invocable_as<result_type(variable_type)> Function>
    [[nodiscard]] auto integrate(const Function& function, variable_type left,
        variable_type right) const -> result_type {
        const variable_type center =
            constants::half<variable_type> * (left + right);
        const variable_type width = right - left;

        constexpr auto center_weight_rate = static_cast<variable_type>(0.5);
        const variable_type center_weight = width * center_weight_rate;
        util::kahan_adder<result_type> sum;
        sum += function(center) * center_weight;

        for (index_type i = 0; i < points_; ++i) {
            const variable_type variable_distance =
                width * variable_rate_list_[static_cast<std::size_t>(i)];
            const variable_type weight =
                width * weight_rate_list_[static_cast<std::size_t>(i)];

            const variable_type var_plus = right - variable_distance;
            const variable_type var_minus = left + variable_distance;
            const result_type function_values =
                function(var_plus) + function(var_minus);
            if (!base::isfinite(function_values)) [[unlikely]] {
                this->logger().warning()(
                    "A function value was not a finite value. "
                    "Stopped numerical integration.");
                break;
            }
            sum += function_values * weight;
        }

        return sum.sum() * interval_;
    }

    /*!
     * \brief Integrate a function.
     *
     * \tparam Function Type of function.
     * \param[in] function Function.
     * \param[in] left Left boundary.
     * \param[in] right Right boundary.
     * \return Result.
     */
    template <base::concepts::invocable_as<result_type(variable_type)> Function>
    [[nodiscard]] auto operator()(const Function& function, variable_type left,
        variable_type right) const -> result_type {
        return integrate(function, left, right);
    }

    /*!
     * \brief Set maximum point in changed variable.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_point(variable_type val) -> tanh_finite_integrator& {
        NUM_COLLECT_ASSERT(val > constants::zero<variable_type>);
        max_point_ = val;
        calculate_coefficients();
        return *this;
    }

    /*!
     * \brief Set number of points.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto points(index_type val) -> tanh_finite_integrator& {
        NUM_COLLECT_ASSERT(val > constants::zero<index_type>);
        points_ = val;
        calculate_coefficients();
        return *this;
    }

private:
    /*!
     * \brief Calculate coefficients for integration.
     *
     * \note Set points_ before calling this function.
     */
    void calculate_coefficients() {
        using constants::one;

        variable_rate_list_.clear();
        variable_rate_list_.reserve(static_cast<std::size_t>(points_));
        weight_rate_list_.clear();
        weight_rate_list_.reserve(static_cast<std::size_t>(points_));

        interval_ = max_point_ / static_cast<variable_type>(points_);
        for (index_type i = 1; i <= points_; ++i) {
            const variable_type changed_variable =
                interval_ * static_cast<variable_type>(i);
            const variable_type exp_value =
                std::exp(static_cast<variable_type>(-2) * changed_variable);
            const variable_type denominator = one<variable_type> + exp_value;
            variable_rate_list_.push_back(exp_value / denominator);
            weight_rate_list_.push_back(static_cast<variable_type>(2) *
                exp_value / (denominator * denominator));
        }
    }

    //! Default maximum point in changed variable.
    static constexpr auto default_max_point =
        impl::tanh_finite_integrator_traits<Variable>::default_max_point;

    //! Maximum point in changed variable.
    variable_type max_point_{default_max_point};

    //! Default number of points.
    static constexpr index_type default_points = 50;

    //! Number of points.
    index_type points_{default_points};

    //! Interval of changed variable.
    variable_type interval_{};

    //! List of rates of distances of points from the upper bound.
    std::vector<variable_type> variable_rate_list_{};

    //! List of rates of weights of points.
    std::vector<variable_type> weight_rate_list_{};
};

}  // namespace num_collect::integration
