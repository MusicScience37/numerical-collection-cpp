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
 * \brief Definition of lobatto3c6_formula class.
 */
#pragma once

#include <cmath>

#include <Eigen/Core>

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/get_size.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/differentiable_problem.h"
#include "num_collect/ode/non_embedded_formula_wrapper.h"
#include "num_collect/ode/runge_kutta/full_implicit_formula_base.h"
#include "num_collect/ode/runge_kutta/inexact_newton_decomposed_full_update_equation_solver.h"
#include "num_collect/ode/simple_solver.h"

namespace num_collect::ode::runge_kutta {

namespace impl {

/*!
 * \brief Coefficients in Lobatto IIIC method of order 6 \cite Hairer1991.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
struct lobatto3c6_coefficients {
    //! Type of scalars.
    using scalar_type = Scalar;

    //! Number of stages.
    static constexpr index_type stages = 4;

    /*!
     * \brief Get the coefficients of intermidiate slopes in the formula.
     *
     * \return Coefficients.
     */
    static auto slope_coeffs() -> Eigen::Matrix<scalar_type, 4, 4> {
        static const Eigen::Matrix<scalar_type, 4, 4> coeffs = []() {
            using std::sqrt;
            Eigen::Matrix<scalar_type, 4, 4> coeffs;
            coeffs(0, 0) =
                static_cast<scalar_type>(1) / static_cast<scalar_type>(12);
            coeffs(0, 1) = -sqrt(static_cast<scalar_type>(5)) /
                static_cast<scalar_type>(12);
            coeffs(0, 2) = sqrt(static_cast<scalar_type>(5)) /
                static_cast<scalar_type>(12);
            coeffs(0, 3) =
                -static_cast<scalar_type>(1) / static_cast<scalar_type>(12);
            coeffs(1, 0) =
                static_cast<scalar_type>(1) / static_cast<scalar_type>(12);
            coeffs(1, 1) =
                static_cast<scalar_type>(1) / static_cast<scalar_type>(4);
            coeffs(1, 2) = (static_cast<scalar_type>(10) -
                               static_cast<scalar_type>(7) *
                                   sqrt(static_cast<scalar_type>(5))) /
                static_cast<scalar_type>(60);
            coeffs(1, 3) = sqrt(static_cast<scalar_type>(5)) /
                static_cast<scalar_type>(60);
            coeffs(2, 0) =
                static_cast<scalar_type>(1) / static_cast<scalar_type>(12);
            coeffs(2, 1) = (static_cast<scalar_type>(10) +
                               static_cast<scalar_type>(7) *
                                   sqrt(static_cast<scalar_type>(5))) /
                static_cast<scalar_type>(60);
            coeffs(2, 2) =
                static_cast<scalar_type>(1) / static_cast<scalar_type>(4);
            coeffs(2, 3) = -sqrt(static_cast<scalar_type>(5)) /
                static_cast<scalar_type>(60);
            coeffs(3, 0) =
                static_cast<scalar_type>(1) / static_cast<scalar_type>(12);
            coeffs(3, 1) =
                static_cast<scalar_type>(5) / static_cast<scalar_type>(12);
            coeffs(3, 2) =
                static_cast<scalar_type>(5) / static_cast<scalar_type>(12);
            coeffs(3, 3) =
                static_cast<scalar_type>(1) / static_cast<scalar_type>(12);
            return coeffs;
        }();
        return coeffs;
    }

    /*!
     * \brief Get the coefficients of time in the formula.
     *
     * \return Coefficients.
     */
    static auto time_coeffs() -> Eigen::Vector4<scalar_type> {
        static const Eigen::Vector4<scalar_type> coeffs = []() {
            Eigen::Vector4<scalar_type> coeffs;
            coeffs(0) = static_cast<scalar_type>(0);
            coeffs(1) = (static_cast<scalar_type>(5) -
                            sqrt(static_cast<scalar_type>(5))) /
                static_cast<scalar_type>(10);
            coeffs(2) = (static_cast<scalar_type>(5) +
                            sqrt(static_cast<scalar_type>(5))) /
                static_cast<scalar_type>(10);
            coeffs(3) = static_cast<scalar_type>(1);
            return coeffs;
        }();
        return coeffs;
    }

