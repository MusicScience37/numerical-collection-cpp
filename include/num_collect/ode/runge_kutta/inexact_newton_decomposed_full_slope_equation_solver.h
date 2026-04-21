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
 * \brief Definition of inexact_newton_decomposed_full_slope_equation_solver
 * class.
 */
#pragma once

#include <cmath>
#include <concepts>
#include <cstdlib>
#include <optional>
#include <variant>

#include <Eigen/Core>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/LU>

#include "num_collect/base/index_type.h"
#include "num_collect/base/iterative_solver_base.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/ode/concepts/differentiable_problem.h"
#include "num_collect/ode/concepts/mass_problem.h"
#include "num_collect/ode/concepts/multi_variate_differentiable_problem.h"
#include "num_collect/ode/concepts/single_variate_differentiable_problem.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/runge_kutta/impl/inexact_newton_decomposed_jacobian_complex_eigen_solver.h"
#include "num_collect/ode/runge_kutta/impl/inexact_newton_decomposed_jacobian_real_eigen_solver.h"
#include "num_collect/ode/runge_kutta/inexact_newton_decomposed_full_equation_solver_data.h"
#include "num_collect/util/vector.h"

namespace num_collect::ode::runge_kutta {

//! Log tag.
constexpr auto inexact_newton_decomposed_full_slope_equation_solver_tag =
    logging::log_tag_view(
        "num_collect::ode::runge_kutta::inexact_newton_decomposed_full_slope_"
        "equation_solver");

/*!
 * \brief Class to solve equations of implicit slopes in full implicit
 * Runge-Kutta methods using inexact Newton method with eigenvalue
 * decomposition \cite Hairer1991.
 *
 * \tparam Problem Type of the problem.
 * \tparam NumStages Number of stages of the formula.
 *
 * This class solves following equation using the stop criterion written in
 * \cite Hairer1991 :
 *
 * \f[
 * \boldsymbol{k}_i = \boldsymbol{f}\left(t + b_i h, \boldsymbol{y}(t)
 * + h \sum_{j = 1}^s a_{ij} \boldsymbol{k}_j \right)
 * \f]
 */
template <concepts::differentiable_problem Problem, int NumStages>
class inexact_newton_decomposed_full_slope_equation_solver;

/*!
 * \brief Class to solve equations of implicit slopes in full implicit
 * Runge-Kutta methods using inexact Newton method with eigenvalue
 * decomposition \cite Hairer1991.
 *
 * \tparam Problem Type of the problem.
 * \tparam NumStages Number of stages of the formula.
 *
 * This class solves following equation using the stop criterion written in
 * \cite Hairer1991 :
 *
 * \f[
 * \boldsymbol{k}_i = \boldsymbol{f}\left(t + b_i h, \boldsymbol{y}(t)
 * + h \sum_{j = 1}^s a_{ij} \boldsymbol{k}_j \right)
 * \f]
 */
template <concepts::single_variate_differentiable_problem Problem,
    int NumStages>
class inexact_newton_decomposed_full_slope_equation_solver<Problem, NumStages>
    : public iterative_solver_base<
          inexact_newton_decomposed_full_slope_equation_solver<Problem,
              NumStages>> {
public:
    //! This class.
    using this_type =
        inexact_newton_decomposed_full_slope_equation_solver<Problem,
            NumStages>;

    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(std::floating_point<scalar_type>,
        "Current implementation only supports real scalars.");

    //! Type of the matrix of coefficients of intermediate slopes.
    using slope_coeff_matrix_type =
        Eigen::Matrix<scalar_type, NumStages, NumStages>;

    //! Type of the vector of coefficients.
    using coeff_vector_type = Eigen::Vector<scalar_type, NumStages>;

    //! Type of the vector of intermediate slopes.
    using slope_vector_type = Eigen::Vector<variable_type, NumStages>;

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    //! Type of solvers of decomposed linear equations for real eigenvalues.
    using real_eigen_decomposed_solver_type =
        impl::inexact_newton_decomposed_jacobian_real_eigen_solver<
            problem_type>;

    //! Type of solvers of decomposed linear equations for complex eigenvalues.
    using complex_eigen_decomposed_solver_type =
        impl::inexact_newton_decomposed_jacobian_complex_eigen_solver<
            problem_type>;

    //! Type of variants of solvers of decomposed linear equations.
    using decomposed_solver_type =
        std::variant<real_eigen_decomposed_solver_type,
            complex_eigen_decomposed_solver_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] data Data for the solver.
     */
    explicit inexact_newton_decomposed_full_slope_equation_solver(
        const inexact_newton_decomposed_full_equation_solver_data<scalar_type,
            NumStages>& data)
        : iterative_solver_base<
              inexact_newton_decomposed_full_slope_equation_solver<Problem,
                  NumStages>>(
              inexact_newton_decomposed_full_slope_equation_solver_tag),
          slope_coeffs_(data.slope_coeffs()),
          time_coeffs_(data.time_coeffs()),
          decomposed_solvers_(
              impl::generate_decomposed_solvers<problem_type>(data)),
          slope_coeffs_eigenvectors_(data.eigenvectors()),
          slope_coeffs_eigenvectors_inverse_(data.eigenvectors_inverse()) {}

