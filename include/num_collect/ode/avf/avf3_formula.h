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
 * \brief Definition of avf3_formula class.
 */
#pragma once

#include <string_view>

#include "num_collect/base/index_type.h"
#include "num_collect/base/norm.h"
#include "num_collect/constants/one.h"   // IWYU pragma: keep
#include "num_collect/constants/zero.h"  // IWYU pragma: keep
#include "num_collect/integration/gauss_legendre_integrator.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/avf/impl/avf_integrand.h"
#include "num_collect/ode/concepts/differentiable_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/non_embedded_formula_wrapper.h"
#include "num_collect/ode/simple_solver.h"
#include "num_collect/util/assert.h"

namespace num_collect::ode::avf {

/*!
 * \brief Class of 3rd order average vector field (AVF) method
 * \cite Quispel2008.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
class avf3_formula {
public:
    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    //! Number of stages of this formula.
    static constexpr index_type stages = 1;

    //! Order of this formula.
    static constexpr index_type order = 3;

    //! Log tag.
    static constexpr auto log_tag =
        logging::log_tag_view("num_collect::ode::avf::avf3_formula");

    /*!
     * \brief Constructor.
     *
     * \param[in] problem Problem.
     */
    explicit avf3_formula(const problem_type& problem = problem_type())
        : integrand_(problem) {}

    /*!
     * \brief Compute the next variable.
     *
     * \param[in] time Current time.
     * \param[in] step_size Step size.
     * \param[in] current Current variable.
     * \param[out] estimate Estimate of the next variable.
     */
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        const auto dim = current.size();

        problem().evaluate_on(time, current,
            evaluation_type{.diff_coeff = true, .jacobian = true});
        static constexpr scalar_type coeff_jacobi =
            -static_cast<scalar_type>(1) / static_cast<scalar_type>(12);
        const jacobian_type coeff = step_size *
            (jacobian_type::Identity(dim, dim) +
                coeff_jacobi * step_size * step_size * problem().jacobian() *
                    problem().jacobian());

        estimate = current + coeff * problem().diff_coeff();

        integrand_.time(time);
        integrand_.prev_var(current);
        variable_type prev_estimate;
        constexpr index_type max_loops = 10000;
        for (index_type i = 0; i < max_loops; ++i) {
            integrand_.next_var(estimate);
            prev_estimate = estimate;
            estimate = current +
                coeff *
                    integrator_(integrand_, constants::zero<scalar_type>,
                        constants::one<scalar_type>);
            if (norm(estimate - prev_estimate) < tol_residual_norm_) {
                return;
            }
        }
    }

    /*!
     * \brief Get the problem.
     *
     * \return Problem.
     */
    [[nodiscard]] auto problem() -> problem_type& {
        return integrand_.problem();
    }

    /*!
     * \brief Get the problem.
     *
     * \return Problem.
     */
    [[nodiscard]] auto problem() const -> const problem_type& {
        return integrand_.problem();
    }

    /*!
     * \brief Set tolerance of residual norm.
     *
     * \param[in] val Value.
     */
    void tol_residual_norm(scalar_type val) {
        NUM_COLLECT_ASSERT(val > constants::zero<scalar_type>);
        tol_residual_norm_ = val;
    }

private:
    //! Integrand.
    impl::avf_integrand<problem_type> integrand_;

    //! Order of integrator.
    static constexpr index_type integrator_order = 5;

    //! Integrator.
    integration::gauss_legendre_integrator<variable_type(scalar_type)>
        integrator_{integrator_order};

    //! Default tolerance of residual norm.
    static constexpr auto default_tol_residual_norm =
        static_cast<scalar_type>(1e-8);

    //! Tolerance of residual norm.
    scalar_type tol_residual_norm_{default_tol_residual_norm};
};

/*!
 * \brief Class of solver using 3rd order average vector field (AVF) method
 * \cite Quispel2008.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
using avf3_solver = simple_solver<avf3_formula<Problem>>;

/*!
 * \brief Class of solver using 3rd order average vector field (AVF) method
 * \cite Quispel2008 with automatic step sizes.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
using avf3_auto_solver = non_embedded_auto_solver<avf3_formula<Problem>>;

}  // namespace num_collect::ode::avf
