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
 * \brief Definition of radau2a5_formula class.
 */
#pragma once

#include <array>
#include <cmath>
#include <type_traits>
#include <variant>

#include <Eigen/Core>

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/get_size.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/differentiable_problem.h"
#include "num_collect/ode/formula_base.h"
#include "num_collect/ode/non_embedded_formula_wrapper.h"
#include "num_collect/ode/runge_kutta/impl/inexact_newton_decomposed_jacobian_real_eigen_solver.h"
#include "num_collect/ode/runge_kutta/inexact_newton_decomposed_full_update_equation_solver.h"
#include "num_collect/ode/simple_solver.h"

namespace num_collect::ode::runge_kutta {

namespace impl {

/*!
 * \brief Coefficients in Radau IIA method of order 5 \cite Hairer1991.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
struct radau2a5_coefficients {
    //! Type of scalars.
    using scalar_type = Scalar;

    //! Number of stages.
    static constexpr index_type stages = 3;

    /*!
     * \brief Get the coefficients of intermidiate slopes in the formula.
     *
     * \return Coefficients.
     */
    static auto slope_coeffs() -> Eigen::Matrix<scalar_type, 3, 3> {
        static const Eigen::Matrix<scalar_type, 3, 3> coeffs = []() {
            using std::sqrt;
            Eigen::Matrix<scalar_type, 3, 3> coeffs;
            coeffs(0, 0) = (static_cast<scalar_type>(88) -
                               static_cast<scalar_type>(7) *
                                   sqrt(static_cast<scalar_type>(6))) /
                static_cast<scalar_type>(360);
            coeffs(0, 1) = (static_cast<scalar_type>(296) -
                               static_cast<scalar_type>(169) *
                                   sqrt(static_cast<scalar_type>(6))) /
                static_cast<scalar_type>(1800);
            coeffs(0, 2) = (static_cast<scalar_type>(-2) +
                               static_cast<scalar_type>(3) *
                                   sqrt(static_cast<scalar_type>(6))) /
                static_cast<scalar_type>(225);
            coeffs(1, 0) = (static_cast<scalar_type>(296) +
                               static_cast<scalar_type>(169) *
                                   sqrt(static_cast<scalar_type>(6))) /
                static_cast<scalar_type>(1800);
            coeffs(1, 1) = (static_cast<scalar_type>(88) +
                               static_cast<scalar_type>(7) *
                                   sqrt(static_cast<scalar_type>(6))) /
                static_cast<scalar_type>(360);
            coeffs(1, 2) = (static_cast<scalar_type>(-2) -
                               static_cast<scalar_type>(3) *
                                   sqrt(static_cast<scalar_type>(6))) /
                static_cast<scalar_type>(225);
            coeffs(2, 0) = (static_cast<scalar_type>(16) -
                               sqrt(static_cast<scalar_type>(6))) /
                static_cast<scalar_type>(36);
            coeffs(2, 1) = (static_cast<scalar_type>(16) +
                               sqrt(static_cast<scalar_type>(6))) /
                static_cast<scalar_type>(36);
            coeffs(2, 2) =
                static_cast<scalar_type>(1) / static_cast<scalar_type>(9);
            return coeffs;
        }();
        return coeffs;
    }

    /*!
     * \brief Get the coefficients of time in the formula.
     *
     * \return Coefficients.
     */
    static auto time_coeffs() -> Eigen::Vector3<scalar_type> {
        static const Eigen::Vector3<scalar_type> coeffs = []() {
            using std::sqrt;
            Eigen::Vector3<scalar_type> coeffs;
            coeffs(0) = (static_cast<scalar_type>(4) -
                            sqrt(static_cast<scalar_type>(6))) /
                static_cast<scalar_type>(10);
            coeffs(1) = (static_cast<scalar_type>(4) +
                            sqrt(static_cast<scalar_type>(6))) /
                static_cast<scalar_type>(10);
            coeffs(2) = static_cast<scalar_type>(1);
            return coeffs;
        }();
        return coeffs;
    }