    /*!
     * \brief Initialize the solver to calculate updates.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variables.
     * \param[in,out] solution Solution of updates. The initial values are used
     * as the initial guess of the solution.
     */
    void init(problem_type& problem, scalar_type time, scalar_type step_size,
        const variable_type& variable, slope_vector_type& solution) {
        problem_ = &problem;
        time_ = time;
        step_size_ = step_size;
        variable_ = variable;
        solution_ = &solution;

        problem_->evaluate_on(time_, variable_,
            evaluation_type{
                .diff_coeff = true, .jacobian = true, .mass = use_mass});

        for (auto& solver : decomposed_solvers_) {
            std::visit(
                [this](auto& concrete_solver) {
                    concrete_solver.update_jacobian(*problem_, step_size_);
                },
                solver);
        }

        solution_in_eigenvector_space_ =
            slope_coeffs_eigenvectors_inverse_ * solution;

        update_norm_.reset();
        if (update_reduction_rate_) {
            constexpr auto exponent = static_cast<scalar_type>(0.8);
            constexpr auto min_rate = static_cast<scalar_type>(0.5);
            using std::pow;
            *update_reduction_rate_ = pow(*update_reduction_rate_, exponent);
            if (*update_reduction_rate_ < min_rate) {
                *update_reduction_rate_ = min_rate;
            }
        }
        iterations_ = 0;
    }

    /*!
     * \brief Iterate the algorithm once.
     *
     * \warning Any required initializations (with init
     * functions) are assumed to have been done.
     */
    void iterate() {
        NUM_COLLECT_PRECONDITION(problem_ != nullptr && solution_ != nullptr,
            this->logger(), "Initialization must be done before iterations.");

        // Calculate slopes and the term of mass.
        if constexpr (use_mass) {
            for (int i = 0; i < NumStages; ++i) {
                temp_variable_ = variable_;
                for (int j = 0; j < NumStages; ++j) {
                    temp_variable_ +=
                        step_size_ * slope_coeffs_(i, j) * (*solution_)(j);
                }
                problem_->evaluate_on(time_ + time_coeffs_(i) * step_size_,
                    temp_variable_,
                    evaluation_type{.diff_coeff = true, .mass = use_mass});
                slopes_(i) =
                    problem_->diff_coeff() - problem_->mass() * (*solution_)(i);
            }
            residual_in_eigenvector_space_ =
                slope_coeffs_eigenvectors_inverse_ * slopes_;
        } else {
            for (int i = 0; i < NumStages; ++i) {
                temp_variable_ = variable_;
                for (int j = 0; j < NumStages; ++j) {
                    temp_variable_ +=
                        step_size_ * slope_coeffs_(i, j) * (*solution_)(j);
                }
                problem_->evaluate_on(time_ + time_coeffs_(i) * step_size_,
                    temp_variable_,
                    evaluation_type{.diff_coeff = true, .mass = use_mass});
                slopes_(i) = problem_->diff_coeff();
            }
            residual_in_eigenvector_space_ = -solution_in_eigenvector_space_;
            residual_in_eigenvector_space_.noalias() +=
                slope_coeffs_eigenvectors_inverse_ * slopes_;
        }

        // Calculate residuals.
        index_type diagonal_index = 0;
        for (auto& solver : decomposed_solvers_) {
            std::visit(
                [this, &diagonal_index](auto& concrete_solver) {
                    using solver_type = std::decay_t<decltype(concrete_solver)>;
                    if constexpr (std::is_same_v<solver_type,
                                      real_eigen_decomposed_solver_type>) {
                        concrete_solver.apply_eigenvalue_inverse(
                            residual_in_eigenvector_space_(diagonal_index));
                        ++diagonal_index;
                    } else {
                        concrete_solver.apply_eigenvalue_inverse(
                            residual_in_eigenvector_space_.segment(
                                diagonal_index, 2));
                        diagonal_index += 2;
                    }
                },
                solver);
        }
        residual_in_eigenvector_space_ *=
            static_cast<scalar_type>(1) / step_size_;

        // Solve the linear equation.
        diagonal_index = 0;
        for (auto& solver : decomposed_solvers_) {
            std::visit(
                [this, &diagonal_index](auto& concrete_solver) {
                    using solver_type = std::decay_t<decltype(concrete_solver)>;
                    if constexpr (std::is_same_v<solver_type,
                                      real_eigen_decomposed_solver_type>) {
                        concrete_solver.solve(
                            residual_in_eigenvector_space_(diagonal_index),
                            update_in_eigenvector_space_(diagonal_index));
                        ++diagonal_index;
                    } else {
                        concrete_solver.solve(
                            residual_in_eigenvector_space_.segment(
                                diagonal_index, 2),
                            update_in_eigenvector_space_.segment(
                                diagonal_index, 2));
                        diagonal_index += 2;
                    }
                },
                solver);
        }

        solution_in_eigenvector_space_ += update_in_eigenvector_space_;
        update_ = slope_coeffs_eigenvectors_ * update_in_eigenvector_space_;
        *solution_ += update_;

        scalar_type update_norm = static_cast<scalar_type>(0);
        for (int i = 0; i < NumStages; ++i) {
            const scalar_type stage_update_norm =
                tolerances().calc_norm(variable_, update_(i));
            update_norm += stage_update_norm * stage_update_norm;
        }
        update_norm = std::sqrt(update_norm);
        if (update_norm_) {
            update_reduction_rate_ = update_norm / (*update_norm_);
        }
        update_norm_ = update_norm;

        ++iterations_;
    }

