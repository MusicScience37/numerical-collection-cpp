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
 * \brief Definition of simple_solver class.
 */
#pragma once

#include "num_collect/constants/zero.h"
#include "num_collect/ode/runge_kutta/solver_base.h"
#include "num_collect/util/assert.h"

namespace num_collect::ode::runge_kutta {

/*!
 * \brief Class of simple solver using Runge-Kutta method.
 *
 * \tparam Formula Type of formula.
 */
template <typename Formula>
class simple_solver : public solver_base<simple_solver<Formula>, Formula> {
public:
    //! This type.
    using this_type = simple_solver<Formula>;

    //! Type of base class.
    using base_type = solver_base<simple_solver<Formula>, Formula>;

    using typename base_type::formula_type;
    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    using base_type::base_type;
    using base_type::formula;
    using base_type::problem;

    //! \copydoc runge_kutta::solver_base::init
    void init(scalar_type time, const variable_type& variable) {
        time_ = time;
        variable_ = variable;
        steps_ = 0;
    }

    //! \copydoc runge_kutta::solver_base::step
    void step() {
        formula().step(time_, step_size_, variable_, variable_);
        time_ += step_size_;
        ++steps_;
    }

    //! \copydoc runge_kutta::solver_base::set_info_to
    void set_info_to(iteration_logger& logger) const {
        logger["Steps"] = steps();
        logger["Time"] = time();
        logger["StepSize"] = step_size();
    }

    //! \copydoc runge_kutta::solver_base::time
    [[nodiscard]] auto time() const -> scalar_type { return time_; }

    //! \copydoc runge_kutta::solver_base::variable
    [[nodiscard]] auto variable() const -> const variable_type& {
        return variable_;
    }

    //! \copydoc runge_kutta::solver_base::step_size()
    [[nodiscard]] auto step_size() const -> scalar_type { return step_size_; }

    //! \copydoc runge_kutta::solver_base::steps
    [[nodiscard]] auto steps() const -> index_type { return steps_; }

    //! \copydoc runge_kutta::solver_base::step_size(scalar_type)
    auto step_size(scalar_type val) -> this_type& {
        NUM_COLLECT_ASSERT(val > constants::zero<scalar_type>);
        step_size_ = val;
        return *this;
    }

private:
    //! Variable.
    variable_type variable_{};

    //! Default step size.
    static constexpr auto default_step_size = static_cast<scalar_type>(1e-4);

    //! Step size.
    scalar_type step_size_{default_step_size};

    //! Time.
    scalar_type time_{};

    //! Number of steps.
    index_type steps_{};
};

}  // namespace num_collect::ode::runge_kutta
