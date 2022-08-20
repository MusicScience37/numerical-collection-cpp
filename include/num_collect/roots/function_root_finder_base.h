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

#include "num_collect/base/iterative_solver_base.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/roots/concepts/function.h"  // IWYU pragma: keep

namespace num_collect::roots {

/*!
 * \brief Base class of root-finding algorithms for functions.
 *
 * \tparam Derived Type of derived class.
 * \tparam Function Type of the function of equation.
 */
template <typename Derived, concepts::function Function>
class function_root_finder_base
    : public num_collect::base::iterative_solver_base<Derived> {
public:
    //! Type of function of equation.
    using function_type = Function;

    //! Type of variables.
    using variable_type = typename function_type::variable_type;

    /*!
     * \brief Constructor.
     *
     * \param[in] tag Log tag.
     * \param[in] function Function of equation.
     */
    explicit function_root_finder_base(logging::log_tag_view tag,
        const function_type& function = function_type())
        : num_collect::base::iterative_solver_base<Derived>(tag),
          function_(function) {}

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

private:
    //! Function of equation.
    Function function_;
};

}  // namespace num_collect::roots
