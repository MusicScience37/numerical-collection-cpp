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
 * \brief Definition of fixed_point_iteration_solver class.
 */
#pragma once

#include <concepts>
#include <limits>

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/functions/sqrt.h"
#include "num_collect/logging/iterations/iteration_logger_mixin.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/ode_errors.h"

namespace num_collect::ode::impl {

//! Log tag.
constexpr auto fixed_point_iteration_solver_tag = logging::log_tag_view(
    "num_collect::ode::impl::fixed_point_iteration_solver");

/*!
 * \brief Class to solve equations using fixed-point iteration.
 *
 * \tparam Variable Type of variables.
 *
 * This class solves the equation of the form
 * \f$ x = f(x) \f$
 * using fixed-point iteration with relaxation.
 */
template <typename Variable>
class fixed_point_iteration_solver;

/*!
 * \brief Class to solve equations using fixed-point iteration.
 *
 * \tparam Variable Type of variables.
 *
 * This class solves the equation of the form
 * \f$ x = f(x) \f$
 * using fixed-point iteration with relaxation.
 */
template <concepts::real_scalar_dense_vector Variable>
class fixed_point_iteration_solver<Variable>
    : public logging::logging_mixin,
      public logging::iterations::iteration_logger_mixin<
          fixed_point_iteration_solver<Variable>> {
public:
    //! Type of this class.
    using this_type = fixed_point_iteration_solver<Variable>;

    //! Type of variables.
    using variable_type = Variable;

    //! Type of scalars.
    using scalar_type = typename variable_type::Scalar;

    /*!
     * \brief Constructor.
     */
    fixed_point_iteration_solver()
        : logging::logging_mixin(fixed_point_iteration_solver_tag) {}

    /*!
     * \brief Solve an equation.
     *
     * \tparam Function Type of the function to calculate the right-hand-side.
     * \param[in] function Function to calculate the right-hand-side.
     * \param[in,out] solution Solution vector. (Given vector is used as the
     * initial solution.)
     * \param[in] tolerance_reference Reference variable for calculating error
     * tolerances.
     *
     * This function solves the equation of the form
     * \f$ x = f(x) \f$
     * given the function \f$ f \f$ on the right-hand-side.
     *
     * The function signature must be like:
     *
     * ~~~{.cpp}
     * void function(const variable_type& input, variable_type& output);
     * ~~~
     */
    template <std::invocable<const variable_type& /*input*/,
        variable_type& /*output*/>
            Function>
    void solve(Function&& function, variable_type& solution,
        const variable_type& tolerance_reference) {
        init(function, solution, tolerance_reference);
        auto& iteration_logger = this->initialize_iteration_logger();
        iteration_logger.write_iteration(this);
        while (!is_converged()) {
            step(function, solution, tolerance_reference);
            iteration_logger.write_iteration(this);
            if (!is_converged() && iterations_ >= max_iterations_) {
                NUM_COLLECT_ODE_THROW_NO_CONVERGENCE(this->logger(),
                    "Failed to solve an equation due to no convergence.");
            }
        }
        iteration_logger.write_summary(this);
    }

    /*!
     * \brief Configure an iteration logger.
     *
     * \param[in] iteration_logger Iteration logger.
     */
    void configure_iteration_logger(logging::iterations::iteration_logger<
        fixed_point_iteration_solver<Variable>>& iteration_logger) const {
        iteration_logger.template append<index_type>(
            "Iter.", &this_type::iterations);
        iteration_logger.template append<scalar_type>(
            "Residual", &this_type::residual_norm);
        iteration_logger.template append<scalar_type>(
            "Reduction", &this_type::residual_reduction_rate);
        iteration_logger.template append<scalar_type>(
            "Relax. Coeff.", &this_type::relaxation_coefficient);
    }

    /*!
     * \brief Determine if the algorithm converged.
     *
     * \return If the algorithm converged.
     *
     * This function assumes that init function has been called.
     */
    [[nodiscard]] auto is_converged() const noexcept -> bool {
        return residual_norm_ <= tolerance_rate_;
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return iterations_;
    }

    /*!
     * \brief Get the norm of the residual.
     *
     * \return Norm of the residual.
     */
    [[nodiscard]] auto residual_norm() const noexcept -> scalar_type {
        return residual_norm_;
    }

    /*!
     * \brief Get the reduction rate of the residual.
     *
     * \return Reduction rate of the residual.
     */
    [[nodiscard]] auto residual_reduction_rate() const noexcept -> scalar_type {
        return residual_reduction_rate_;
    }

    /*!
     * \brief Get the relaxation coefficient.
     *
     * \return Relaxation coefficient.
     */
    [[nodiscard]] auto relaxation_coefficient() const noexcept -> scalar_type {
        return relaxation_coefficient_;
    }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<variable_type>& val)
        -> fixed_point_iteration_solver& {
        tolerances_ = val;
        return *this;
    }

