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
 * \brief Definition of function_root_finder_base class.
 */
#pragma once

#include <type_traits>

#include "num_collect/util/index_type.h"
#include "num_collect/util/iterative_solver_base.h"

namespace num_collect::roots {

/*!
 * \brief Base class of root-finding algorithms for functions.
 *
 * \tparam Derived Type of derived class.
 * \tparam Function Type of the function of equation.
 */
template <typename Derived, typename Function>
class function_root_finder_base : public iterative_solver_base<Derived> {
public:
    //! Type of function of equation.
    using function_type = Function;

    //! Type of variables.
    using variable_type = typename function_type::variable_type;

    /*!
     * \brief Construct.
     *
     * \param[in] function Function of equation.
     */
    explicit function_root_finder_base(
        const function_type& function = function_type())
        : function_(function) {}

    /*!
     * \brief Iterate the algorithm once.
     *
     * \warning `init` function is assumed to have been called before call to
     * `iterate` function.
     */
    void iterate() { derived().iterate(); }

    /*!
     * \brief Get the function of equation.
     *
     * \return Function of equation.
     */
    [[nodiscard]] auto function() -> function_type& { return function_; }

    /*!
     * \brief Get the function of equation.
     *
     * \return Function of equation.
     */
    [[nodiscard]] auto function() const -> const function_type& {
        return function_;
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

private:
    //! Function of equation.
    Function function_;
};

}  // namespace num_collect::roots
