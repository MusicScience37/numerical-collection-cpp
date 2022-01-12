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
 * \brief Definition of dfp_optimizer class.
 */
#pragma once

#include <Eigen/Cholesky>

#include "num_collect/opt/backtracking_line_searcher.h"
#include "num_collect/opt/descent_method_base.h"

namespace num_collect::opt {

//! Tag of dfp_optimizer.
inline constexpr auto dfp_optimizer_tag =
    logging::log_tag_view("num_collect::opt::dfp_optimizer");

/*!
 * \brief Class of quasi-Newton method with Davidon-Fletcher-Powell (DFP)
 * formula.
 *
 * \warning This implementation assumes Eigen's dense vector types for variable
 * types.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 * \tparam LineSearcher Type of class to perform line search.
 * \tparam Hessian Type of Hessian matrix.
 */
template <typename ObjectiveFunction,
    typename LineSearcher = backtracking_line_searcher<ObjectiveFunction>,
    typename Hessian = Eigen::MatrixXd>
class dfp_optimizer
    : public descent_method_base<
          dfp_optimizer<ObjectiveFunction, LineSearcher, Hessian>,
          LineSearcher> {
public:
    //! Type of base class.
    using base_type = descent_method_base<
        dfp_optimizer<ObjectiveFunction, LineSearcher, Hessian>, LineSearcher>;

    using typename base_type::objective_function_type;
    using typename base_type::variable_type;

    //! Type of scalars in variables.
    using variable_scalar_type = typename variable_type::Scalar;

    //! Type of Hessian.
    using hessian_type = Hessian;

    /*!
     * \brief Construct.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit dfp_optimizer(
        const objective_function_type& obj_fun = objective_function_type())
        : base_type(dfp_optimizer_tag, obj_fun) {}

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
            hessian_grad_ = approx_hessian_ * diff_grad_;
            approx_hessian_ = approx_hessian_ +
                (diff_var_ * diff_var_.transpose()) /
                    (diff_var_.dot(diff_grad_)) -
                (hessian_grad_ * hessian_grad_.transpose()) /
                    (diff_grad_.dot(hessian_grad_));
        }
        prev_var_ = opt_variable();
        prev_grad_ = gradient();
        has_first_iteration_done_ = true;

        return -approx_hessian_ * prev_grad_;
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
    //! Approxymate Hessian.
    hessian_type approx_hessian_{};

    //! Previous variable.
    variable_type prev_var_{};

    //! Previous gradient.
    variable_type prev_grad_{};

    //! Difference of variable.
    variable_type diff_var_{};

    //! Difference of gradient.
    variable_type diff_grad_{};

    //! approx_hessian_ * diff_grad_
    variable_type hessian_grad_{};

    //! Whether the first iteration has been done.
    bool has_first_iteration_done_{false};
};

}  // namespace num_collect::opt