    /*!
     * \brief Determine if stopping criteria of the algorithm are satisfied.
     *
     * \return If stopping criteria of the algorithm are satisfied.
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        bool converged = false;
        if (update_norm_ && update_reduction_rate_ &&
            *update_reduction_rate_ < static_cast<scalar_type>(1)) {
            converged =
                (*update_reduction_rate_ /
                    (static_cast<scalar_type>(1) - *update_reduction_rate_)) *
                    (*update_norm_) <=
                tolerance_rate_;
        }

        constexpr index_type max_iterations = 1000;  // safe guard
        return converged || (iterations_ > max_iterations);
    }

    /*!
     * \brief Configure an iteration logger.
     *
     * \param[in] iteration_logger Iteration logger.
     */
    void configure_iteration_logger(
        logging::iterations::iteration_logger<this_type>& iteration_logger)
        const {
        iteration_logger.template append<index_type>(
            "Iter.", &this_type::iterations);
        iteration_logger.template append<scalar_type>(
            "Update", &this_type::update_norm);
    }

    /*!
     * \brief Get the norm of update.
     *
     * \return Norm of update.
     */
    [[nodiscard]] auto update_norm() const -> scalar_type {
        if (!update_norm_) {
            return static_cast<scalar_type>(0);
        }
        return *update_norm_;
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const -> index_type { return iterations_; }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val)
        -> inexact_newton_decomposed_full_slope_equation_solver& {
        tolerances_ = val;
        return *this;
    }

    /*!
     * \brief Get the error tolerances.
     *
     * \return Error tolerances.
     */
    [[nodiscard]] auto tolerances() const
        -> const error_tolerances<variable_type>& {
        return tolerances_;
    }

    /*!
     * \brief Get the solvers of decomposed linear equations.
     *
     * \return Solvers of decomposed linear equations.
     */
    [[nodiscard]] auto decomposed_solvers() const noexcept
        -> const util::vector<decomposed_solver_type>& {
        return decomposed_solvers_;
    }

    /*!
     * \brief Get the solvers of decomposed linear equations.
     *
     * \return Solvers of decomposed linear equations.
     */
    [[nodiscard]] auto decomposed_solvers() noexcept
        -> util::vector<decomposed_solver_type>& {
        return decomposed_solvers_;
    }

private:
    //! Coefficients of intermediate slopes in the formula.
    slope_coeff_matrix_type slope_coeffs_;

