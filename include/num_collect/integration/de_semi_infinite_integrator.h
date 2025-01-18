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
 * \brief Definition of de_semi_infinite_integrator function.
 */
#pragma once

#include <cmath>
#include <type_traits>

#include "num_collect/base/concepts/invocable_as.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/constants/half.h"  // IWYU pragma: keep
#include "num_collect/constants/one.h"   // IWYU pragma: keep
#include "num_collect/constants/pi.h"    // IWYU pragma: keep
#include "num_collect/constants/two.h"   // IWYU pragma: keep
#include "num_collect/constants/zero.h"  // IWYU pragma: keep
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"

namespace num_collect::integration {

//! Tag of de_semi_infinite_integrator.
constexpr auto de_semi_infinite_integrator_tag = logging::log_tag_view(
    "num_collect::integration::de_semi_infinite_integrator");

namespace impl {

/*!
 * \brief Helper class of constants for use in de_semi_infinite_integrator
 * class.
 *
 * \tparam Variable Type of variables.
 */
template <typename Variable>
struct de_semi_infinite_integrator_traits;

/*!
 * \brief Implementation of de_semi_infinite_integrator_traits for float.
 */
template <>
struct de_semi_infinite_integrator_traits<float> {
public:
    //! Default maximum point in changed variable.
    static constexpr float default_max_point = 3.0F;
};

/*!
 * \brief Implementation of de_semi_infinite_integrator_traits for float.
 */
template <>
struct de_semi_infinite_integrator_traits<double> {
public:
    //! Default maximum point in changed variable.
    static constexpr double default_max_point = 4.0;
};

}  // namespace impl

/*!
 * \brief Class to perform numerical integration on semi-infinite range \f$(0,
 * \infty)\f$ using double exponential rule.
 *
 * \tparam Signature Function signature.
 */
template <typename Signature>
class de_semi_infinite_integrator;

/*!
 * \brief Class to perform numerical integration on semi-infinite range \f$(0,
 * \infty)\f$ using double exponential rule.
 *
 * \tparam Result Type of results.
 * \tparam Variable Type of variables.
 */
template <typename Result, base::concepts::real_scalar Variable>
class de_semi_infinite_integrator<Result(Variable)>
    : public logging::logging_mixin {
public:
    //! Type of variables.
    using variable_type = std::decay_t<Variable>;

    //! Type of results.
    using result_type = std::decay_t<Result>;

    /*!
     * \brief Constructor.
     */
    de_semi_infinite_integrator()
        : logging::logging_mixin(de_semi_infinite_integrator_tag) {}

    /*!
     * \brief Integrate a function.
     *
     * \tparam Function Type of function.
     * \param[in] function Function.
     * \return Result.
     */
    template <base::concepts::invocable_as<result_type(variable_type)> Function>
    [[nodiscard]] auto integrate(const Function& function) const
        -> result_type {
        const variable_type interval =
            max_point_ / static_cast<variable_type>(points_);

        constexpr variable_type diff_coeff_center =
            constants::pi<variable_type>;
        result_type sum =
            function(constants::one<variable_type>) * diff_coeff_center;

        for (index_type i = 1; i < points_; ++i) {
            const variable_type changed_var =
                interval * static_cast<variable_type>(i);
            const variable_type pi_sinh_value =
                constants::pi<variable_type> * std::sinh(changed_var);

            const variable_type var_plus = std::exp(pi_sinh_value);
            const variable_type diff_coeff_plus = diff_coeff(changed_var);
            sum += function(var_plus) * diff_coeff_plus;

            const variable_type var_minus =
                constants::one<variable_type> / var_plus;
            const variable_type diff_coeff_minus = diff_coeff(-changed_var);
            sum += function(var_minus) * diff_coeff_minus;
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
    [[nodiscard]] auto operator()(const Function& function) const
        -> result_type {
        return integrate(function);
    }

    /*!
     * \brief Set maximum point in changed variable.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_point(variable_type val) -> de_semi_infinite_integrator& {
        NUM_COLLECT_PRECONDITION(val > static_cast<variable_type>(0),
            this->logger(), "Maximum point must be a positive value.");
        max_point_ = val;
        return *this;
    }

    /*!
     * \brief Set number of points.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto points(index_type val) -> de_semi_infinite_integrator& {
        NUM_COLLECT_PRECONDITION(val > 0, this->logger(),
            "Number of points must be a positive integer.");
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
     * \return Differential coefficient.
     */
    [[nodiscard]] static auto diff_coeff(variable_type changed_var)
        -> variable_type {
        return constants::pi<variable_type> *
            std::exp(constants::pi<variable_type> * std::sinh(changed_var)) *
            std::cosh(changed_var);
    }

    //! Default maximum point in changed variable.
    static constexpr auto default_max_point =
        impl::de_semi_infinite_integrator_traits<Variable>::default_max_point;

    //! Maximum point in changed variable.
    variable_type max_point_{default_max_point};

    //! Default number of points.
    static constexpr index_type default_points = 20;

    //! Number of points.
    index_type points_{default_points};
};

}  // namespace num_collect::integration
