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
 * \brief Definition of newton_optimizer class.
 */
#pragma once

#include <string_view>

#include <Eigen/Cholesky>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/backtracking_line_searcher.h"
#include "num_collect/opt/concepts/line_searcher.h"  // IWYU pragma: keep
#include "num_collect/opt/concepts/multi_variate_twice_differentiable_objective_function.h"  // IWYU pragma: keep
#include "num_collect/opt/descent_method_base.h"

namespace num_collect::opt {

//! Tag of newton_optimizer.
inline constexpr auto newton_optimizer_tag =
    logging::log_tag_view("num_collect::opt::newton_optimizer");

/*!
 * \brief Class of newton method for optimization.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 * \tparam LineSearcher Type of class to perform line search.
 * \tparam HessianSolver Type of solvers of linear equation of Hessian.
 */
template <concepts::multi_variate_twice_differentiable_objective_function
              ObjectiveFunction,
    concepts::line_searcher LineSearcher =
        backtracking_line_searcher<ObjectiveFunction>,
    typename HessianSolver =
        Eigen::LLT<typename ObjectiveFunction::hessian_type>>
class newton_optimizer
    : public descent_method_base<
          newton_optimizer<ObjectiveFunction, LineSearcher, HessianSolver>,
          LineSearcher> {
public:
    //! Type of base class.
    using base_type = descent_method_base<
        newton_optimizer<ObjectiveFunction, LineSearcher, HessianSolver>,
        LineSearcher>;

    using typename base_type::objective_function_type;
    using typename base_type::variable_type;

    //! Type of Hessian.
    using hessian_type = typename objective_function_type::hessian_type;

    //! Type of solvers of linear equation of Hessian.
    using hessian_solver_type = HessianSolver;

    /*!
     * \brief Constructor.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit newton_optimizer(
        const objective_function_type& obj_fun = objective_function_type())
        : base_type(newton_optimizer_tag, obj_fun) {}

    using base_type::evaluations;
    using base_type::gradient;
    using base_type::gradient_norm;
    using base_type::iterations;
    using base_type::line_searcher;
    using base_type::opt_value;
    using typename base_type::value_type;

    /*!
     * \brief Get Hessian for current optimal variable.
     *
     * \return Hessian for current optimal variable.
     */
    [[nodiscard]] auto hessian() const -> const hessian_type& {
        return line_searcher().obj_fun().hessian();
    }

    /*!
     * \copydoc num_collect::opt::descent_method_base::calc_direction
     */
    [[nodiscard]] auto calc_direction() -> variable_type {
        return calc_direction_impl(hessian());
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
        iteration_logger.append<value_type>(
            "Grad.", [this] { return gradient_norm(); });
    }

private:
    /*!
     * \brief Calculate search direction.
     *
     * \param[in] hessian Hessian.
     * \return Search direction.
     */
    [[nodiscard]] auto calc_direction_impl(const hessian_type& hessian)
        -> variable_type {
        solver_.compute(hessian);
        return -solver_.solve(gradient());
    }

    //! Solver of linear equation of Hessian.
    hessian_solver_type solver_;
};

}  // namespace num_collect::opt
