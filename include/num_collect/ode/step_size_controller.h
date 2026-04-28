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
 * \brief Definition of step_size_controller class.
 */
#pragma once

#include "num_collect/base/precondition.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/ode/concepts/problem.h"
#include "num_collect/ode/concepts/step_size_strategy.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/step_size_limits.h"

namespace num_collect::ode {

//! Log tag.
constexpr auto step_size_controller_log_tag =
    logging::log_tag_view("num_collect::ode::step_size_controller");

/*!
 * \brief Class to control step sizes adaptively.
 *
 * \tparam Problem Type of the problem.
 * \tparam Strategy Type of the strategy to calculate next step sizes.
 */
template <concepts::problem Problem, concepts::step_size_strategy Strategy>
class step_size_controller : public logging::logging_mixin {
public:
    //! Type of the problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    /*!
     * \brief Constructor.
     *
     * \param[in] method_order Order of the method.
     */
    explicit step_size_controller(index_type method_order)
        : logging::logging_mixin(step_size_controller_log_tag),
          strategy_(method_order) {}

    /*!
     * \brief Initialize the internal state.
     */
    void init() { strategy_.init(); }

    /*!
     * \brief Reduce step size if possible.
     *
     * \param[in,out] step_size Step size.
     * \return true Step size was reduced.
     * \return false Step size was not reduced.
     */
    [[nodiscard]] auto reduce_if_possible(scalar_type& step_size) -> bool {
        if (step_size > limits_.lower_limit()) {
            step_size *= reduction_rate_;
            step_size = limits_.apply(step_size);
            return true;
        }
        return false;
    }

    /*!
     * \brief Calculate the next step size.
     *
     * \param[in,out] step_size Step size.
     * \param[in] variable Variable.
     * \param[in] error Error estimate.
     */
    void calc_next(scalar_type& step_size, const variable_type& variable,
        const variable_type& error) {
        const scalar_type error_norm =
            this->tolerances().calc_norm(variable, error);
        strategy_.calc_next(step_size, error_norm);
        step_size = this->limits().apply(step_size);
    }

    /*!
     * \brief Set the limits of step sizes.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto limits(const step_size_limits<scalar_type>& val)
        -> step_size_controller& {
        limits_ = val;
        return *this;
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
    auto tolerances(const error_tolerances<variable_type>& val)
        -> step_size_controller& {
        tolerances_ = val;
        return *this;
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
    auto reduction_rate(const scalar_type& val) -> step_size_controller& {
        NUM_COLLECT_PRECONDITION(val > static_cast<scalar_type>(0),
            "Rate to reduce step sizes when error is large must be a positive "
            "value.");
        reduction_rate_ = val;
        return *this;
    }

    /*!
     * \brief Access the strategy.
     *
     * \return Strategy.
     */
    [[nodiscard]] auto strategy() noexcept -> Strategy& { return strategy_; }

    /*!
     * \brief Access the strategy.
     *
     * \return Strategy.
     */
    [[nodiscard]] auto strategy() const noexcept -> const Strategy& {
        return strategy_;
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

    //! Strategy to calculate next step sizes.
    Strategy strategy_;
};

}  // namespace num_collect::ode
