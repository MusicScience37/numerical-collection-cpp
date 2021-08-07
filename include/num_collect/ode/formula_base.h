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
 * \brief Definition of formula_base class.
 */
#pragma once

#include "num_collect/util/index_type.h"

namespace num_collect::ode {

/*!
 * \brief Class of formulas in Runge-Kutta method.
 *
 * \tparam Derived Type of derived class.
 * \tparam Problem Type of problem.
 */
template <typename Derived, typename Problem>
class formula_base {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    /*!
     * \brief Construct.
     *
     * \param[in] problem Problem.
     */
    explicit formula_base(const problem_type& problem = problem_type())
        : problem_(problem) {}

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
        derived().step(time, step_size, current, estimate);
    }

    /*!
     * \brief Get the problem.
     *
     * \return Problem.
     */
    [[nodiscard]] auto problem() -> problem_type& { return problem_; }

    /*!
     * \brief Get the problem.
     *
     * \return Problem.
     */
    [[nodiscard]] auto problem() const -> const problem_type& {
        return problem_;
    }

protected:
    /*!
     * \brief Access derived object.
     *
     * \return Reference to the derived object.
     */
    [[nodiscard]] auto derived() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }

    /*!
     * \brief Access derived object.
     *
     * \return Reference to the derived object.
     */
    [[nodiscard]] auto derived() const noexcept -> const Derived& {
        return *static_cast<const Derived*>(this);
    }

    /*!
     * \brief Convert coefficients.
     *
     * \tparam T Input type.
     * \param[in] val Input value.
     * \return Converted value.
     */
    template <typename T>
    static constexpr auto coeff(T val) -> scalar_type {
        return static_cast<scalar_type>(val);
    }

    /*!
     * \brief Create coefficients.
     *
     * \tparam T1 Input type.
     * \tparam T2 Input type.
     * \param[in] num Numerator.
     * \param[in] den Denominator.
     * \return Coefficient.
     */
    template <typename T1, typename T2>
    static constexpr auto coeff(T1 num, T2 den) -> scalar_type {
        return static_cast<scalar_type>(num) / static_cast<scalar_type>(den);
    }

private:
    //! Problem
    problem_type problem_;
};

}  // namespace num_collect::ode
