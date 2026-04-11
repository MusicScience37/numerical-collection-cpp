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
 * \brief Definition of radau2a3_formula class.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/get_size.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/differentiable_problem.h"
#include "num_collect/ode/formula_base.h"
#include "num_collect/ode/non_embedded_formula_wrapper.h"
#include "num_collect/ode/runge_kutta/inexact_newton_decomposed_full_update_equation_solver.h"
#include "num_collect/ode/simple_solver.h"

namespace num_collect::ode::runge_kutta {

namespace impl {

/*!
 * \brief Coefficients in Radau IIA method of order 3 \cite Hairer1991.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
struct radau2a3_coefficients {
    //! Type of scalars.
    using scalar_type = Scalar;

    //! Number of stages.
    static constexpr index_type stages = 2;

    /*!
     * \brief Get the coefficients of intermidiate slopes in the formula.
     *
     * \return Coefficients.
     */
    static auto slope_coeffs() -> Eigen::Matrix<scalar_type, 2, 2> {
        Eigen::Matrix<scalar_type, 2, 2> coeffs{};
        coeffs(0, 0) =
            static_cast<scalar_type>(5) / static_cast<scalar_type>(12);
        coeffs(0, 1) =
            static_cast<scalar_type>(-1) / static_cast<scalar_type>(12);
        coeffs(1, 0) =
            static_cast<scalar_type>(3) / static_cast<scalar_type>(4);
        coeffs(1, 1) =
            static_cast<scalar_type>(1) / static_cast<scalar_type>(4);
        return coeffs;
    }

    /*!
     * \brief Get the coefficients of time in the formula.
     *
     * \return Coefficients.
     */
    static auto time_coeffs() -> Eigen::Vector2<scalar_type> {
        Eigen::Vector2<scalar_type> coeffs{};
        coeffs(0) = static_cast<scalar_type>(1) / static_cast<scalar_type>(3);
        coeffs(1) = static_cast<scalar_type>(1);
        return coeffs;
    }

    /*!
     * \brief Get the coefficients of intermidiate updates in the formula.
     *
     * \return Coefficients.
     */
    static auto update_coeffs() -> Eigen::Vector2<scalar_type> {
        Eigen::Vector2<scalar_type> coeffs{};
        coeffs(0) = static_cast<scalar_type>(3) / static_cast<scalar_type>(4);
        coeffs(1) = static_cast<scalar_type>(1) / static_cast<scalar_type>(4);
        return coeffs;
    }

    /*!
     * \brief Get the data for
     * inexact_newton_decomposed_full_update_equation_solver class.
     *
     * \return Data.
     */
    static auto formula_solver_data()
        -> inexact_newton_decomposed_full_update_equation_solver_data<
            scalar_type, stages> {
        static const inexact_newton_decomposed_full_update_equation_solver_data<
            scalar_type, stages>
            data{slope_coeffs(), time_coeffs(), update_coeffs()};
        return data;
    };
};

}  // namespace impl

/*!
 * \brief Class of Radau IIA method of order 3 \cite Hairer1991.
 *
 * \tparam Problem Type of problem.
 *
 * \note Current implementation does not support sparse Jacobians.
 * This limitation may be removed in the future.
 * \note This formula does not support changing mass matrix.
 */
template <concepts::differentiable_problem Problem>
class radau2a3_formula
    : public formula_base<radau2a3_formula<Problem>, Problem> {
public:
    //! Type of base class.
    using base_type = formula_base<radau2a3_formula<Problem>, Problem>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    using base_type::base_type;
    using base_type::problem;

protected:
    using base_type::coeff;

public:
    //! Number of stages of this formula.
    static constexpr index_type stages = 2;

    //! Order of this formula.
    static constexpr index_type order = 3;

    //! Log tag.
    static constexpr auto log_tag = logging::log_tag_view(
        "num_collect::ode::runge_kutta::radau2a3_formula");

    //! Type of the solver of the implicit formula.
    using formula_solver_type =
        inexact_newton_decomposed_full_update_equation_solver<Problem, stages>;

    /*!
     * \brief Get the coefficients of intermidiate slopes in the formula.
     *
     * \return Coefficients.
     */
    static auto slope_coeffs() -> Eigen::Matrix<scalar_type, 2, 2> {
        return impl::radau2a3_coefficients<scalar_type>::slope_coeffs();
    }

    /*!
     * \brief Get the coefficients of time in the formula.
     *
     * \return Coefficients.
     */
    static auto time_coeffs() -> Eigen::Vector2<scalar_type> {
        return impl::radau2a3_coefficients<scalar_type>::time_coeffs();
    }

    /*!
     * \brief Get the coefficients of intermidiate updates in the formula.
     *
     * \return Coefficients.
     */
    static auto update_coeffs() -> Eigen::Vector2<scalar_type> {
        return impl::radau2a3_coefficients<scalar_type>::update_coeffs();
    }

    //! \copydoc ode::formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        updates_.resize(get_size(current) * stages);
        updates_.setZero();
        formula_solver_.init(problem(), time, step_size, current, updates_);
        formula_solver_.solve();
        if constexpr (base::concepts::dense_vector<variable_type>) {
            estimate = current + updates_.tail(get_size(current));
        } else {
            estimate = current + updates_(updates_.size() - 1);
        }
    }

    /*!
     * \brief Get solver of formula.
     *
     * \return Solver of formula.
     */
    [[nodiscard]] auto formula_solver() -> formula_solver_type& {
        return formula_solver_;
    }

    /*!
     * \brief Get solver of formula.
     *
     * \return Solver of formula.
     */
    [[nodiscard]] auto formula_solver() const -> const formula_solver_type& {
        return formula_solver_;
    }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val)
        -> radau2a3_formula& {
        formula_solver_.tolerances(val);
        return *this;
    }

    /*!
     * \brief Access to the logger.
     *
     * \return Logger.
     */
    [[nodiscard]] auto logger() const noexcept
        -> const num_collect::logging::logger& {
        return formula_solver_.logger();
    }

    /*!
     * \brief Access to the logger.
     *
     * \return Logger.
     */
    [[nodiscard]] auto logger() noexcept -> num_collect::logging::logger& {
        return formula_solver_.logger();
    }

private:
    //! Type of the vector of intermidiate updates.
    using update_vector_type = typename formula_solver_type::update_vector_type;

    //! Solver of the implicit formula.
    formula_solver_type formula_solver_{
        impl::radau2a3_coefficients<scalar_type>::formula_solver_data()};

    //! Intermidiate updates.
    update_vector_type updates_;
};

/*!
 * \brief Class of solver using Radau IIA method of order 3.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
using radau2a3_solver = simple_solver<radau2a3_formula<Problem>>;

/*!
 * \brief Class of solver using Radau IIA method of order 3 with automatic step
 * sizes.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
using radau2a3_auto_solver =
    non_embedded_auto_solver<radau2a3_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
