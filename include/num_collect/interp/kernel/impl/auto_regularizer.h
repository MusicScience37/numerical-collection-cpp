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
 * \brief Definition of auto_regularizer class.
 */
#pragma once

#include <cmath>
#include <utility>

#include "num_collect/opt/function_object_wrapper.h"
#include "num_collect/opt/heuristic_global_optimizer.h"
#include "num_collect/regularization/impl/coeff_param.h"

namespace num_collect::interp::kernel::impl {

/*!
 * \brief Class of objective function in auto_regularizer class.
 *
 * \tparam Solver Type of the solver (a type of self_adjoint_kernel_solver).
 */
template <typename Solver>
class auto_regularizer_objective_function {
public:
    //! Type of the solver.
    using solver_type = Solver;

    //! Type of scalars.
    using scalar_type = typename solver_type::scalar_type;

    /*!
     * \brief Construct.
     *
     * \param[in] solver Solver.
     */
    explicit auto_regularizer_objective_function(const solver_type& solver)
        : solver_(&solver) {}

    /*!
     * \brief Calculate value.
     *
     * \param[in] log_reg_param Logarithm of the regularization parameter.
     * \return Value.
     */
    [[nodiscard]] auto operator()(const scalar_type& log_reg_param) const {
        using std::pow;
        const scalar_type reg_param =
            pow(static_cast<scalar_type>(10), log_reg_param);
        return solver_->calc_mle_objective(reg_param);
    }

private:
    //! Solver.
    const solver_type* solver_;
};

/*!
 * \brief Class to determine regularization parameters in kernels.
 *
 * \tparam Solver Type of the solver (a type of self_adjoint_kernel_solver).
 */
template <typename Solver>
class auto_regularizer {
public:
    //! Type of the solver.
    using solver_type = Solver;

    //! Type of scalars.
    using scalar_type = typename solver_type::scalar_type;

    //! Type of the objective function.
    using objective_function_type =
        auto_regularizer_objective_function<solver_type>;

    /*!
     * \brief Construct.
     *
     * \param[in] solver Solver.
     */
    explicit auto_regularizer(const solver_type& solver)
        : solver_(&solver),
          optimizer_(
              opt::make_function_object_wrapper<scalar_type(scalar_type)>(
                  objective_function_type(solver))) {}

    /*!
     * \brief Optimize the regularization parameter.
     */
    void optimize() {
        const auto [min_param, max_param] = param_search_region();
        using std::log10;
        const scalar_type log_min_param = log10(min_param);
        const scalar_type log_max_param = log10(max_param);
        optimizer_.init(log_min_param, log_max_param);
        optimizer_.solve();
    }

    /*!
     * \brief Get the optimal regularization parameter.
     *
     * \return Optimal regularization parameter.
     */
    [[nodiscard]] auto opt_param() const -> scalar_type {
        using std::pow;
        const auto log_reg_param = optimizer_.opt_variable();
        const auto reg_param = pow(static_cast<scalar_type>(10), log_reg_param);
        return reg_param;
    }

    /*!
     * \brief Get the value of the MLE objective function.
     *
     * \return Value of the MLE objective function.
     */
    [[nodiscard]] auto opt_value() const -> scalar_type {
        return optimizer_.opt_value();
    }

private:
    /*!
     * \brief Get the default region to search for the optimal regularization
     * parameter.
     *
     * \return Pair of minimum and maximum regularization parameters.
     */
    [[nodiscard]] auto param_search_region() const
        -> std::pair<scalar_type, scalar_type> {
        const scalar_type max_eigenvalue =
            solver_->eigenvalues()(solver_->eigenvalues().size() - 1);
        return {
            regularization::impl::coeff_max_param<scalar_type> * max_eigenvalue,
            regularization::impl::coeff_min_param<scalar_type> *
                max_eigenvalue};
    }

    //! Solver.
    const solver_type* solver_;

    //! Optimizer.
    opt::heuristic_global_optimizer<opt::function_object_wrapper<
        scalar_type(scalar_type), objective_function_type>>
        optimizer_;
};

}  // namespace num_collect::interp::kernel::impl
