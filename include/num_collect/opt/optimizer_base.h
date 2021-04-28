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

#include "num_collect/util/index_type.h"
#include "num_collect/util/iteration_logger.h"

namespace num_collect::opt {

/*!
 * \brief Base class of implementations of optimization algorithms.
 *
 * \tparam Derived Type of derived class.
 */
template <typename Derived>
class optimizer_base {
public:
    /*!
     * \brief Iterate the algorithm once.
     *
     * \warning `init` function is assumed to have been called before call to
     * `iterate` function.
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
     * \warning `init` function is assumed to have been called before call to
     * `solve` function.
     */
    void solve() {
        while (!is_stop_criteria_satisfied()) {
            iterate();
        }
    }

    /*!
     * \brief Set information of the last iteration to logger.
     *
     * \param[in] logger Iteration logger.
     */
    void set_info_to(iteration_logger& logger) const {
        derived().set_info_to(logger);
    }

    /*!
     * \brief Solve the problem.
     *
     * Iterate the algorithm until the stopping criteria are satisfied.
     *
     * \warning `init` function is assumed to have been called before call to
     * `solve` function.
     *
     * \param[in] logging_stream Stream to write logs.
     */
    void solve(std::ostream& logging_stream) {
        iteration_logger logger;
        set_info_to(logger);
        logger.write_to(logging_stream);
        while (!is_stop_criteria_satisfied()) {
            iterate();
            set_info_to(logger);
            logger.write_to(logging_stream);
        }
    }

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

}  // namespace num_collect::opt