    //! Coefficients of time.
    coeff_vector_type time_coeffs_;

    //! Solvers of decomposed linear equations.
    util::vector<decomposed_solver_type> decomposed_solvers_;

    //! Eigenvectors of coefficients of intermediate slopes.
    slope_coeff_matrix_type slope_coeffs_eigenvectors_;

    //! Inverse of eigenvectors of coefficients of intermediate slopes.
    slope_coeff_matrix_type slope_coeffs_eigenvectors_inverse_;

    //! Pointer to the problem.
    problem_type* problem_{nullptr};

    //! Time.
    scalar_type time_{};

    //! Step size.
    scalar_type step_size_{};

    //! Variable.
    variable_type variable_{};

    //! Solution of updates.
    slope_vector_type* solution_{nullptr};

    //! Solution in eigenvector space.
    slope_vector_type solution_in_eigenvector_space_{};

    //! Temporary variable.
    variable_type temp_variable_{};

    //! Buffer of slopes.
    slope_vector_type slopes_{};

    //! Residual vector in eigenvector space.
    slope_vector_type residual_in_eigenvector_space_{};

    //! Newton update vector in eigenvector space.
    slope_vector_type update_in_eigenvector_space_{};

    //! Newton update vector.
    slope_vector_type update_{};

    //! Norm of update.
    std::optional<scalar_type> update_norm_{};

    //! Rate in which update is reduced from the previous step.
    std::optional<scalar_type> update_reduction_rate_{};

    //! Default rate of tolerance in this solver.
    static constexpr auto default_tolerance_rate =
        static_cast<scalar_type>(1e-2);

    //! Rate of tolerance in this solver.
    scalar_type tolerance_rate_{default_tolerance_rate};

    //! Number of iterations.
    index_type iterations_{0};

    //! Error tolerances.
    error_tolerances<variable_type> tolerances_{};
};

/*!
 * \brief Class to solve equations of implicit slopes in full implicit
 * Runge-Kutta methods using inexact Newton method with eigenvalue
 * decomposition \cite Hairer1991.
 *
 * \tparam Problem Type of the problem.
 * \tparam NumStages Number of stages of the formula.
 *
 * This class solves following equation using the stop criterion written in
 * \cite Hairer1991 :
 *
 * \f[
 * \boldsymbol{k}_i = \boldsymbol{f}\left(t + b_i h, \boldsymbol{y}(t)
 * + h \sum_{j = 1}^s a_{ij} \boldsymbol{k}_j \right)
 * \f]
 */
