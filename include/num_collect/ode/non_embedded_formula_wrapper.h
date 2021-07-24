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
 * \brief Definition of non_embedded_formula_wrapper class.
 */
#pragma once

#include "num_collect/constants/half.h"
#include "num_collect/constants/two.h"
#include "num_collect/ode/embedded_solver.h"
#include "num_collect/ode/formula_base.h"
#include "num_collect/util/index_type.h"

namespace num_collect::ode {

/*!
 * \brief Wrapper class to use a non-embedded formula as an embedded formula.
 *
 * \tparam BaseFormula Type of formula to use.
 */
template <typename BaseFormula>
class non_embedded_formula_wrapper {
public:
    //! Type of formula to use in this class.
    using base_formula_type = BaseFormula;

    //! Type of problem.
    using problem_type = typename base_formula_type::problem_type;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Number of stages of this formula.
    static constexpr index_type stages = base_formula_type::stages;

    //! Order of this formula.
    static constexpr index_type order = base_formula_type::order;

    //! Order of lesser coefficients of this formula.
    static constexpr index_type lesser_order = base_formula_type::order;

    /*!
     * \brief Construct.
     *
     * \param[in] problem Problem.
     */
    explicit non_embedded_formula_wrapper(
        const problem_type& problem = problem_type())
        : formula_(problem) {}

    /*!
     * \brief Compute the next variable.
     *
     * \param[in] time Current time.
     * \param[in] step_size Step size.
     * \param[in] current Current variable.
     * \param[out] estimate Estimate of the next variable.
     */
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        formula_.step(time, step_size, current, estimate);
    }

    /*!
     * \brief Compute the next variable and weak estimate of it with embedded
     * formula.
     *
     * \param[in] time Current time.
     * \param[in] step_size Step size.
     * \param[in] current Current variable.
     * \param[out] estimate Estimate of the next variable.
     * \param[out] error Estimate of error.
     */
    void step_embedded(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate,
        variable_type& error) {
        variable_type rough_estimate;
        step(time, step_size, current, rough_estimate);
        variable_type half_estimate;
        step(time, step_size, current, half_estimate);
        step(time, step_size, current, estimate);
        error = rough_estimate - estimate;
    }

    /*!
     * \brief Get the problem.
     *
     * \return Problem.
     */
    [[nodiscard]] auto problem() -> problem_type& { return formula_.problem_; }

    /*!
     * \brief Get the problem.
     *
     * \return Problem.
     */
    [[nodiscard]] auto problem() const -> const problem_type& {
        return formula_.problem_;
    }

private:
    //! Formula.
    base_formula_type formula_;
};

/*!
 * \brief Class of solver with automatic step size using non-embedded formula.
 *
 * \tparam Formula Type of formula.
 */
template <typename Formula>
using non_embedded_auto_solver =
    embedded_solver<non_embedded_formula_wrapper<Formula>>;

}  // namespace num_collect::ode