    /*!
     * \brief Get the coefficients of intermidiate updates in the formula.
     *
     * \return Coefficients.
     */
    static auto update_coeffs() -> Eigen::Vector4<scalar_type> {
        static const Eigen::Vector4<scalar_type> coeffs = []() {
            Eigen::Vector4<scalar_type> coeffs;
            coeffs(0) =
                static_cast<scalar_type>(1) / static_cast<scalar_type>(12);
            coeffs(1) =
                static_cast<scalar_type>(5) / static_cast<scalar_type>(12);
            coeffs(2) =
                static_cast<scalar_type>(5) / static_cast<scalar_type>(12);
            coeffs(3) =
                static_cast<scalar_type>(1) / static_cast<scalar_type>(12);
            return coeffs;
        }();
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
        static const auto data =
            inexact_newton_decomposed_full_update_equation_solver_data<
                scalar_type, stages>::from_butcher_tableau(slope_coeffs(),
                time_coeffs());
        return data;
    };
};

}  // namespace impl

/*!
 * \brief Class of Lobatto IIIC method of order 6 \cite Hairer1991.
 *
 * \tparam Problem Type of problem.
 *
 * \note This formula does not support changing mass matrix.
 */
template <concepts::differentiable_problem Problem>
class lobatto3c6_formula
    : public full_implicit_formula_base<lobatto3c6_formula<Problem>, Problem,
          inexact_newton_decomposed_full_update_equation_solver<Problem, 4>> {
public:
    //! Type of base class.
    using base_type =
        full_implicit_formula_base<lobatto3c6_formula<Problem>, Problem,
            inexact_newton_decomposed_full_update_equation_solver<Problem, 4>>;

    using typename base_type::formula_solver_type;
    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    using base_type::base_type;
    using base_type::formula_solver;
    using base_type::problem;

    //! Number of stages of this formula.
    static constexpr index_type stages = 4;

    //! Order of this formula.
    static constexpr index_type order = 6;

    //! Log tag.
    static constexpr auto log_tag = logging::log_tag_view(
        "num_collect::ode::runge_kutta::lobatto3c6_formula");

    /*!
     * \brief Get the coefficients of intermidiate slopes in the formula.
     *
     * \return Coefficients.
     */
    static auto slope_coeffs() -> Eigen::Matrix<scalar_type, 4, 4> {
        return impl::lobatto3c6_coefficients<scalar_type>::slope_coeffs();
    }

    /*!
     * \brief Get the coefficients of time in the formula.
     *
     * \return Coefficients.
     */
    static auto time_coeffs() -> Eigen::Vector4<scalar_type> {
        return impl::lobatto3c6_coefficients<scalar_type>::time_coeffs();
    }

    /*!
     * \brief Get the coefficients of intermidiate updates in the formula.
     *
     * \return Coefficients.
     */
    static auto update_coeffs() -> Eigen::Vector4<scalar_type> {
        return impl::lobatto3c6_coefficients<scalar_type>::update_coeffs();
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] problem Problem.
     */
    explicit lobatto3c6_formula(const problem_type& problem = problem_type())
        : base_type(problem,
              impl::lobatto3c6_coefficients<
                  scalar_type>::formula_solver_data()) {}

    //! \copydoc ode::formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        updates_.resize(get_size(current) * stages);
        updates_.setZero();
        formula_solver().init(problem(), time, step_size, current, updates_);
        formula_solver().solve();
        if constexpr (base::concepts::dense_vector<variable_type>) {
            estimate = current + updates_.tail(get_size(current));
        } else {
            estimate = current + updates_(updates_.size() - 1);
        }
    }

private:
    //! Type of the vector of intermidiate updates.
    using update_vector_type = typename formula_solver_type::update_vector_type;

    //! Intermidiate updates.
    update_vector_type updates_;
};

/*!
 * \brief Class of solver using Lobatto IIIC method of order 6.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
using lobatto3c6_solver = simple_solver<lobatto3c6_formula<Problem>>;

/*!
 * \brief Class of solver using Lobatto IIIC method of order 6 with automatic
 * step sizes.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
using lobatto3c6_auto_solver =
    non_embedded_auto_solver<lobatto3c6_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
