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
 * \brief Definition of de_finite_integrator function.
 */
#pragma once

#include <cmath>
#include <type_traits>

#include "num_collect/base/concepts/invocable_as.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar.h"   // IWYU pragma: keep
#include "num_collect/base/index_type.h"
#include "num_collect/constants/half.h"  // IWYU pragma: keep
#include "num_collect/constants/one.h"   // IWYU pragma: keep
#include "num_collect/constants/pi.h"    // IWYU pragma: keep
#include "num_collect/constants/two.h"   // IWYU pragma: keep
#include "num_collect/constants/zero.h"  // IWYU pragma: keep
#include "num_collect/util/assert.h"

namespace num_collect::integration {

namespace impl {

/*!
 * \brief Helper class of constants for use in de_finite_integrator class.
 *
 * \tparam Variable Type of variables.
 */
template <typename Variable>
struct de_finite_integrator_traits;

/*!
 * \brief Implementation of de_finite_integrator_traits for float.
 */
template <>
struct de_finite_integrator_traits<float> {
public:
    //! Default maximum point in changed variable.
    static constexpr float default_max_point = 2.4F;
};

/*!
 * \brief Implementation of de_finite_integrator_traits for float.
 */
template <>
struct de_finite_integrator_traits<double> {
public:
    //! Default maximum point in changed variable.
    static constexpr double default_max_point = 3.0;
};

}  // namespace impl

/*!
 * \brief Class to perform numerical integration on finite range using double
 * exponential rule.
 *
 * \tparam Signature Function signature.
 */
template <typename Signature>
class de_finite_integrator;

/*!
 * \brief Class to perform numerical integration on finite range using double
 * exponential rule.
 *
 * \tparam Result Type of results.
 * \tparam Variable Type of variables.
 */
template <typename Result, base::concepts::real_scalar Variable>
class de_finite_integrator<Result(Variable)> {
public:
    //! Type of variables.
    using variable_type = std::decay_t<Variable>;

    //! Type of results.
    using result_type = std::decay_t<Result>;

    /*!
     * \brief Constructor.
     */
    de_finite_integrator() = default;

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
        const variable_type half_width =
            constants::half<variable_type> * (right - left);
        const variable_type interval =
            max_point_ / static_cast<variable_type>(points_);
        constexpr variable_type half_pi =
            constants::half<variable_type> * constants::pi<variable_type>;

        const variable_type diff_coeff_center = half_pi * half_width;
        result_type sum = function(center) * diff_coeff_center;

        for (index_type i = 1; i <= points_; ++i) {
            const variable_type changed_var =
                interval * static_cast<variable_type>(i);
            const variable_type tanh_value =
                std::tanh(half_pi * std::sinh(changed_var));
            const variable_type diff_coeff_value =
                diff_coeff(changed_var, half_width);

            const variable_type var_plus = center + half_width * tanh_value;
            sum += function(var_plus) * diff_coeff_value;

            const variable_type var_minus = center - half_width * tanh_value;
            sum += function(var_minus) * diff_coeff_value;
        }

        return sum * interval;
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
    auto max_point(variable_type val) -> de_finite_integrator& {
        NUM_COLLECT_ASSERT(val > constants::zero<variable_type>);
        max_point_ = val;
        return *this;
    }

    /*!
     * \brief Set number of points.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto points(index_type val) -> de_finite_integrator& {
        NUM_COLLECT_ASSERT(val > constants::zero<index_type>);
        points_ = val;
        return *this;
    }

private:
    /*!
     * \brief Calculate differential coefficient for change of variable.
     *
     * \note This assumes that input is a positive number.
     *
     * \param[in] changed_var Changed variable.
     * \param[in] half_width Half of width of the range to integrate on.
     * \return Differential coefficient.
     */
    [[nodiscard]] static auto diff_coeff(
        variable_type changed_var, variable_type half_width) -> variable_type {
        const variable_type exp_value =
            std::exp(-constants::pi<variable_type> * std::sinh(changed_var));
        const variable_type exp_value_p1 =
            constants::one<variable_type> + exp_value;
        return constants::two<variable_type> * constants::pi<variable_type> *
            half_width * std::cosh(changed_var) * exp_value /
            (exp_value_p1 * exp_value_p1);
    }

    //! Default maximum point in changed variable.
    static constexpr auto default_max_point =
        impl::de_finite_integrator_traits<Variable>::default_max_point;

    //! Maximum point in changed variable.
    variable_type max_point_{default_max_point};

    //! Default number of points.
    static constexpr index_type default_points = 20;

    //! Number of points.
    index_type points_{default_points};
};

}  // namespace num_collect::integration
