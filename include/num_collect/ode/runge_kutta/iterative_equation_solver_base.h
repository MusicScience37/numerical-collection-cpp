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
 * \brief Definition of iterative_equation_solver_base class.
 */
#pragma once

#include "num_collect/base/iterative_solver_base.h"
#include "num_collect/logging/log_tag_view.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Base class of iterative solvers for equations of implicit formulas.
 *
 * \tparam Derived Type of derived class.
 */
template <typename Derived>
class iterative_equation_solver_base : public iterative_solver_base<Derived> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] tag Log tag.
     */
    explicit iterative_equation_solver_base(logging::log_tag_view tag)
        : iterative_solver_base<Derived>(tag) {}

    /*!
     * \brief Determine if the algorithm converged.
     *
     * \return If the algorithm converged.
     */
    [[nodiscard]] auto is_converged() const -> bool {
        return derived().is_converged();
    }

    /*!
     * \brief Solve the problem.
     *
     * Iterate the algorithm until the stopping criteria are satisfied with
     * logging.
     *
     * \warning Any required initializations (with `init` functions) are assumed
     * to have been done.
     */
    void solve() {
        iterative_solver_base<Derived>::solve();
        if (!is_converged()) {
            NUM_COLLECT_ODE_THROW_NO_CONVERGENCE(this->logger(),
                "No convergence in the equation solver in the implicit "
                "Runge-Kutta method.");
        }
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

}  // namespace num_collect::ode::runge_kutta
