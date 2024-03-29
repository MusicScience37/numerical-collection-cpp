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
 * \brief Definition of iterative_solver_base class.
 */
#pragma once

#include "num_collect/logging/iterations/iteration_logger_mixin.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"

namespace num_collect {
inline namespace base {

/*!
 * \brief Base class of iterative solvers.
 *
 * \tparam Derived Type of derived class.
 */
template <typename Derived>
class iterative_solver_base
    : public logging::logging_mixin,
      public logging::iterations::iteration_logger_mixin<Derived> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] tag Log tag.
     */
    explicit iterative_solver_base(logging::log_tag_view tag)
        : logging::logging_mixin(tag) {
        this->logger().set_iterative();
    }

    /*!
     * \brief Iterate the algorithm once.
     *
     * \warning Any required initializations (with `init` functions) are assumed
     * to have been done.
     */
    void iterate() { derived().iterate(); }

    /*!
     * \brief Determine if stopping criteria of the algorithm are satisfied.
     *
     * \return If stopping criteria of the algorithm are satisfied.
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return derived().is_stop_criteria_satisfied();
    }

    /*!
     * \brief Solve the problem.
     *
     * Iterate the algorithm until the stopping criteria are satisfied.
     *
     * \warning Any required initializations (with `init` functions) are assumed
     * to have been done.
     */
    void solve() {
        auto& iter_logger = this->initialize_iteration_logger();
        iter_logger.write_iteration(&derived());
        while (!is_stop_criteria_satisfied()) {
            iterate();
            iter_logger.write_iteration(&derived());
        }
        iter_logger.write_summary(&derived());
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
};

}  // namespace base
}  // namespace num_collect
