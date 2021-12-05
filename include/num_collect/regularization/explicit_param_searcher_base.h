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
 * \brief Definition of explicit_param_searcher_base class.
 */
#pragma once

namespace num_collect::regularization {

/*!
 * \brief Base class for searching the optimal regularization parameters using
 * explicit formulas for regularization.
 *
 * \tparam Derived Type of the derived class.
 * \tparam Solver Type of solvers.
 */
template <typename Derived, typename Solver>
class explicit_param_searcher_base {
public:
    //! Type of solvers.
    using solver_type = Solver;

    //! Type of scalars.
    using scalar_type = typename solver_type::scalar_type;

    //! Type of data.
    using data_type = typename solver_type::data_type;

    /*!
     * \brief Search the optimal regularization parameter.
     */
    void search() { derived().search(); }

    /*!
     * \brief Get the optimal regularization parameter.
     *
     * \return Optimal regularization parameter.
     */
    [[nodiscard]] auto opt_param() const -> scalar_type {
        return derived().opt_param();
    }

    /*!
     * \brief Solver with the optimal regularization parameter.
     *
     * \tparam Solution Type of the solution.
     * \param[out] solution Solution.
     */
    void solve(data_type& solution) const { derived().solve(solution); }

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
};

}  // namespace num_collect::regularization
