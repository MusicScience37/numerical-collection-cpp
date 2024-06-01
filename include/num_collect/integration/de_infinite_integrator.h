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
 * \brief Definition of de_infinite_integrator function.
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
#include "num_collect/constants/zero.h"  // IWYU pragma: keep
#include "num_collect/util/assert.h"

namespace num_collect::integration {

/*!
 * \brief Class to perform numerical integration on infinite range \f$(-\infty,
 * \infty)\f$ using double exponential rule.
 *
 * \tparam Signature Function signature.
 */
template <typename Signature>
class de_infinite_integrator;

/*!
 * \brief Class to perform numerical integration on infinite range \f$(-\infty,
 * \infty)\f$ using double exponential rule.
 *
 * \tparam Result Type of results.
 * \tparam Variable Type of variables.
 */
template <typename Result, base::concepts::real_scalar Variable>
class de_infinite_integrator<Result(Variable)> {
public:
    //! Type of variables.
    using variable_type = std::decay_t<Variable>;

    //! Type of results.
    using result_type = std::decay_t<Result>;

    /*!
     * \brief Constructor.
     */
    de_infinite_integrator() = default;

    /*!
     * \brief Integrate a function.
     *
     * \tparam Function Type of function.
     * \param[in] function Function.
     * \return Result.
     */
    template <base::concepts::invocable_as<result_type(variable_type)> Function>
    [[nodiscard]] auto integrate(
        const Function& function) const -> result_type {
        const variable_type interval =
            max_point_ / static_cast<variable_type>(points_);

        constexpr variable_type diff_coeff_center = half_pi;
        result_type sum =
            function(constants::zero<variable_type>) * diff_coeff_center;

        for (index_type i = 1; i < points_; ++i) {
            const variable_type changed_var =
                interval * static_cast<variable_type>(i);
            const variable_type half_pi_sinh = half_pi * std::sinh(changed_var);
            const variable_type var = std::sinh(half_pi_sinh);
            const variable_type diff_coeff =
                half_pi * std::cosh(half_pi_sinh) * std::cosh(changed_var);
            sum += function(var) * diff_coeff;
            sum += function(-var) * diff_coeff;
        }

        return sum * interval;
    }

    /*!
     * \brief Integrate a function.
     *
     * \tparam Function Type of function.
     * \param[in] function Function.
     * \return Result.
     */
    template <base::concepts::invocable_as<result_type(variable_type)> Function>
    [[nodiscard]] auto operator()(
        const Function& function) const -> result_type {
        return integrate(function);
    }

    /*!
     * \brief Set maximum point in changed variable.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_point(variable_type val) -> de_infinite_integrator& {
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
    auto points(index_type val) -> de_infinite_integrator& {
        NUM_COLLECT_ASSERT(val > constants::zero<index_type>);
        points_ = val;
        return *this;
    }

private:
    //! Half value of pi.
    static constexpr variable_type half_pi =
        constants::half<variable_type> * constants::pi<variable_type>;

    //! Default maximum point in changed variable.
    static constexpr auto default_max_point = static_cast<variable_type>(4);

    //! Maximum point in changed variable.
    variable_type max_point_{default_max_point};

    //! Default number of points.
    static constexpr index_type default_points = 20;

    //! Number of points.
    index_type points_{default_points};
};

}  // namespace num_collect::integration