    /*!
     * \brief Set the tolerance rate.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerance_rate(scalar_type val) -> fixed_point_iteration_solver& {
        NUM_COLLECT_PRECONDITION(val > static_cast<scalar_type>(0),
            "Tolerance rate must be positive.");
        tolerance_rate_ = val;
        return *this;
    }

    /*!
     * \brief Set the maximum number of iterations.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_iterations(index_type val) -> fixed_point_iteration_solver& {
        NUM_COLLECT_PRECONDITION(
            val > 0, "Maximum number of iterations must be positive.");
        max_iterations_ = val;
        return *this;
    }

    /*!
     * \brief Set the initial relaxation coefficient.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto initial_relaxation_coefficient(scalar_type val)
        -> fixed_point_iteration_solver& {
        NUM_COLLECT_PRECONDITION(val > static_cast<scalar_type>(0),
            "Initial relaxation coefficient must be positive.");
        initial_relaxation_coefficient_ = val;
        return *this;
    }

    /*!
     * \brief Set the maximum relaxation coefficient.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_relaxation_coefficient(scalar_type val)
        -> fixed_point_iteration_solver& {
        NUM_COLLECT_PRECONDITION(val > static_cast<scalar_type>(0),
            "Maximum relaxation coefficient must be positive.");
        max_relaxation_coefficient_ = val;
        return *this;
    }

    /*!
     * \brief Set the minimum relaxation coefficient.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto min_relaxation_coefficient(scalar_type val)
        -> fixed_point_iteration_solver& {
        NUM_COLLECT_PRECONDITION(val > static_cast<scalar_type>(0),
            "Minimum relaxation coefficient must be positive.");
        min_relaxation_coefficient_ = val;
        return *this;
    }

    /*!
     * \brief Set the rate of reduction of relaxation coefficient when the error
     * increases.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto relaxation_coefficient_reduction_rate(scalar_type val)
        -> fixed_point_iteration_solver& {
        NUM_COLLECT_PRECONDITION(val > static_cast<scalar_type>(0),
            "Relaxation coefficient reduction rate must be positive.");
        NUM_COLLECT_PRECONDITION(val < static_cast<scalar_type>(1),
            "Relaxation coefficient reduction rate must be less than 1.");
        relaxation_coefficient_reduction_rate_ = val;
        return *this;
    }

    /*!
     * \brief Set the rate of increase of relaxation coefficient when the error
     * decreases.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto relaxation_coefficient_increase_rate(scalar_type val)
        -> fixed_point_iteration_solver& {
        NUM_COLLECT_PRECONDITION(val > static_cast<scalar_type>(1),
            "Relaxation coefficient increase rate must be greater than 1.");
        relaxation_coefficient_increase_rate_ = val;
        return *this;
    }

private:
    /*!
     * \brief Initialize the iteration.
     *
     * \tparam Function Type of the function to calculate the right-hand-side.
     * \param[in] function Function to calculate the right-hand-side.
     * \param[in] solution Initial solution. This function won't modify this
     * variable.
     * \param[in] tolerance_reference Reference variable for calculating error
     * tolerances.
     */
    template <std::invocable<const variable_type& /*input*/,
        variable_type& /*output*/>
            Function>
    void init(Function&& function, const variable_type& solution,
        const variable_type& tolerance_reference) {
        residual_.resize(solution.size());
        function(solution, residual_);
        residual_ -= solution;
        residual_norm_ = tolerances_.calc_norm(tolerance_reference, residual_);
        iterations_ = 0;
        relaxation_coefficient_ = initial_relaxation_coefficient_;
        residual_reduction_rate_ =
            std::numeric_limits<scalar_type>::quiet_NaN();

        // These variables are given values in iterations, so this function only
        // acquires the memory.
        non_relaxed_update_.resize(solution.size());
        previous_solution_.resize(solution.size());
    }