template <concepts::multi_variate_differentiable_problem Problem, int NumStages>
class inexact_newton_decomposed_full_slope_equation_solver<Problem, NumStages>
    : public iterative_solver_base<
          inexact_newton_decomposed_full_slope_equation_solver<Problem,
              NumStages>> {
public:
    //! This class.
    using this_type =
        inexact_newton_decomposed_full_slope_equation_solver<Problem,
            NumStages>;

    //! Type of problem.
    using problem_type = Problem;

    //! Type of variables.
    using variable_type = typename problem_type::variable_type;

    //! Type of scalars.
    using scalar_type = typename problem_type::scalar_type;

    //! Type of Jacobian.
    using jacobian_type = typename problem_type::jacobian_type;

    static_assert(std::floating_point<scalar_type>,
        "Current implementation only supports real scalars.");

    //! Type of the matrix of coefficients of intermediate slopes.
    using slope_coeff_matrix_type =
        Eigen::Matrix<scalar_type, NumStages, NumStages>;

    //! Type of the vector of coefficients.
    using coeff_vector_type = Eigen::Vector<scalar_type, NumStages>;

    //! Whether to use mass.
    static constexpr bool use_mass = concepts::mass_problem<problem_type>;

    //! Number of dimension at compile-time.
    static constexpr int num_dimensions_at_compile_time =
        variable_type::SizeAtCompileTime;

    //! Size of the vectors of intermediate slopes at compile-time.
    static constexpr int slope_vector_size_at_compile_time =
        (num_dimensions_at_compile_time == Eigen::Dynamic)
        ? Eigen::Dynamic
        : NumStages * num_dimensions_at_compile_time;

    //! Type of the vector of intermediate slopes.
    using slope_vector_type =
        Eigen::Vector<scalar_type, slope_vector_size_at_compile_time>;

    //! Type of solvers of decomposed linear equations for real eigenvalues.
    using real_eigen_decomposed_solver_type =
        impl::inexact_newton_decomposed_jacobian_real_eigen_solver<
            problem_type>;

    //! Type of solvers of decomposed linear equations for complex eigenvalues.
    using complex_eigen_decomposed_solver_type =
        impl::inexact_newton_decomposed_jacobian_complex_eigen_solver<
            problem_type>;

    //! Type of variants of solvers of decomposed linear equations.
    using decomposed_solver_type =
        std::variant<real_eigen_decomposed_solver_type,
            complex_eigen_decomposed_solver_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] data Data for the solver.
     */
    explicit inexact_newton_decomposed_full_slope_equation_solver(
        const inexact_newton_decomposed_full_equation_solver_data<scalar_type,
            NumStages>& data)
        : iterative_solver_base<
              inexact_newton_decomposed_full_slope_equation_solver<Problem,
                  NumStages>>(
              inexact_newton_decomposed_full_slope_equation_solver_tag),
          slope_coeffs_(data.slope_coeffs()),
          time_coeffs_(data.time_coeffs()),
          decomposed_solvers_(
              impl::generate_decomposed_solvers<problem_type>(data)),
          slope_coeffs_eigenvectors_(data.eigenvectors()),
          slope_coeffs_eigenvectors_inverse_(data.eigenvectors_inverse()) {}

    /*!
     * \brief Initialize the solver to calculate updates.
     *
     * \param[in] problem Problem.
     * \param[in] time Time.
     * \param[in] step_size Step size.
     * \param[in] variable Variables.
     * \param[in,out] solution Solution of updates. The initial values are used
     * as the initial guess of the solution.
     */
    void init(problem_type& problem, scalar_type time, scalar_type step_size,
        const variable_type& variable, slope_vector_type& solution) {
        problem_ = &problem;
        time_ = time;
        step_size_ = step_size;
        variable_ = variable;
        solution_ = &solution;

        problem_->evaluate_on(time_, variable_,
            evaluation_type{
                .diff_coeff = true, .jacobian = true, .mass = use_mass});

        for (auto& solver : decomposed_solvers_) {
            std::visit(
                [this](auto& concrete_solver) {
                    concrete_solver.update_jacobian(*problem_, step_size_);
                },
                solver);
        }

        const index_type dimensions = variable.size();

        solution_in_eigenvector_space_.resize(dimensions * NumStages);
        solution_in_eigenvector_space_.setZero();
        for (int j = 0; j < NumStages; ++j) {
            for (int i = 0; i < NumStages; ++i) {
                solution_in_eigenvector_space_.segment(i * dimensions,
                    dimensions) += slope_coeffs_eigenvectors_inverse_(i, j) *
                    solution.segment(j * dimensions, dimensions);
            }
        }
        slopes_.resize(dimensions * NumStages);
        residual_in_eigenvector_space_.resize(dimensions * NumStages);
        update_in_eigenvector_space_.resize(dimensions * NumStages);
        update_.resize(dimensions * NumStages);

        update_norm_.reset();
        if (update_reduction_rate_) {
            constexpr auto exponent = static_cast<scalar_type>(0.8);
            constexpr auto min_rate = static_cast<scalar_type>(0.5);
            using std::pow;
            *update_reduction_rate_ = pow(*update_reduction_rate_, exponent);
            if (*update_reduction_rate_ < min_rate) {
                *update_reduction_rate_ = min_rate;
            }
        }
        iterations_ = 0;
    }

    /*!
     * \brief Iterate the algorithm once.
     *
     * \warning Any required initializations (with init
     * functions) are assumed to have been done.
     */
    void iterate() {
        NUM_COLLECT_PRECONDITION(problem_ != nullptr && solution_ != nullptr,
            this->logger(), "Initialization must be done before iterations.");

        const index_type dimensions = variable_.size();

        // Calculate slopes and the term of mass.
        if constexpr (use_mass) {
            for (int i = 0; i < NumStages; ++i) {
                temp_variable_ = variable_;
                for (int j = 0; j < NumStages; ++j) {
                    temp_variable_ += step_size_ * slope_coeffs_(i, j) *
                        (*solution_).segment(j * dimensions, dimensions);
                }
                problem_->evaluate_on(time_ + time_coeffs_(i) * step_size_,
                    temp_variable_,
                    evaluation_type{.diff_coeff = true, .mass = use_mass});
                slopes_.segment(i * dimensions, dimensions) =
                    problem_->diff_coeff() -
                    problem_->mass() *
                        (*solution_).segment(i * dimensions, dimensions);
            }
            residual_in_eigenvector_space_.setZero();
            for (int i = 0; i < NumStages; ++i) {
                for (int j = 0; j < NumStages; ++j) {
                    residual_in_eigenvector_space_.segment(
                        i * dimensions, dimensions) +=
                        slope_coeffs_eigenvectors_inverse_(i, j) *
                        slopes_.segment(j * dimensions, dimensions);
                }
            }
        } else {
            for (int i = 0; i < NumStages; ++i) {
                temp_variable_ = variable_;
                for (int j = 0; j < NumStages; ++j) {
                    temp_variable_ += step_size_ * slope_coeffs_(i, j) *
                        (*solution_).segment(j * dimensions, dimensions);
                }
                problem_->evaluate_on(time_ + time_coeffs_(i) * step_size_,
                    temp_variable_,
                    evaluation_type{.diff_coeff = true, .mass = use_mass});
                slopes_.segment(i * dimensions, dimensions) =
                    problem_->diff_coeff();
            }
            residual_in_eigenvector_space_ = -solution_in_eigenvector_space_;
            for (int i = 0; i < NumStages; ++i) {
                for (int j = 0; j < NumStages; ++j) {
                    residual_in_eigenvector_space_.segment(
                        i * dimensions, dimensions) +=
                        slope_coeffs_eigenvectors_inverse_(i, j) *
                        slopes_.segment(j * dimensions, dimensions);
                }
            }
        }

        // Calculate residuals.
        index_type diagonal_index = 0;
        for (auto& solver : decomposed_solvers_) {
            std::visit(
                [this, &diagonal_index, dimensions](auto& concrete_solver) {
                    using solver_type = std::decay_t<decltype(concrete_solver)>;
                    if constexpr (std::is_same_v<solver_type,
                                      real_eigen_decomposed_solver_type>) {
                        concrete_solver.apply_eigenvalue_inverse(
                            residual_in_eigenvector_space_.segment(
                                diagonal_index, dimensions));
                        diagonal_index += dimensions;
                    } else {
                        concrete_solver.apply_eigenvalue_inverse(
                            residual_in_eigenvector_space_.segment(
                                diagonal_index, 2 * dimensions));
                        diagonal_index += 2 * dimensions;
                    }
                },
                solver);
        }
        residual_in_eigenvector_space_ *=
            static_cast<scalar_type>(1) / step_size_;

        // Solve the linear equation.
        diagonal_index = 0;
        for (auto& solver : decomposed_solvers_) {
            std::visit(
                [this, &diagonal_index, dimensions](auto& concrete_solver) {
                    using solver_type = std::decay_t<decltype(concrete_solver)>;
                    if constexpr (std::is_same_v<solver_type,
                                      real_eigen_decomposed_solver_type>) {
                        concrete_solver.solve(
                            residual_in_eigenvector_space_.segment(
                                diagonal_index, dimensions),
                            update_in_eigenvector_space_.segment(
                                diagonal_index, dimensions));
                        diagonal_index += dimensions;
                    } else {
                        concrete_solver.solve(
                            residual_in_eigenvector_space_.segment(
                                diagonal_index, 2 * dimensions),
                            update_in_eigenvector_space_.segment(
                                diagonal_index, 2 * dimensions));
                        diagonal_index += 2 * dimensions;
                    }
                },
                solver);
        }

        solution_in_eigenvector_space_ += update_in_eigenvector_space_;
        update_.setZero();
        for (int i = 0; i < NumStages; ++i) {
            for (int j = 0; j < NumStages; ++j) {
                update_.segment(i * dimensions, dimensions) +=
                    slope_coeffs_eigenvectors_(i, j) *
                    update_in_eigenvector_space_.segment(
                        j * dimensions, dimensions);
            }
        }
        *solution_ += update_;

        scalar_type update_norm = static_cast<scalar_type>(0);
        for (int i = 0; i < NumStages; ++i) {
            const scalar_type stage_update_norm = tolerances().calc_norm(
                variable_, update_.segment(i * dimensions, dimensions));
            update_norm += stage_update_norm * stage_update_norm;
        }
        update_norm = std::sqrt(update_norm);
        if (update_norm_) {
            update_reduction_rate_ = update_norm / (*update_norm_);
        }
        update_norm_ = update_norm;

        ++iterations_;
    }

    /*!
     * \brief Determine if stopping criteria of the algorithm are satisfied.
     *
     * \return If stopping criteria of the algorithm are satisfied.
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        bool converged = false;
        if (update_norm_ && update_reduction_rate_ &&
            *update_reduction_rate_ < static_cast<scalar_type>(1)) {
            converged =
                (*update_reduction_rate_ /
                    (static_cast<scalar_type>(1) - *update_reduction_rate_)) *
                    (*update_norm_) <=
                tolerance_rate_;
        }

        constexpr index_type max_iterations = 1000;  // safe guard
        return converged || (iterations_ > max_iterations);
    }

    /*!
     * \brief Configure an iteration logger.
     *
     * \param[in] iteration_logger Iteration logger.
     */
    void configure_iteration_logger(
        logging::iterations::iteration_logger<this_type>& iteration_logger)
        const {
        iteration_logger.template append<index_type>(
            "Iter.", &this_type::iterations);
        iteration_logger.template append<scalar_type>(
            "Update", &this_type::update_norm);
    }

    /*!
     * \brief Get the norm of update.
     *
     * \return Norm of update.
     */
    [[nodiscard]] auto update_norm() const -> scalar_type {
        if (!update_norm_) {
            return static_cast<scalar_type>(0);
        }
        return *update_norm_;
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const -> index_type { return iterations_; }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val)
        -> inexact_newton_decomposed_full_slope_equation_solver& {
        tolerances_ = val;
        return *this;
    }

    /*!
     * \brief Get the error tolerances.
     *
     * \return Error tolerances.
     */
    [[nodiscard]] auto tolerances() const
        -> const error_tolerances<variable_type>& {
        return tolerances_;
    }

    /*!
     * \brief Get the solvers of decomposed linear equations.
     *
     * \return Solvers of decomposed linear equations.
     */
    [[nodiscard]] auto decomposed_solvers() const noexcept
        -> const util::vector<decomposed_solver_type>& {
        return decomposed_solvers_;
    }

    /*!
     * \brief Get the solvers of decomposed linear equations.
     *
     * \return Solvers of decomposed linear equations.
     */
    [[nodiscard]] auto decomposed_solvers() noexcept
        -> util::vector<decomposed_solver_type>& {
        return decomposed_solvers_;
    }

