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
 * \brief Definition of conjugate_gradient_optimizer class.
 */
#pragma once

#include "num_collect/base/index_type.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/backtracking_line_searcher.h"
#include "num_collect/opt/concepts/line_searcher.h"
#include "num_collect/opt/concepts/multi_variate_differentiable_objective_function.h"
#include "num_collect/opt/descent_method_base.h"

namespace num_collect::opt {

//! Tag of conjugate_gradient_optimizer.
constexpr auto conjugate_gradient_optimizer_tag =
    logging::log_tag_view("num_collect::opt::conjugate_gradient_optimizer");

/*!
 * \brief Class of conjugate gradient method for optimization.
 *
 * \warning This implementation assumes Eigen's dense vector types for variable
 * types.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 * \tparam LineSearcher Type of class to perform line search.
 *
 * \todo Conjugate gradient method for optimization may need to be fixed for
 * faster convergence.
 */
template <
    concepts::multi_variate_differentiable_objective_function ObjectiveFunction,
    concepts::line_searcher LineSearcher =
        backtracking_line_searcher<ObjectiveFunction>>
class conjugate_gradient_optimizer
    : public descent_method_base<
          conjugate_gradient_optimizer<ObjectiveFunction, LineSearcher>,
          LineSearcher> {
public:
    //! This class.
    using this_type =
        conjugate_gradient_optimizer<ObjectiveFunction, LineSearcher>;

    //! Type of base class.
    using base_type = descent_method_base<this_type, LineSearcher>;

    using typename base_type::objective_function_type;
    using typename base_type::variable_type;

    //! Type of scalars in variables.
    using variable_scalar_type = typename variable_type::Scalar;

    /*!
     * \brief Constructor.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit conjugate_gradient_optimizer(
        const objective_function_type& obj_fun = objective_function_type())
        : base_type(conjugate_gradient_optimizer_tag, obj_fun) {}

    using base_type::evaluations;
    using base_type::gradient;
    using base_type::gradient_norm;
    using base_type::iterations;
    using base_type::line_searcher;
    using base_type::opt_value;
    using base_type::opt_variable;
    using typename base_type::value_type;

    /*!
     * \brief Initialize.
     *
     * \param[in] init_variable Initial variable.
     */
    void init(const variable_type& init_variable) {
        base_type::init(init_variable);
        has_first_iteration_done_ = false;
    }

    /*!
     * \copydoc num_collect::opt::descent_method_base::calc_direction
     */
    [[nodiscard]] auto calc_direction() -> variable_type {
        if (has_first_iteration_done_) {
            variable_scalar_type prev_coeff =
                gradient().dot(gradient() - prev_grad_) /
                prev_grad_.squaredNorm();
            if (prev_coeff < static_cast<variable_scalar_type>(0)) {
                prev_coeff = static_cast<variable_scalar_type>(0);
            }
            direction_ = -gradient() + prev_coeff * direction_;
            prev_grad_ = gradient();
        } else {
            direction_ = -gradient();
            prev_grad_ = gradient();
            has_first_iteration_done_ = true;
        }
        return direction_;
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

private:
    //! Previous gradient.
    variable_type prev_grad_{};

    //! Search direction.
    variable_type direction_{};

    //! Whether the first iteration has been done.
    bool has_first_iteration_done_{false};
};

}  // namespace num_collect::opt
