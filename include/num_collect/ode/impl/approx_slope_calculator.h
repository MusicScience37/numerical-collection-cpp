/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of approx_slope_calculator class.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include "num_collect/base/index_type.h"
#include "num_collect/ode/concepts/mass_problem.h"
#include "num_collect/ode/concepts/multi_variate_problem.h"
#include "num_collect/ode/concepts/problem.h"
#include "num_collect/ode/concepts/single_variate_problem.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/impl/gmres.h"

namespace num_collect::ode::impl {

/*!
 * \brief Class to approximate a slope.
 *
 * \tparam Problem Type of the problem.
 *
 * This class approximates a slope even when the problem have a singular mass
 * matrix. For this purpose, this class always use approximation of the slope.
 */
template <concepts::problem Problem>
class approx_slope_calculator;

/*!
 * \brief Class to approximate a slope.
 *
 * \tparam Problem Type of the problem.
 *
 * This class approximates a slope even when the problem have a singular mass
 * matrix. For this purpose, this class always use approximation of the slope.
 */
template <concepts::problem Problem>
    requires(!concepts::mass_problem<Problem>)
class approx_slope_calculator<Problem> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    /*!
     * \brief Constructor.
     */
    approx_slope_calculator() = default;

    /*!
     * \brief Approximate a slope.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] variable Variable.
     * \param[out] slope Approximation of slope.
     */
    void operator()(problem_type& problem, scalar_type time,
        const variable_type& variable, variable_type& slope) const {
        problem.evaluate_on(
            time, variable, evaluation_type{.diff_coeff = true});
        slope = problem.diff_coeff();
    }
};

/*!
 * \brief Class to approximate a slope.
 *
 * \tparam Problem Type of the problem.
 *
 * This class approximates a slope even when the problem have a singular mass
 * matrix. For this purpose, this class always use approximation of the slope.
 */
template <concepts::single_variate_problem Problem>
    requires concepts::mass_problem<Problem>
class approx_slope_calculator<Problem> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    /*!
     * \brief Constructor.
     */
    approx_slope_calculator() = default;

    /*!
     * \brief Approximate a slope.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] variable Variable.
     * \param[out] slope Approximation of slope.
     */
    void operator()(problem_type& problem, scalar_type time,
        const variable_type& variable, variable_type& slope) const {
        problem.evaluate_on(
            time, variable, evaluation_type{.diff_coeff = true, .mass = true});

        using std::abs;
        constexpr scalar_type threshold =
            std::numeric_limits<scalar_type>::epsilon();
        if (abs(problem.mass()) < threshold) {
            slope = static_cast<scalar_type>(0);
            return;
        }

        slope = problem.diff_coeff() / problem.mass();
    }
};

/*!
 * \brief Class to approximate a slope.
 *
 * \tparam Problem Type of the problem.
 *
 * This class approximates a slope even when the problem have a singular mass
 * matrix. For this purpose, this class always use approximation of the slope.
 */
template <concepts::multi_variate_problem Problem>
    requires concepts::mass_problem<Problem>
class approx_slope_calculator<Problem> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    /*!
     * \brief Constructor.
     */
    approx_slope_calculator() = default;

    /*!
     * \brief Approximate a slope.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] variable Variable.
     * \param[out] slope Approximation of slope.
     */
    void operator()(problem_type& problem, scalar_type time,
        const variable_type& variable, variable_type& slope) {
        problem.evaluate_on(
            time, variable, evaluation_type{.diff_coeff = true, .mass = true});

        constexpr index_type max_subspace_dim = 100;
        solver_.max_subspace_dim(std::min(variable.size(), max_subspace_dim));
        const auto coeff_function = [&problem](const variable_type& target,
                                        variable_type& result) {
            result = problem.mass() * target;
        };
        slope = variable_type::Zero(variable.size());
        solver_.solve(coeff_function, problem.diff_coeff(), slope);
    }

private:
    //! GMRES solver.
    gmres<variable_type> solver_;
};

}  // namespace num_collect::ode::impl
