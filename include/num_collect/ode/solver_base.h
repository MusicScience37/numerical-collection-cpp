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
 * \brief Definition of solver_base class.
 */
#pragma once

#include "num_collect/base/index_type.h"
#include "num_collect/logging/iterations/iteration_logger_mixin.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/ode/concepts/formula.h"

namespace num_collect::ode {

/*!
 * \brief Base class of solvers of ODEs.
 *
 * \tparam Derived Type of derived class.
 * \tparam Formula Type of formula.
 */
template <typename Derived, concepts::formula Formula>
class solver_base
    : public logging::logging_mixin,
      public logging::iterations::iteration_logger_mixin<Derived> {
public:
    //! Type of formula.
    using formula_type = Formula;

    //! Type of problem.
    using problem_type = typename formula_type::problem_type;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Number of stages of the formula.
    static constexpr index_type stages = formula_type::stages;

    //! Order of the formula.
    static constexpr index_type order = formula_type::order;

    /*!
     * \brief Constructor.
     *
     * \param[in] problem Problem.
     */
    explicit solver_base(const problem_type& problem)
        : logging::logging_mixin(formula_type::log_tag), formula_(problem) {
        this->logger().set_iterative();
        this->configure_child_algorithm_logger_if_exists(formula_);
    }

    /*!
     * \brief Initialize.
     *
     * \param[in] time Initial time.
     * \param[in] variable Initial variable.
     */
    void init(scalar_type time, const variable_type& variable) {
        derived().init(time, variable);
    }

    /*!
     * \brief Compute the variable of the next step.
     *
     * \warning `init` function is assumed to have been called before call to
     * `step` function.
     */
    void step() { derived().step(); }

    /*!
     * \brief Compute the variable at the given time.
     *
     * \warning `init` function is assumed to have been called before call to
     * `solve_till` function.
     *
     * \param[in] end_time Time to compute the variable at.
     */
    void solve_till(scalar_type end_time) {
        auto& iter_logger = this->initialize_iteration_logger();
        iter_logger.write_iteration(&derived());
        while (time() < end_time) {
            const scalar_type max_step_size = end_time - time();
            const scalar_type current_step_size = step_size();
            const bool rewrite_step_size = current_step_size > max_step_size;
            if (rewrite_step_size) {
                step_size(max_step_size);
            }
            step();
            if (rewrite_step_size && (step_size() >= max_step_size)) {
                step_size(current_step_size);
            }
            iter_logger.write_iteration(&derived());
        }
        iter_logger.write_summary(&derived());
    }

    /*!
     * \brief Get the formula.
     *
     * \return Formula.
     */
    [[nodiscard]] auto formula() -> formula_type& { return formula_; }

    /*!
     * \brief Get the formula.
     *
     * \return Formula.
     */
    [[nodiscard]] auto formula() const -> const formula_type& {
        return formula_;
    }

    /*!
     * \brief Get the problem.
     *
     * \return Problem.
     */
    [[nodiscard]] auto problem() -> problem_type& { return formula_.problem(); }

    /*!
     * \brief Get the problem.
     *
     * \return Problem.
     */
    [[nodiscard]] auto problem() const -> const problem_type& {
        return formula_.problem();
    }

    /*!
     * \brief Get the current time.
     *
     * \return Current time.
     */
    [[nodiscard]] auto time() const -> scalar_type { return derived().time(); }

    /*!
     * \brief Get the current variable.
     *
     * \return Current variable.
     */
    [[nodiscard]] auto variable() const -> const variable_type& {
        return derived().variable();
    }

    /*!
     * \brief Get the step size.
     *
     * \return Step size.
     */
    [[nodiscard]] auto step_size() const -> scalar_type {
        return derived().step_size();
    }

    /*!
     * \brief Get the number of steps.
     *
     * \return Number of steps.
     */
    [[nodiscard]] auto steps() const -> index_type { return derived().steps(); }

    /*!
     * \brief Set the step size.
     *
     * \param[in] val Value.
     * \return This object.
     */
    auto step_size(scalar_type val) -> Derived& {
        return derived().step_size(val);
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
    //! Formula.
    formula_type formula_;
};

}  // namespace num_collect::ode
