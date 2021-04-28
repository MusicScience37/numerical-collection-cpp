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

#include "num_collect/opt/backtracking_line_searcher.h"
#include "num_collect/opt/descent_method_base.h"

namespace num_collect::opt {

/*!
 * \brief Class of steepest descent method.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 * \tparam LineSearcher Type of class to perform line search.
 */
template <typename ObjectiveFunction,
    typename LineSearcher = backtracking_line_searcher<ObjectiveFunction>>
class steepest_descent
    : public descent_method_base<
          steepest_descent<ObjectiveFunction, LineSearcher>, LineSearcher> {
public:
    //! Type of base class.
    using base_type =
        descent_method_base<steepest_descent<ObjectiveFunction, LineSearcher>,
            LineSearcher>;

    //! Type of function values.
    using value_type = typename LineSearcher::value_type;

    using base_type::evaluations;
    using base_type::gradient;
    using base_type::iterations;
    using base_type::opt_value;
    using typename base_type::objective_function_type;

    /*!
     * \brief Construct.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit steepest_descent(
        const objective_function_type& obj_fun = objective_function_type())
        : base_type(obj_fun) {}

    /*!
     * \copydoc num_collect::opt::descent_method_base::calc_direction
     */
    [[nodiscard]] auto calc_direction() { return -gradient(); }

    /*!
     * \brief Calculate norm of gradient.
     *
     * \return Norm of gradient.
     */
    [[nodiscard]] auto gradient_norm() const -> value_type {
        return gradient().norm();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return gradient_norm() < tol_grad_norm_;
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

    /*!
     * \brief Set tolerance of norm of gradient.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto tol_gradient_norm(const value_type& value) -> steepest_descent& {
        tol_grad_norm_ = value;
        return *this;
    }

private:
    //! Default tolerance of norm of gradient.
    static inline const auto default_tol_grad_norm =
        static_cast<value_type>(1e-3);

    //! Tolerance of norm of gradient.
    value_type tol_grad_norm_{default_tol_grad_norm};
};

}  // namespace num_collect::opt
