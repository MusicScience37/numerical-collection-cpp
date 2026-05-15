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
 * \brief Definition of functional_bicgstab class.
 */
#pragma once

// IWYU pragma: no_include <Eigen/SparseCore>

#include <cmath>
#include <concepts>
#include <limits>
#include <optional>

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/logging/logging_mixin.h"

namespace num_collect::linear {

//! Log tag.
constexpr auto functional_bicgstab_tag =
    logging::log_tag_view("num_collect::linear::functional_bicgstab");

/*!
 * \brief Class to solve linear equations using BiCGstab \cite Golub2013
 * using functions to multiply coefficient matrix.
 *
 * \tparam Vector Type of vectors.
 */
template <base::concepts::real_scalar_dense_vector Vector>
class functional_bicgstab : public logging::logging_mixin {
public:
    //! Type of vectors.
    using vector_type = Vector;

    //! Type of scalars.
    using scalar_type = typename vector_type::Scalar;

    //! Constructor.
    functional_bicgstab() : logging::logging_mixin(functional_bicgstab_tag) {}

    /*!
     * \brief Prepare the preconditioner.
     *
     * \param[in] coeff_diagonal Diagonal coefficients of the coefficient
     * matrix.
     */
    void prepare_preconditioner(const vector_type& coeff_diagonal) {
        if (!preconditioner_diagonal_) {
            preconditioner_diagonal_.emplace();
        }
        preconditioner_diagonal_->resize(coeff_diagonal.size());

        const scalar_type max_coeff = coeff_diagonal.cwiseAbs().maxCoeff();
        const scalar_type coeff_thresh = tolerance_ * max_coeff;
        for (index_type i = 0; i < coeff_diagonal.size(); ++i) {
            const scalar_type current_coeff = coeff_diagonal(i);
            using std::abs;
            if (abs(current_coeff) > coeff_thresh) {
                (*preconditioner_diagonal_)(i) =
                    static_cast<scalar_type>(1) / current_coeff;
            } else {
                (*preconditioner_diagonal_)(i) = static_cast<scalar_type>(1);
            }
        }
    }

