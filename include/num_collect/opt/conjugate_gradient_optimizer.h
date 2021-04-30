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

#include <Eigen/Cholesky>

#include "num_collect/opt/backtracking_line_searcher.h"
#include "num_collect/opt/descent_method_base.h"

namespace num_collect::opt {

/*!
 * \brief Class of conjugate gradient method for optimization.
 *
 * \warning This implementation assumes Eigen's dense vector types for variable
 * types.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 * \tparam LineSearcher Type of class to perform line search.
 */
template <typename ObjectiveFunction,
    typename LineSearcher = backtracking_line_searcher<ObjectiveFunction>>
class conjugate_gradient_optimizer
    : public descent_method_base<
          conjugate_gradient_optimizer<ObjectiveFunction, LineSearcher>,
          LineSearcher> {
public:
    //! Type of base class.
    using base_type = descent_method_base<
        conjugate_gradient_optimizer<ObjectiveFunction, LineSearcher>,
        LineSearcher>;

    using typename base_type::objective_function_type;
    using typename base_type::variable_type;

    //! Type of scalars in variables.
    using variable_scalar_type = typename variable_type::Scalar;

    /*!
     * \brief Construct.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit conjugate_gradient_optimizer(
        const objective_function_type& obj_fun = objective_function_type())
        : base_type(obj_fun) {}

    using base_type::evaluations;
    using base_type::gradient;
    using base_type::gradient_norm;
    using base_type::iterations;
    using base_type::line_searcher;
    using base_type::opt_value;
    using base_type::opt_variable;

    /*!
     * \brief Initialize.
     *
     * \param[in] init_variable Initial variable.
     */
    void init(const variable_type& init_variable) {
        base_type::init(init_variable);
        const auto dimensions = init_variable.size();
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
            if (prev_coeff < variable_scalar_type(0)) {
                prev_coeff = variable_scalar_type(0);
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
     * \copydoc num_collect::opt::optimizer_base::set_info_to
     */
    void set_info_to(iteration_logger& logger) const {
        logger["Iter."] = iterations();
        logger["Eval."] = evaluations();
        logger["Value"] = static_cast<double>(opt_value());
        logger["Grad."] = static_cast<double>(gradient_norm());
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
