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
 * \brief Definition of avf_integrand class.
 */
#pragma once

#include "num_collect/constants/one.h"         // IWYU pragma: keep
#include "num_collect/ode/concepts/problem.h"  // IWYU pragma: keep

namespace num_collect::ode::avf::impl {

/*!
 * \brief Class of integrand for average vector field (AVF) method
 * \cite Quispel2008.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::problem Problem>
class avf_integrand {
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
    explicit avf_integrand(const problem_type& problem = problem_type())
        : problem_(problem) {}

    /*!
     * \brief Calculate integrand.
     *
     * \param[in] rate Rate of variable.
     * \return Value of integrand.
     */
    [[nodiscard]] auto operator()(scalar_type rate) const -> variable_type {
        problem_.evaluate_on(time_,
            (constants::one<scalar_type> - rate) * prev_var_ +
                rate * next_var_);
        return problem_.diff_coeff();
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

    /*!
     * \brief Set time.
     *
     * \param[in] val Value.
     */
    void time(scalar_type val) { time_ = val; }

    /*!
     * \brief Set the previous variable.
     *
     * \param[in] var Variable.
     */
    void prev_var(const variable_type& var) { prev_var_ = var; }

    /*!
     * \brief Set the next variable.
     *
     * \param[in] var Variable.
     */
    void next_var(const variable_type& var) { next_var_ = var; }

private:
    //! Problem
    mutable problem_type problem_;

    //! Time.
    scalar_type time_{};

    //! Previous variable.
    variable_type prev_var_{};

    //! Next variable.
    variable_type next_var_{};
};

}  // namespace num_collect::ode::avf::impl
