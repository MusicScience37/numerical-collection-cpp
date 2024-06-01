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
 * \brief Definition of steepest_descent class.
 */
#pragma once

#include <string_view>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/backtracking_line_searcher.h"
#include "num_collect/opt/concepts/differentiable_objective_function.h"  // IWYU pragma: keep
#include "num_collect/opt/concepts/line_searcher.h"  // IWYU pragma: keep
#include "num_collect/opt/descent_method_base.h"

namespace num_collect::opt {

//! Tag of steepest_descent.
constexpr auto steepest_descent_tag =
    logging::log_tag_view("num_collect::opt::steepest_descent");

/*!
 * \brief Class of steepest descent method.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 * \tparam LineSearcher Type of class to perform line search.
 */
template <concepts::differentiable_objective_function ObjectiveFunction,
    concepts::line_searcher LineSearcher =
        backtracking_line_searcher<ObjectiveFunction>>
class steepest_descent
    : public descent_method_base<
          steepest_descent<ObjectiveFunction, LineSearcher>, LineSearcher> {
public:
    //! This class.
    using this_type = steepest_descent<ObjectiveFunction, LineSearcher>;

    //! Type of base class.
    using base_type = descent_method_base<this_type, LineSearcher>;

    using base_type::evaluations;
    using base_type::gradient;
    using base_type::gradient_norm;
    using base_type::iterations;
    using base_type::opt_value;
    using typename base_type::objective_function_type;
    using typename base_type::value_type;
    using typename base_type::variable_type;

    /*!
     * \brief Constructor.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit steepest_descent(
        const objective_function_type& obj_fun = objective_function_type())
        : base_type(steepest_descent_tag, obj_fun) {}

    /*!
     * \copydoc num_collect::opt::descent_method_base::calc_direction
     */
    [[nodiscard]] auto calc_direction() const -> variable_type {
        return -gradient();
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::configure_iteration_logger
     */
    void configure_iteration_logger(
        logging::iterations::iteration_logger<this_type>& iteration_logger)
        const {
        iteration_logger.template append<index_type>(
            "Iter.", &base_type::iterations);
        iteration_logger.template append<index_type>(
            "Eval.", &base_type::evaluations);
        iteration_logger.template append<value_type>(
            "Value", &base_type::opt_value);
        iteration_logger.template append<value_type>(
            "Grad.", &base_type::gradient_norm);
    }
};

}  // namespace num_collect::opt
