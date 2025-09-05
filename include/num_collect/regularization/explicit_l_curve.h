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
 * \brief Definition of explicit_l_curve class.
 */
#pragma once

#include <cmath>

#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/function_object_wrapper.h"
#include "num_collect/opt/heuristic_global_optimizer.h"
#include "num_collect/regularization/concepts/explicit_regularized_solver.h"
#include "num_collect/regularization/explicit_param_searcher_base.h"

namespace num_collect::regularization {

//! Tag of explicit_l_curve.
constexpr auto explicit_l_curve_tag =
    logging::log_tag_view("num_collect::regularization::explicit_l_curve");

/*!
 * \brief Class of objective function in L-curve.
 *
 * \warning This class is meant for use in optimizers.
 *
 * \tparam Solver Type of solvers.
 */
template <concepts::explicit_regularized_solver Solver>
class explicit_l_curve_objective_function {
public:
    //! Type of solvers.
    using solver_type = Solver;

    //! Type of scalars.
    using scalar_type = typename solver_type::scalar_type;

    /*!
     * \brief Constructor.
     *
     * \param[in] solver Solver.
     */
    explicit explicit_l_curve_objective_function(const solver_type& solver)
        : solver_(&solver) {}

    /*!
     * \brief Calculate the curvature of L-curve.
     *
     * \param[in] log_param Logarithm of a regularization parameter.
     * \return Negated value of the curvature.
     */
    [[nodiscard]] auto operator()(const scalar_type& log_param) const
        -> scalar_type {
        using std::pow;
        const scalar_type param = pow(static_cast<scalar_type>(10),  // NOLINT
            log_param);
        return -solver_->l_curve_curvature(param);
    }

private:
    //! Solver.
    const solver_type* solver_;
};

/*!
 * \brief Class to search optimal regularization parameter using l-curve.
 *
 * \tparam Solver Type of solvers.
 * \tparam Optimizer Type of optimizers.
 */
template <concepts::explicit_regularized_solver Solver,
    template <typename> typename Optimizer = opt::heuristic_global_optimizer>
class explicit_l_curve
    : public explicit_param_searcher_base<explicit_l_curve<Solver, Optimizer>,
          Solver> {
public:
    //! Type of base class.
    using base_type =
        explicit_param_searcher_base<explicit_l_curve<Solver, Optimizer>,
            Solver>;

    using typename base_type::data_type;
    using typename base_type::scalar_type;
    using typename base_type::solver_type;

    //! Type of optimizers.
    using optimizer_type =
        Optimizer<opt::function_object_wrapper<scalar_type(scalar_type),
            explicit_l_curve_objective_function<solver_type>>>;

    /*!
     * \brief Constructor.
     *
     * \param[in] solver Solver.
     */
    explicit explicit_l_curve(const solver_type& solver)
        : base_type(explicit_l_curve_tag),
          solver_(&solver),
          optimizer_(
              opt::make_function_object_wrapper<scalar_type(scalar_type)>(
                  explicit_l_curve_objective_function<solver_type>(solver))) {}

    //! \copydoc explicit_param_searcher_base::search
    void search() {
        using std::log10;
        using std::pow;
        const auto [min_param, max_param] = solver_->param_search_region();
        const scalar_type log_min_param = log10(min_param);
        const scalar_type log_max_param = log10(max_param);
        optimizer_.init(log_min_param, log_max_param);
        optimizer_.solve();
        opt_param_ = pow(static_cast<scalar_type>(10),  // NOLINT
            optimizer_.opt_variable());
        NUM_COLLECT_LOG_SUMMARY(
            this->logger(), "Selected parameter: {}", opt_param_);
    }

    //! \copydoc explicit_param_searcher_base::opt_param
    [[nodiscard]] auto opt_param() const -> scalar_type { return opt_param_; }

    //! \copydoc explicit_param_searcher_base::solve
    void solve(data_type& solution) const {
        solver_->solve(opt_param_, solution);
    }

private:
    //! Solver.
    const solver_type* solver_;

    //! Optimizer.
    optimizer_type optimizer_;

    //! Optimal regularization parameter.
    scalar_type opt_param_{};
};

}  // namespace num_collect::regularization