    /*!
     * \brief Solve.
     *
     * coeff_function is assumed to have a signature like:
     *
     * ~~~{.cpp}
     * void coeff_function(const variable_type& target, variable_type& result);
     * ~~~
     *
     * \tparam CoeffFunction Type of the function to multiply coefficient
     * matrix.
     * \param[in] coeff_function Function to multiply coefficient matrix.
     * \param[in] rhs Right-hand-side vector.
     * \param[in,out] solution Solution. (Given vector is used as the initial
     * solution.)
     */
    template <std::invocable<const vector_type&, vector_type&> CoeffFunction>
    void solve(CoeffFunction&& coeff_function, const vector_type& rhs,
        vector_type& solution) {
        using std::abs;

        iterations_ = 0;
        initialize(coeff_function, rhs, solution);

        const scalar_type rhs_norm = rhs.norm();
        const scalar_type absolute_tolerance = rhs_norm * tolerance_;
        constexpr scalar_type epsilon =
            std::numeric_limits<scalar_type>::epsilon();

        scalar_type residual_norm = residual_.norm();
        if (residual_norm <= absolute_tolerance) {
            NUM_COLLECT_LOG_TRACE(this->logger(),
                "No iteration needed. residual_norm={}", residual_norm);
            return;
        }

        while (true) {
            if (preconditioner_diagonal_) {
                preconditioner_buffer_ =
                    p_.cwiseProduct(*preconditioner_diagonal_);
                coeff_function(preconditioner_buffer_, ap_);
            } else {
                coeff_function(p_, ap_);
            }
            const scalar_type r0_ap_dot = r0_.dot(ap_);
            if (abs(r0_ap_dot) <
                r0_.stableNorm() * ap_.stableNorm() * epsilon) {
                // Restart with random r0.
                coeff_function(solution, residual_);
                residual_ = rhs - residual_;
                r0_.setRandom();
                p_ = residual_;
                rho_ = r0_.dot(residual_);
                ++iterations_;  // Add iteration count to prevent infinite loop.
                continue;
            }
            const scalar_type mu = rho_ / r0_ap_dot;
            if (preconditioner_diagonal_) {
                solution += mu * preconditioner_buffer_;
            } else {
                solution += mu * p_;
            }
            residual_ -= mu * ap_;  // s in reference.

            residual_norm = residual_.norm();
            if (residual_norm <= absolute_tolerance) {
                NUM_COLLECT_LOG_TRACE(this->logger(),
                    "Finished iterations: iterations={}, residual_norm={}",
                    iterations_, residual_norm);
                return;
            }

            if (preconditioner_diagonal_) {
                preconditioner_buffer_ =
                    residual_.cwiseProduct(*preconditioner_diagonal_);
                coeff_function(preconditioner_buffer_, as_);
            } else {
                coeff_function(residual_, as_);
            }
            const scalar_type as_norm2 = as_.squaredNorm();
            if (abs(as_norm2) < std::numeric_limits<scalar_type>::min()) {
                initialize(coeff_function, rhs, solution);
                ++iterations_;
                continue;
            }
            const scalar_type omega = residual_.dot(as_) / as_norm2;
            if (preconditioner_diagonal_) {
                solution += omega * preconditioner_buffer_;
            } else {
                solution += omega * residual_;
            }
            residual_ -= omega * as_;  // r in reference.

            residual_norm = residual_.norm();
            if (residual_norm <= absolute_tolerance ||
                iterations_ >= max_iterations_) {
                NUM_COLLECT_LOG_TRACE(this->logger(),
                    "Finished iterations: iterations={}, residual_norm={}",
                    iterations_, residual_norm);
                return;
            }
            NUM_COLLECT_LOG_TRACE(this->logger(),
                "iterations={}, residual_norm={}", iterations_, residual_norm);

            const scalar_type rho_old = rho_;
            rho_ = r0_.dot(residual_);
            const scalar_type tau = rho_ * mu / (rho_old * omega);
            p_ = residual_ + tau * (p_ - omega * ap_);

            ++iterations_;
        }
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
     * \brief Set the maximum number of iterations.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_iterations(index_type val) -> functional_bicgstab& {
        NUM_COLLECT_PRECONDITION(val > 0, "max_iterations must be positive.");
        max_iterations_ = val;
        return *this;
    }

    /*!
     * \brief Set the relative tolerance.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerance(scalar_type val) -> functional_bicgstab& {
        NUM_COLLECT_PRECONDITION(val <= static_cast<scalar_type>(1),
            "tolerance must be less than or equal to 1.");
        NUM_COLLECT_PRECONDITION(
            val >= std::numeric_limits<scalar_type>::epsilon(),
            "tolerance must be greater than or equal to machine epsilon.");
        tolerance_ = val;
        return *this;
    }

private:
    /*!
     * \brief Initialize.
     *
     * \tparam CoeffFunction Type of the function to multiply coefficient
     * matrix.
     * \param[in] coeff_function Function to multiply coefficient matrix.
     * \param[in] rhs Right-hand-side vector.
     * \param[in] solution Solution. (Given vector is used as the initial
     * solution.)
     */
    template <std::invocable<const vector_type&, vector_type&> CoeffFunction>
    void initialize(CoeffFunction&& coeff_function, const vector_type& rhs,
        const vector_type& solution) {
        coeff_function(solution, residual_);
        residual_ = rhs - residual_;
        r0_ = residual_;
        p_ = residual_;
        rho_ = r0_.dot(residual_);
    }

    //! Number of iterations.
    index_type iterations_{};

    //! Default maximum number of iterations.
    static constexpr index_type default_max_iterations = 1000;

    //! Maximum number of iterations.
    index_type max_iterations_{default_max_iterations};

    //! Relative tolerance.
    scalar_type tolerance_{Eigen::NumTraits<scalar_type>::dummy_precision()};

    //! Residual.
    vector_type residual_{};

    /*!
     * \name Intermediate variables as in notation of \cite Golub2013.
     */
    ///@{
    //! Intermediate variable.
    vector_type r0_{};
    vector_type p_{};
    scalar_type rho_{};
    vector_type ap_{};
    vector_type as_{};
    ///@}

    //! Diagonal coefficients of preconditioner.
    std::optional<vector_type> preconditioner_diagonal_{};

    //! Buffer of the vector to apply preconditioner.
    vector_type preconditioner_buffer_{};
};

}  // namespace num_collect::linear