    /*!
     * \brief Perform one iteration.
     *
     * \tparam Function Type of the function to calculate the right-hand-side.
     * \param[in] function Function to calculate the right-hand-side.
     * \param[in,out] solution Solution vector.
     * \param[in] tolerance_reference Reference variable for calculating error
     * tolerances.
     */
    template <std::invocable<const variable_type& /*input*/,
        variable_type& /*output*/>
            Function>
    void step(Function&& function, variable_type& solution,
        const variable_type& tolerance_reference) {
        previous_solution_ = solution;
        previous_residual_norm_ = residual_norm_;

        non_relaxed_update_ = residual_;
        solution =
            previous_solution_ + relaxation_coefficient_ * non_relaxed_update_;
        function(solution, residual_);
        residual_ -= solution;
        residual_norm_ = tolerances_.calc_norm(tolerance_reference, residual_);
        if (residual_norm_ > previous_residual_norm_) {
            while (residual_norm_ > previous_residual_norm_) {
                relaxation_coefficient_ *=
                    relaxation_coefficient_reduction_rate_;
                if (relaxation_coefficient_ < min_relaxation_coefficient_) {
                    NUM_COLLECT_ODE_THROW_LINEAR_SOLVER_FAILURE(this->logger(),
                        "Failed to solve an equation even with the minimum "
                        "relaxation coefficient.");
                }
                solution = previous_solution_ +
                    relaxation_coefficient_ * non_relaxed_update_;
                function(solution, residual_);
                residual_ -= solution;
                residual_norm_ =
                    tolerances_.calc_norm(tolerance_reference, residual_);
            }
        } else {
            relaxation_coefficient_ *= relaxation_coefficient_increase_rate_;
            if (relaxation_coefficient_ > max_relaxation_coefficient_) {
                relaxation_coefficient_ = max_relaxation_coefficient_;
            }
        }
        residual_reduction_rate_ = residual_norm_ / previous_residual_norm_;
        ++iterations_;
    }

    //! Current residual.
    variable_type residual_{};

    //! Update without relaxation.
    variable_type non_relaxed_update_{};

    //! Previous solution.
    variable_type previous_solution_{};

    //! Number of iterations.
    index_type iterations_{};

    //! Norm of the residual.
    scalar_type residual_norm_{};

    //! Previous norm of the residual.
    scalar_type previous_residual_norm_{};

    //! Reduction rate of the residual.
    scalar_type residual_reduction_rate_{};

    //! Relaxation coefficient.
    scalar_type relaxation_coefficient_{};

    //! Error tolerances.
    error_tolerances<variable_type> tolerances_{};

    //! Default tolerance rate.
    static constexpr auto default_tolerance_rate =
        static_cast<scalar_type>(1e-4);

    //! Tolerance rate.
    scalar_type tolerance_rate_{default_tolerance_rate};

    //! Default maximum number of iterations.
    static constexpr index_type default_max_iterations = 1000;

    //! Maximum number of iterations.
    index_type max_iterations_{default_max_iterations};

    //! Default initial relaxation coefficient.
    static constexpr auto default_initial_relaxation_coefficient =
        static_cast<scalar_type>(1.0);

    //! Initial relaxation coefficient.
    scalar_type initial_relaxation_coefficient_{
        default_initial_relaxation_coefficient};

    //! Default maximum relaxation coefficient.
    static constexpr auto default_max_relaxation_coefficient =
        static_cast<scalar_type>(1.0);

    //! Maximum relaxation coefficient.
    scalar_type max_relaxation_coefficient_{default_max_relaxation_coefficient};

    //! Default minimum relaxation coefficient.
    static constexpr auto default_min_relaxation_coefficient =
        functions::sqrt(std::numeric_limits<scalar_type>::epsilon());

    //! Minimum relaxation coefficient.
    scalar_type min_relaxation_coefficient_{default_min_relaxation_coefficient};

    //! Default rate of reduction of relaxation coefficient when the error increases.
    static constexpr auto default_relaxation_coefficient_reduction_rate =
        static_cast<scalar_type>(0.5);

    //! Rate of reduction of relaxation coefficient when the error increases.
    scalar_type relaxation_coefficient_reduction_rate_{
        default_relaxation_coefficient_reduction_rate};

    //! Default rate of increase of relaxation coefficient when the error decreases.
    static constexpr auto default_relaxation_coefficient_increase_rate =
        static_cast<scalar_type>(1.05);

    //! Rate of increase of relaxation coefficient when the error decreases.
    scalar_type relaxation_coefficient_increase_rate_{
        default_relaxation_coefficient_increase_rate};
};

}  // namespace num_collect::ode::impl
