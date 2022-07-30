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
 * \brief Definition of optimizer_base class.
 */
#pragma once

#include "num_collect/base/index_type.h"
#include "num_collect/base/iterative_solver_base.h"
#include "num_collect/logging/log_tag_view.h"

namespace num_collect::opt {

/*!
 * \brief Base class of implementations of optimization algorithms.
 *
 * \tparam Derived Type of derived class.
 */
template <typename Derived>
class optimizer_base
    : public num_collect::base::iterative_solver_base<Derived> {
protected:
    using num_collect::base::iterative_solver_base<Derived>::derived;

public:
    /*!
     * \brief Construct.
     *
     * \param[in] tag Log tag.
     */
    explicit optimizer_base(logging::log_tag_view tag)
        : num_collect::base::iterative_solver_base<Derived>(tag) {}

    /*!
     * \brief Get current optimal variable.
     *
     * \return Current optimal variable.
     */
    [[nodiscard]] auto opt_variable() const { return derived().opt_variable(); }

    /*!
     * \brief Get current optimal value.
     *
     * \return Current optimal value.
     */
    [[nodiscard]] auto opt_value() const { return derived().opt_value(); }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return derived().iterations();
    }

    /*!
     * \brief Get the number of function evaluations.
     *
     * \return Number of function evaluations.
     */
    [[nodiscard]] auto evaluations() const noexcept -> index_type {
        return derived().evaluations();
    }
};

}  // namespace num_collect::opt
