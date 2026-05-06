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
 * \brief Definition of inexact_newton_forcing_term_calculator class.
 */
#pragma once

#include <algorithm>
#include <limits>
#include <optional>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/precondition.h"
#include "num_collect/util/assert.h"

namespace num_collect::ode::runge_kutta::impl {

/*!
 * \brief Class to calculate a forcing term for inexact Newton method
 * \cite Eisenstat1996, \cite Blom2013.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class inexact_newton_forcing_term_calculator {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    //! Constructor.
    inexact_newton_forcing_term_calculator() = default;

    /*!
     * \brief Reset the calculator.
     */
    void reset() {
        previous_residual_norm_.reset();
        previous_forcing_term_.reset();
    }

    /*!
     * \brief Calculate the forcing term.
     *
     * \param[in] residual_norm Norm of the residual before the update.
     * \return Forcing term.
     *
     * \note This algorithm assumes that the residual is not zero since no
     * update is needed if the residual is zero.
     */
    [[nodiscard]] auto calculate(scalar_type residual_norm) -> scalar_type {
        NUM_COLLECT_DEBUG_ASSERT(residual_norm > static_cast<scalar_type>(0));

        scalar_type forcing_term{};

        if (previous_residual_norm_ && previous_forcing_term_) {
            // Initial guess without safeguard.
            const scalar_type residual_reduction_rate =
                residual_norm / *previous_residual_norm_;
            forcing_term = forcing_term_rate_ * residual_reduction_rate *
                residual_reduction_rate;

            // Safeguard written in the original paper to prevent too small
            // forcing terms.
            const scalar_type forcing_term_from_previous_step =
                forcing_term_rate_ * (*previous_forcing_term_) *
                (*previous_forcing_term_);
            // The threshold written in the original paper as a fixed value.
            constexpr scalar_type
                forcing_term_from_previous_step_applied_threshold =
                    static_cast<scalar_type>(0.1);
            if (forcing_term_from_previous_step >
                forcing_term_from_previous_step_applied_threshold) {
                forcing_term =
                    std::max(forcing_term, forcing_term_from_previous_step);
            }
        } else {
            // Initial guess.
            forcing_term = initial_forcing_term_;
        }

        // Safeguard to prevent over-solving.
        if (absolute_tolerance_) {
            // The rate written in the original paper as a fixed value.
            constexpr scalar_type absolute_tolerance_rate =
                static_cast<scalar_type>(0.5);
            forcing_term = std::max(forcing_term,
                absolute_tolerance_rate * (*absolute_tolerance_) /
                    residual_norm);
        }

        // Apply the range of the forcing term.
        forcing_term =
            std::clamp(forcing_term, min_forcing_term_, max_forcing_term_);

        // Update the previous values for the next iteration.
        previous_residual_norm_ = residual_norm;
        previous_forcing_term_ = forcing_term;

        return forcing_term;
    }

    /*!
     * \brief Set the rate of the forcing term.
     *
     * \param[in] value Rate of the forcing term.
     * \return This.
     */
    auto forcing_term_rate(scalar_type value)
        -> inexact_newton_forcing_term_calculator& {
        NUM_COLLECT_PRECONDITION(value > static_cast<scalar_type>(0),
            "The rate of the forcing term must be positive.");
        NUM_COLLECT_PRECONDITION(value <= static_cast<scalar_type>(1),
            "The rate of the forcing term must be less than or equal to 1.");
        forcing_term_rate_ = value;
        return *this;
    }

    /*!
     * \brief Set the range of the forcing term.
     *
     * \param[in] min_value Minimum value of the forcing term.
     * \param[in] max_value Maximum value of the forcing term.
     * \return This.
     */
    auto forcing_term_range(scalar_type min_value, scalar_type max_value)
        -> inexact_newton_forcing_term_calculator& {
        NUM_COLLECT_PRECONDITION(
            min_value > std::numeric_limits<scalar_type>::epsilon(),
            "The minimum value of the forcing term must be greater than the "
            "machine epsilon.");
        NUM_COLLECT_PRECONDITION(max_value > min_value,
            "The maximum value of the forcing term must be greater than the "
            "minimum value.");
        NUM_COLLECT_PRECONDITION(max_value < static_cast<scalar_type>(1),
            "The maximum value of the forcing term must be less than 1.");
        min_forcing_term_ = min_value;
        max_forcing_term_ = max_value;
        return *this;
    }

