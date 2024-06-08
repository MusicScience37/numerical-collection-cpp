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
 * \brief Definition of step_size_controller_base class.
 */
#pragma once

#include "num_collect/base/exception.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/ode/concepts/formula.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/step_size_limits.h"

namespace num_collect::ode {

/*!
 * \brief Base class of classes to control step sizes.
 *
 * \tparam Derived Type of derived class.
 * \tparam Formula Type of the formula.
 */
template <typename Derived, concepts::formula Formula>
class step_size_controller_base : public logging::logging_mixin {
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
     * \brief Set the limits of step sizes.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto limits(const step_size_limits<scalar_type>& val) -> Derived& {
        limits_ = val;
        return derived();
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
    auto tolerances(const error_tolerances<variable_type>& val) -> Derived& {
        tolerances_ = val;
        return derived();
    }

    /*!
     * \brief Get the error tolerances.
     *
     * \return Error tolerances.
     */
    [[nodiscard]] auto tolerances() const
        -> const error_tolerances<variable_type>& {
        return tolerances_;
    }

    /*!
     * \brief Set the rate to reduce step sizes when error is large.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto reduction_rate(const scalar_type& val) -> Derived& {
        if (val <= static_cast<scalar_type>(0)) {
            throw invalid_argument(
                "Rate to reduce step sizes when error is large must be a "
                "positive value.");
        }
        reduction_rate_ = val;
        return derived();
    }

protected:
    /*!
     * \brief Constructor.
     *
     * \param[in] tag Log tag.
     */
    explicit step_size_controller_base(logging::log_tag_view tag)
        : logging::logging_mixin(tag) {}

    /*!
     * \brief Access derived object.
     *
     * \return Reference to the derived object.
     */
    [[nodiscard]] auto derived() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }

    /*!
     * \brief Access derived object.
     *
     * \return Reference to the derived object.
     */
    [[nodiscard]] auto derived() const noexcept -> const Derived& {
        return *static_cast<const Derived*>(this);
    }

    /*!
     * \brief Reduce step size if needed.
     *
     * \param[in,out] step_size Step size.
     * \param[in] variable Variable.
     * \param[in] error Error estimate.
     * \retval true Step size was reduced.
     * \retval false Otherwise.
     */
    [[nodiscard]] auto reduce_if_needed(scalar_type& step_size,
        const variable_type& variable, const variable_type& error) -> bool {
        const bool tolerance_satisfied = tolerances().check(variable, error);
        if (!tolerance_satisfied) {
            if (step_size > limits_.lower_limit()) {
                this->logger().trace()(
                    "Error tolerance not satisfied with step size {}.",
                    step_size);
                step_size *= reduction_rate_;
                step_size = limits_.apply(step_size);
                return true;
            }
            this->logger().warning()(
                "Error tolerance not satisfied even with the lowest step size "
                "{} (error: {}).",
                step_size, tolerances().calc_norm(variable, error));
        }
        return false;
    }

private:
    //! Limits of step sizes.
    step_size_limits<scalar_type> limits_{};

    //! Error tolerances.
    error_tolerances<variable_type> tolerances_{};

    //! Default rate to reduce step sizes when error is large.
    static constexpr auto default_reduction_rate =
        static_cast<scalar_type>(0.5);

    //! Rate to reduce step sizes when error is large.
    scalar_type reduction_rate_{default_reduction_rate};
};

}  // namespace num_collect::ode
