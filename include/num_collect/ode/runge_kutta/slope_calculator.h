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
 * \brief Definition of slope_calculator class.
 */
#pragma once

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/LU>

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/exception.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/ode/concepts/mass_problem.h"
#include "num_collect/ode/concepts/multi_variate_problem.h"
#include "num_collect/ode/concepts/problem.h"
#include "num_collect/ode/concepts/single_variate_problem.h"
#include "num_collect/ode/evaluation_type.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class to calculate a slope in Runge-Kutta formulas.
 *
 * \tparam Problem Type of the problem.
 *
 * This class calculates a slope with handling of mass matrices if exists.
 */
template <concepts::problem Problem>
class slope_calculator;

/*!
 * \brief Class to calculate a slope in Runge-Kutta formulas.
 *
 * \tparam Problem Type of the problem.
 *
 * This class calculates a slope with handling of mass matrices if exists.
 */
template <concepts::single_variate_problem Problem>
class slope_calculator<Problem> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    /*!
     * \brief Constructor.
     */
    slope_calculator() = default;

    /*!
     * \brief Calculate a slope.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] variable Variable.
     * \param[out] slope Slope.
     */
    void operator()(problem_type& problem, scalar_type time,
        const variable_type& variable, variable_type& slope) const {
        problem.evaluate_on(time, variable,
            evaluation_type{.diff_coeff = true, .mass = use_mass});
        if constexpr (use_mass) {
            slope = problem.diff_coeff() / problem.mass();
        } else {
            slope = problem.diff_coeff();
        }
    }
};

/*!
 * \brief Class to calculate a slope in Runge-Kutta formulas.
 *
 * \tparam Problem Type of the problem.
 *
 * This class calculates a slope with handling of mass matrices if exists.
 */
template <concepts::multi_variate_problem Problem>
    requires(!concepts::mass_problem<Problem>)
class slope_calculator<Problem> {
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
    slope_calculator() = default;

    /*!
     * \brief Calculate a slope.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] variable Variable.
     * \param[out] slope Slope.
     */
    void operator()(problem_type& problem, scalar_type time,
        const variable_type& variable, variable_type& slope) const {
        problem.evaluate_on(
            time, variable, evaluation_type{.diff_coeff = true});
        slope = problem.diff_coeff();
    }
};

/*!
 * \brief Class to calculate a slope in Runge-Kutta formulas.
 *
 * \tparam Problem Type of the problem.
 *
 * This class calculates a slope with handling of mass matrices if exists.
 */
template <concepts::multi_variate_problem Problem>
    requires concepts::mass_problem<Problem> &&
    base::concepts::dense_matrix<typename Problem::mass_type>
class slope_calculator<Problem> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of mass matrix.
    using mass_type = typename problem_type::mass_type;

    /*!
     * \brief Constructor.
     */
    slope_calculator() = default;

    /*!
     * \brief Calculate a slope.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] variable Variable.
     * \param[out] slope Slope.
     */
    void operator()(problem_type& problem, scalar_type time,
        const variable_type& variable, variable_type& slope) {
        problem.evaluate_on(
            time, variable, evaluation_type{.diff_coeff = true, .mass = true});
        solver_.compute(problem.mass());
        slope = solver_.solve(problem.diff_coeff());
        if (!slope.allFinite()) {
            NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
                "Failed to solver an equation for calculating slope.");
        }
    }

private:
    //! Solver of the matrix.
    Eigen::PartialPivLU<mass_type> solver_;
};

/*!
 * \brief Class to calculate a slope in Runge-Kutta formulas.
 *
 * \tparam Problem Type of the problem.
 *
 * This class calculates a slope with handling of mass matrices if exists.
 */
template <concepts::multi_variate_problem Problem>
    requires concepts::mass_problem<Problem> &&
    base::concepts::sparse_matrix<typename Problem::mass_type>
class slope_calculator<Problem> {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of mass matrix.
    using mass_type = typename problem_type::mass_type;

    /*!
     * \brief Constructor.
     */
    slope_calculator() = default;

    /*!
     * \brief Calculate a slope.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] variable Variable.
     * \param[out] slope Slope.
     */
    void operator()(problem_type& problem, scalar_type time,
        const variable_type& variable, variable_type& slope) {
        problem.evaluate_on(
            time, variable, evaluation_type{.diff_coeff = true, .mass = true});
        solver_.compute(problem.mass());
        slope = solver_.solve(problem.diff_coeff());
        if (!slope.allFinite()) {
            NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
                "Failed to solver an equation for calculating slope.");
        }
    }

private:
    //! Solver of the matrix.
    Eigen::BiCGSTAB<mass_type> solver_;
};

}  // namespace num_collect::ode::runge_kutta