    /*!
     * \brief Set the maximum value of the forcing term.
     *
     * \param[in] value Maximum value of the forcing term.
     * \return This.
     */
    auto max_forcing_term(scalar_type value)
        -> inexact_newton_forcing_term_calculator& {
        NUM_COLLECT_PRECONDITION(value > min_forcing_term_,
            "The maximum value of the forcing term must be greater than the "
            "minimum value.");
        NUM_COLLECT_PRECONDITION(value < static_cast<scalar_type>(1),
            "The maximum value of the forcing term must be less than 1.");
        max_forcing_term_ = value;
        return *this;
    }

    /*!
     * \brief Set the minimum value of the forcing term.
     *
     * \param[in] value Minimum value of the forcing term.
     * \return This.
     */
    auto min_forcing_term(scalar_type value)
        -> inexact_newton_forcing_term_calculator& {
        NUM_COLLECT_PRECONDITION(
            value > std::numeric_limits<scalar_type>::epsilon(),
            "The minimum value of the forcing term must be greater than the "
            "machine epsilon.");
        NUM_COLLECT_PRECONDITION(value < max_forcing_term_,
            "The minimum value of the forcing term must be less than the "
            "maximum value.");
        min_forcing_term_ = value;
        return *this;
    }

    /*!
     * \brief Set the absolute tolerance for the residual norm.
     *
     * \param[in] value Absolute tolerance for the residual norm.
     * \return This.
     */
    auto absolute_tolerance(scalar_type value)
        -> inexact_newton_forcing_term_calculator& {
        NUM_COLLECT_PRECONDITION(value > static_cast<scalar_type>(0),
            "The absolute tolerance for the residual norm must be positive.");
        absolute_tolerance_ = value;
        return *this;
    }

    /*!
     * \brief Set the initial forcing term.
     *
     * \param[in] value Initial forcing term.
     * \return This.
     *
     * \note This value will be clamped by the range of the forcing term.
     */
    auto initial_forcing_term(scalar_type value)
        -> inexact_newton_forcing_term_calculator& {
        NUM_COLLECT_PRECONDITION(
            value > std::numeric_limits<scalar_type>::epsilon(),
            "The initial forcing term must be greater than the machine "
            "epsilon.");
        NUM_COLLECT_PRECONDITION(value < static_cast<scalar_type>(1),
            "The initial forcing term must be less than 1.");
        initial_forcing_term_ = value;
        return *this;
    }

private:
    //! Norm of the previous residual in the inexact Newton method.
    std::optional<scalar_type> previous_residual_norm_{};

    //! Previous forcing term.
    std::optional<scalar_type> previous_forcing_term_{};

    //! Default rate of the forcing term.
    static constexpr auto default_forcing_term_rate =
        static_cast<scalar_type>(0.9);

    //! Rate of the forcing term.
    scalar_type forcing_term_rate_{default_forcing_term_rate};

    /*!
     * \brief Default maximum value of the forcing term.
     *
     * \note The value in the original papers is 0.9, but 0.9 did not work in a
     * test with Kaps' problem without Jacobian.
     * 0.1 is written as "modestly accurate" in Eisenstat1996.
     */
    static constexpr auto default_max_forcing_term =
        static_cast<scalar_type>(0.1);

    //! Maximum value of the forcing term.
    scalar_type max_forcing_term_{default_max_forcing_term};

    //! Default minimum value of the forcing term.
    static constexpr auto default_min_forcing_term =
        static_cast<scalar_type>(1e-10);

    //! Minimum value of the forcing term.
    scalar_type min_forcing_term_{default_min_forcing_term};

    //! Absolute tolerance for the residual norm.
    std::optional<scalar_type> absolute_tolerance_{};

    /*!
     * \brief The default initial forcing term.
     *
     * \note The values in the original papers are 0.1 in Eisenstat1996 and 0.9
     * in Blom2013. However, 1e-4 worked well in an experiment. 1e-4 is written
     * as "close approximation" in Eisenstat1996.
     */
    static constexpr auto default_initial_forcing_term =
        static_cast<scalar_type>(1e-4);

    //! Initial forcing term.
    scalar_type initial_forcing_term_{default_initial_forcing_term};
};

}  // namespace num_collect::ode::runge_kutta::impl
