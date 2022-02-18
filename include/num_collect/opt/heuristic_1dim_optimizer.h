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
 * \brief Definition of heuristic_1dim_optimizer class.
 */
#pragma once

#include <limits>
#include <type_traits>
#include <vector>

#include "num_collect/base/index_type.h"
#include "num_collect/opt/concepts/single_variate_objective_function.h"
#include "num_collect/opt/golden_section_search.h"
#include "num_collect/opt/optimizer_base.h"
#include "num_collect/opt/sampling_optimizer.h"
#include "num_collect/util/assert.h"

namespace num_collect::opt {

//! Tag of dividing_rectangles.
inline constexpr auto heuristic_1dim_optimizer_tag =
    logging::log_tag_view("num_collect::opt::heuristic_1dim_optimizer");

/*!
 * \brief Class to perform global optimization in 1 dimension using heuristics.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <concepts::single_variate_objective_function ObjectiveFunction>
class heuristic_1dim_optimizer
    : public optimizer_base<heuristic_1dim_optimizer<ObjectiveFunction>> {
public:
    //! Type of the objective function.
    using objective_function_type = ObjectiveFunction;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

    static_assert(std::is_same_v<variable_type, value_type>);

    /*!
     * \brief Construct.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit heuristic_1dim_optimizer(
        const objective_function_type& obj_fun = objective_function_type())
        : optimizer_base<heuristic_1dim_optimizer<ObjectiveFunction>>(
              heuristic_1dim_optimizer_tag),
          opt1_(obj_fun),
          opt2_(obj_fun) {}

    /*!
     * \brief Initialize the algorithm.
     *
     * \param[in] lower Lower limit.
     * \param[in] upper Upper limit.
     */
    void init(const variable_type& lower, const variable_type& upper) {
        opt1_.init(lower, upper);
        opt2_.init(opt1_.lower(), opt1_.upper());
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() { opt2_.iterate(); }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return opt2_.is_stop_criteria_satisfied();
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::configure_iteration_logger
     */
    void configure_iteration_logger(
        logging::iteration_logger& iteration_logger) const {
        iteration_logger.append<index_type>(
            "Iter.", [this] { return iterations(); });
        iteration_logger.append<index_type>(
            "Eval.", [this] { return evaluations(); });
        iteration_logger.append<value_type>(
            "Value", [this] { return opt_value(); });
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_variable
     */
    [[nodiscard]] auto opt_variable() const -> variable_type {
        return opt2_.opt_variable();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_value
     */
    [[nodiscard]] auto opt_value() const -> value_type {
        return opt2_.opt_value();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterations
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return opt1_.iterations() + opt2_.iterations();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::evaluations
     */
    [[nodiscard]] auto evaluations() const noexcept -> index_type {
        return opt1_.evaluations() + opt2_.evaluations();
    }

private:
    //! First optimizer.
    sampling_optimizer<objective_function_type> opt1_;

    //! Second optimizer.
    golden_section_search<objective_function_type> opt2_;
};

}  // namespace num_collect::opt