    /*!
     * \brief Get the coefficients of intermidiate updates in the formula.
     *
     * \return Coefficients.
     */
    static auto update_coeffs() -> Eigen::Vector3<scalar_type> {
        static const Eigen::Vector3<scalar_type> coeffs = []() {
            using std::sqrt;
            Eigen::Vector3<scalar_type> coeffs;
            coeffs(0) = (static_cast<scalar_type>(16) -
                            sqrt(static_cast<scalar_type>(6))) /
                static_cast<scalar_type>(36);
            coeffs(1) = (static_cast<scalar_type>(16) +
                            sqrt(static_cast<scalar_type>(6))) /
                static_cast<scalar_type>(36);
            coeffs(2) =
                static_cast<scalar_type>(1) / static_cast<scalar_type>(9);
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
 * \brief Class of Radau IIA method of order 5 \cite Hairer1991.
 *
 * \tparam Problem Type of problem.
 *
 * \note This formula does not support changing mass matrix.
 */
template <concepts::differentiable_problem Problem>
class radau2a5_formula
    : public formula_base<radau2a5_formula<Problem>, Problem> {
public:
    //! Type of base class.
    using base_type = formula_base<radau2a5_formula<Problem>, Problem>;

    using typename base_type::problem_type;
    using typename base_type::scalar_type;
    using typename base_type::variable_type;

    using base_type::base_type;
    using base_type::problem;

protected:
    using base_type::coeff;

public:
    //! Number of stages of this formula.
    static constexpr index_type stages = 3;

    //! Order of this formula.
    static constexpr index_type order = 5;

    //! Log tag.
    static constexpr auto log_tag = logging::log_tag_view(
        "num_collect::ode::runge_kutta::radau2a5_formula");

    //! Type of the solver of the implicit formula.
    using formula_solver_type =
        inexact_newton_decomposed_full_update_equation_solver<Problem, stages>;

    /*!
     * \brief Get the coefficients of intermidiate slopes in the formula.
     *
     * \return Coefficients.
     */
    static auto slope_coeffs() -> Eigen::Matrix<scalar_type, 3, 3> {
        return impl::radau2a5_coefficients<scalar_type>::slope_coeffs();
    }

    /*!
     * \brief Get the coefficients of time in the formula.
     *
     * \return Coefficients.
     */
    static auto time_coeffs() -> Eigen::Vector3<scalar_type> {
        return impl::radau2a5_coefficients<scalar_type>::time_coeffs();
    }

    /*!
     * \brief Get the coefficients of intermidiate updates in the formula.
     *
     * \return Coefficients.
     */
    static auto update_coeffs() -> Eigen::Vector3<scalar_type> {
        return impl::radau2a5_coefficients<scalar_type>::update_coeffs();
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] problem Problem.
     */
    explicit radau2a5_formula(const problem_type& problem = problem_type())
        : base_type(problem) {
        // Search for the real eigenvalue.
        auto& decomposed_solver = formula_solver_.decomposed_solvers();
        for (auto& solver : decomposed_solver) {
            std::visit(
                [this](auto& concrete_solver) {
                    using solver_type = std::decay_t<decltype(concrete_solver)>;
                    if constexpr (std::is_same_v<solver_type,
                                      real_eigenvalue_solver_type>) {
                        real_eigenvalue_solver_ = &concrete_solver;
                    }
                },
                solver);
        }
        // This is guaranteed by the property of the formula.
        NUM_COLLECT_ASSERT(real_eigenvalue_solver_ != nullptr);

        const scalar_type real_eigenvalue =
            real_eigenvalue_solver_->eigenvalue();

        using std::sqrt;
        error_coeffs_[0] = real_eigenvalue;
        error_coeffs_[1] = real_eigenvalue *
            (static_cast<scalar_type>(-13) -
                static_cast<scalar_type>(7) *
                    sqrt(static_cast<scalar_type>(6))) /
            static_cast<scalar_type>(3);
        error_coeffs_[2] = real_eigenvalue *
            (static_cast<scalar_type>(-13) +
                static_cast<scalar_type>(7) *
                    sqrt(static_cast<scalar_type>(6))) /
            static_cast<scalar_type>(3);
        error_coeffs_[3] = real_eigenvalue * static_cast<scalar_type>(-1) /
            static_cast<scalar_type>(3);
    }

    //! \copydoc ode::formula_base::step
    void step(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate) {
        updates_.resize(get_size(current) * stages);
        updates_.setZero();
        formula_solver_.init(problem(), time, step_size, current, updates_);

        // Save a slope for error estimation.
        slope_ = problem().diff_coeff();

        formula_solver_.solve();

        if constexpr (base::concepts::dense_vector<variable_type>) {
            estimate = current + updates_.tail(get_size(current));
        } else {
            estimate = current + updates_(updates_.size() - 1);
        }
    }

    /*!
     * \brief Compute the next variable and the error estimation.
     *
     * \param[in] time Current time.
     * \param[in] step_size Step size.
     * \param[in] current Current variable.
     * \param[out] estimate Estimate of the next variable.
     * \param[out] error Estimate of error.
     *
     * \note This is not an embedded formula and the error estimation did not
     * work for stiff problems.
     */
    void step_with_error_estimate(scalar_type time, scalar_type step_size,
        const variable_type& current, variable_type& estimate,
        variable_type& error) {
        step(time, step_size, current, estimate);

        if constexpr (base::concepts::dense_vector<variable_type>) {
            error_buffer_ = step_size * error_coeffs_[0] * slope_;
            const index_type dimensions = current.size();
            error_buffer_ += error_coeffs_[1] * updates_.segment(0, dimensions);
            error_buffer_ +=
                error_coeffs_[2] * updates_.segment(dimensions, dimensions);
            error_buffer_ +=
                error_coeffs_[3] * updates_.segment(2 * dimensions, dimensions);
        } else {
            // Scalars.
            error_buffer_ = step_size * error_coeffs_[0] * slope_;
            error_buffer_ += error_coeffs_[1] * updates_(0);
            error_buffer_ += error_coeffs_[2] * updates_(1);
            error_buffer_ += error_coeffs_[3] * updates_(2);
        }
        real_eigenvalue_solver_->solve(error_buffer_, error);
        error *= static_cast<scalar_type>(1) /
            (step_size * real_eigenvalue_solver_->eigenvalue());
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
        -> radau2a5_formula& {
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

    //! Type of the solver of equations of decomposed Jacobians for real eigenvalues.
    using real_eigenvalue_solver_type =
        impl::inexact_newton_decomposed_jacobian_real_eigen_solver<
            problem_type>;

    //! Solver of the implicit formula.
    formula_solver_type formula_solver_{
        impl::radau2a5_coefficients<scalar_type>::formula_solver_data()};

    //! Intermidiate updates.
    update_vector_type updates_;

    //! Solver for the real eigenvalue.
    real_eigenvalue_solver_type* real_eigenvalue_solver_{nullptr};

    //! Slope of the last variable.
    variable_type slope_{};

    //! Buffer for calculation of error estimation.
    variable_type error_buffer_{};

    //! Coefficients for error estimation.
    std::array<scalar_type, 4> error_coeffs_{};
};

/*!
 * \brief Class of solver using Radau IIA method of order 5.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
using radau2a5_solver = simple_solver<radau2a5_formula<Problem>>;

/*!
 * \brief Class of solver using Radau IIA method of order 5 with automatic step
 * sizes.
 *
 * \tparam Problem Type of problem.
 */
template <concepts::differentiable_problem Problem>
using radau2a5_auto_solver =
    non_embedded_auto_solver<radau2a5_formula<Problem>>;

}  // namespace num_collect::ode::runge_kutta