private:
    //! Coefficients of intermediate slopes in the formula.
    slope_coeff_matrix_type slope_coeffs_;

    //! Coefficients of time.
    coeff_vector_type time_coeffs_;

    //! Solvers of decomposed linear equations.
    util::vector<decomposed_solver_type> decomposed_solvers_;

    //! Eigenvectors of coefficients of intermediate slopes.
    slope_coeff_matrix_type slope_coeffs_eigenvectors_;

    //! Inverse of eigenvectors of coefficients of intermediate slopes.
    slope_coeff_matrix_type slope_coeffs_eigenvectors_inverse_;

    //! Pointer to the problem.
    problem_type* problem_{nullptr};

    //! Time.
    scalar_type time_{};

    //! Step size.
    scalar_type step_size_{};

    //! Variable.
    variable_type variable_{};

    //! Solution of updates.
    slope_vector_type* solution_{nullptr};

    //! Solution in eigenvector space.
    slope_vector_type solution_in_eigenvector_space_{};

    //! Temporary variable.
    variable_type temp_variable_{};

    //! Buffer of slopes.
    slope_vector_type slopes_{};

    //! Residual vector in eigenvector space.
    slope_vector_type residual_in_eigenvector_space_{};

    //! Newton update vector in eigenvector space.
    slope_vector_type update_in_eigenvector_space_{};

    //! Newton update vector.
    slope_vector_type update_{};

    //! Norm of update.
    std::optional<scalar_type> update_norm_{};

    //! Rate in which update is reduced from the previous step.
    std::optional<scalar_type> update_reduction_rate_{};

    //! Default rate of tolerance in this solver.
    static constexpr auto default_tolerance_rate =
        static_cast<scalar_type>(1e-2);

    //! Rate of tolerance in this solver.
    scalar_type tolerance_rate_{default_tolerance_rate};

    //! Number of iterations.
    index_type iterations_{0};

    //! Error tolerances.
    error_tolerances<variable_type> tolerances_{};
};

}  // namespace num_collect::ode::runge_kutta
