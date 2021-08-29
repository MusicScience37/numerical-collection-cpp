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

#include "num_collect/opt/heuristic_1dim_optimizer.h"
#include "num_collect/regularization/param_searcher_base.h"

namespace num_collect::regularization {

/*!
 * \brief Class to calculate the curvature of L-curve.
 *
 * \warning This class is meant for use in optimizers.
 *
 * \tparam Solver Type of solvers.
 */
template <typename Solver>
class explicit_l_curve_curvature {
public:
    //! Type of solvers.
    using solver_type = Solver;

    //! Type of scalars.
    using scalar_type = typename solver_type::scalar_type;

    //! Type of variables.
    using variable_type = scalar_type;

    //! Type of function values.
    using value_type = scalar_type;

    /*!
     * \brief Construct.
     *
     * \param[in] solver Solver.
     */
    explicit explicit_l_curve_curvature(const solver_type& solver)
        : solver_(&solver) {}

    /*!
     * \brief Calculate the curvature of L-curve.
     *
     * \param[in] param Regularization parameter.
     * \return Curvature of L-curve.
     */
    [[nodiscard]] auto curvature(const scalar_type& param) const {
        return solver_->curvature(param);
    }

    /*!
     * \brief Calculate the curvature of L-curve.
     *
     * \param[in] log_param Logarithm of a regularization parameter.
     */
    void evaluate_on(const scalar_type& log_param) {
        using std::pow;
        const scalar_type param = pow(static_cast<scalar_type>(10),  // NOLINT
            log_param);
        curvature_ = curvature(param);
    }

    /*!
     * \brief Get the negated value of the curvature.
     *
     * \return Negated value of the curvature.
     */
    [[nodiscard]] auto value() const -> scalar_type { return -curvature_; }

private:
    //! Solver.
    const solver_type* solver_;

    //! Curvature.
    scalar_type curvature_{};
};

/*!
 * \brief Class to search optimal regularization parameter using l-curve.
 *
 * \tparam Solver Type of solvers.
 * \tparam Optimizer Type of optimizers.
 */
template <typename Solver,
    typename Optimizer =
        opt::heuristic_1dim_optimizer<explicit_l_curve_curvature<Solver>>>
class explicit_l_curve
    : public param_searcher_base<explicit_l_curve<Solver, Optimizer>, Solver> {
public:
    //! Type of base class.
    using base_type =
        param_searcher_base<explicit_l_curve<Solver, Optimizer>, Solver>;

    using typename base_type::scalar_type;
    using typename base_type::solver_type;

    //! Type of optimizers.
    using optimizer_type = Optimizer;

    /*!
     * \brief Construct.
     *
     * \param[in] solver Solver.
     */
    explicit explicit_l_curve(const solver_type& solver)
        : solver_(&solver),
          optimizer_(explicit_l_curve_curvature<Solver>(solver)) {}

    //! \copydoc param_searcher_base::search
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
    }

    //! \copydoc param_searcher_base::opt_param
    [[nodiscard]] auto opt_param() const -> scalar_type { return opt_param_; }

    //! \copydoc param_searcher_base::solve
    template <typename Solution>
    void solve(Solution& solution) const {
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
