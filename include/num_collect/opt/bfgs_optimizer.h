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
 * \brief Definition of bfgs_optimizer class.
 */
#pragma once

#include <string_view>

#include <Eigen/Cholesky>
#include <Eigen/Core>

#include "num_collect/base/concepts/eigen_solver_of.h"  // IWYU pragma: keep
#include "num_collect/base/index_type.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/backtracking_line_searcher.h"
#include "num_collect/opt/concepts/line_searcher.h"  // IWYU pragma: keep
#include "num_collect/opt/concepts/multi_variate_differentiable_objective_function.h"  // IWYU pragma: keep
#include "num_collect/opt/descent_method_base.h"

namespace num_collect::opt {

//! Tag of bfgs_optimizer.
constexpr auto bfgs_optimizer_tag =
    logging::log_tag_view("num_collect::opt::bfgs_optimizer");

/*!
 * \brief Class of quasi-Newton method with Broyden-Fletcher-Goldfarb-Shanno
 * (BFGS) formula.
 *
 * \warning This implementation assumes Eigen's dense vector types for variable
 * types.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 * \tparam LineSearcher Type of class to perform line search.
 * \tparam HessianSolver Type of solvers of linear equation of Hessian.
 */
template <
    concepts::multi_variate_differentiable_objective_function ObjectiveFunction,
    concepts::line_searcher LineSearcher =
        backtracking_line_searcher<ObjectiveFunction>,
    base::concepts::eigen_solver_of<
        Eigen::MatrixX<typename ObjectiveFunction::value_type>,
        typename ObjectiveFunction::variable_type>
        HessianSolver =
            Eigen::LLT<Eigen::MatrixX<typename ObjectiveFunction::value_type>>>
class bfgs_optimizer
    : public descent_method_base<
          bfgs_optimizer<ObjectiveFunction, LineSearcher, HessianSolver>,
          LineSearcher> {
public:
    //! This class.
    using this_type =
        bfgs_optimizer<ObjectiveFunction, LineSearcher, HessianSolver>;

    //! Type of base class.
    using base_type = descent_method_base<this_type, LineSearcher>;

    using typename base_type::objective_function_type;
    using typename base_type::variable_type;

    //! Type of scalars in variables.
    using variable_scalar_type = typename variable_type::Scalar;

    //! Type of solvers of linear equation of Hessian.
    using hessian_solver_type = HessianSolver;

    //! Type of Hessian.
    using hessian_type = typename hessian_solver_type::MatrixType;

    /*!
     * \brief Constructor.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit bfgs_optimizer(
        const objective_function_type& obj_fun = objective_function_type())
        : base_type(bfgs_optimizer_tag, obj_fun) {}

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
        const auto dimensions = init_variable.size();
        approx_hessian_ = hessian_type::Identity(dimensions, dimensions);
        has_first_iteration_done_ = false;
    }

    /*!
     * \copydoc num_collect::opt::descent_method_base::calc_direction
     */
    [[nodiscard]] auto calc_direction() -> variable_type {
        if (has_first_iteration_done_) {
            diff_var_ = opt_variable() - prev_var_;
            diff_grad_ = gradient() - prev_grad_;
            hessian_var_ = approx_hessian_ * diff_var_;
            approx_hessian_ = approx_hessian_ +
                (diff_grad_ * diff_grad_.transpose()) /
                    (diff_grad_.dot(diff_var_)) -
                (hessian_var_ * hessian_var_.transpose()) /
                    (diff_var_.dot(hessian_var_));
        }
        prev_var_ = opt_variable();
        prev_grad_ = gradient();
        has_first_iteration_done_ = true;

        solver_.compute(approx_hessian_);
        return -solver_.solve(prev_grad_);
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
    //! Approximate Hessian.
    hessian_type approx_hessian_{};

    //! Previous variable.
    variable_type prev_var_{};

    //! Previous gradient.
    variable_type prev_grad_{};

    //! Difference of variable.
    variable_type diff_var_{};

    //! Difference of gradient.
    variable_type diff_grad_{};

    //! approx_hessian_ * diff_var_
    variable_type hessian_var_{};

    //! Whether the first iteration has been done.
    bool has_first_iteration_done_{false};

    //! Solver of linear equation of Hessian.
    hessian_solver_type solver_{};
};

}  // namespace num_collect::opt
