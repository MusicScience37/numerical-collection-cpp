/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of classes to wrap implicit problems to use as explicit
 * problems.
 */
#pragma once

#include <utility>

#include <Eigen/LU>

#include "num_collect/ode/concepts/mass_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/multi_variate_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/single_variate_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/evaluation_type.h"

namespace num_collect::ode {

/*!
 * \brief Class to wrap implicit problems to use as explicit problems.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::single_variate_problem Problem>
    requires concepts::mass_problem<Problem>
class single_variate_implicit_problem_wrapper {
public:
    //! Type of variables.
    using variable_type = typename Problem::variable_type;

    //! Type of scalars.
    using scalar_type = typename Problem::scalar_type;

    //! Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{.diff_coeff = true};

    /*!
     * \brief Constructor.
     *
     * \param[in] problem Problem.
     */
    explicit single_variate_implicit_problem_wrapper(Problem problem)
        : problem_(std::move(problem)) {}

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] time Time.
     * \param[in] variable Variable.
     */
    void evaluate_on(const scalar_type& time, const variable_type& variable,
        evaluation_type /*evaluations*/) {
        problem_.evaluate_on(
            time, variable, evaluation_type{.diff_coeff = true, .mass = true});
        diff_coeff_ = problem_.diff_coeff() / problem_.mass();
    }

    /*!
     * \brief Get the differential coefficient.
     *
     * \return Differential coefficient.
     */
    [[nodiscard]] auto diff_coeff() const noexcept -> const variable_type& {
        return diff_coeff_;
    }

private:
    //! Problem.
    Problem problem_;

    //! Differential coefficient.
    variable_type diff_coeff_{};
};

/*!
 * \brief Class to wrap implicit problems to use as explicit problems.
 *
 * \tparam Problem Type of the problem.
 * \tparam LinearEquationSolver Type of the solver of linear equations.
 */
template <concepts::multi_variate_problem Problem,
    // TODO: Concept of solvers in Eigen.
    typename LinearEquationSolver =
        Eigen::PartialPivLU<typename Problem::mass_type>>
    requires concepts::mass_problem<Problem>
class multi_variate_implicit_problem_wrapper {
public:
    //! Type of variables.
    using variable_type = typename Problem::variable_type;

    //! Type of scalars.
    using scalar_type = typename Problem::scalar_type;

    //! Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{.diff_coeff = true};

    /*!
     * \brief Constructor.
     *
     * \param[in] problem Problem.
     */
    explicit multi_variate_implicit_problem_wrapper(Problem problem)
        : problem_(std::move(problem)) {}

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] time Time.
     * \param[in] variable Variable.
     */
    void evaluate_on(const scalar_type& time, const variable_type& variable,
        evaluation_type /*evaluations*/) {
        problem_.evaluate_on(
            time, variable, evaluation_type{.diff_coeff = true, .mass = true});
        solver_.compute(problem_.mass());
        diff_coeff_ = solver_.solve(problem_.diff_coeff());
    }

    /*!
     * \brief Get the differential coefficient.
     *
     * \return Differential coefficient.
     */
    [[nodiscard]] auto diff_coeff() const noexcept -> const variable_type& {
        return diff_coeff_;
    }

private:
    //! Problem.
    Problem problem_;

    //! Differential coefficient.
    variable_type diff_coeff_{};

    //! Solver of linear equations.
    LinearEquationSolver solver_{};
};

/*!
 * \brief Wrap an implicit problem to use as an explicit problem.
 *
 * \tparam Problem Type of the problem.
 * \param[in] problem Problem.
 * \return Wrapped problem.
 */
template <concepts::single_variate_problem Problem>
    requires concepts::mass_problem<Problem>
[[nodiscard]] auto wrap_implicit_problem(Problem problem)
    -> single_variate_implicit_problem_wrapper<Problem> {
    return single_variate_implicit_problem_wrapper<Problem>(problem);
}

#ifndef NUM_COLLECT_DOCUMENTATION
// This function causes errors in Doxygen.

/*!
 * \brief Wrap an implicit problem to use as an explicit problem.
 *
 * \tparam Problem Type of the problem.
 * \param[in] problem Problem.
 * \return Wrapped problem.
 */
template <concepts::multi_variate_problem Problem>
    requires concepts::mass_problem<Problem>
[[nodiscard]] auto wrap_implicit_problem(Problem problem)
    -> multi_variate_implicit_problem_wrapper<Problem> {
    return multi_variate_implicit_problem_wrapper<Problem>(problem);
}
#endif

}  // namespace num_collect::ode
