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
 * \brief Definition of bicgstab class.
 */
#pragma once

// IWYU pragma: no_include <Eigen/Core>
// IWYU pragma: no_include <Eigen/SparseCore>

#include <cmath>
#include <limits>
#include <string_view>

#include "num_collect/base/concepts/invocable.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar_dense_vector.h"  // IWYU pragma: keep
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/ode/error_tolerances.h"

namespace num_collect::ode::impl {

//! Log tag.
constexpr auto bicgstab_tag =
    logging::log_tag_view("num_collect::ode::impl::bicgstab");

/*!
 * \brief Class to solve linear equations using BiCGstab \cite Golub2013.
 *
 * \tparam Vector Type of vectors.
 */
template <base::concepts::real_scalar_dense_vector Vector>
class bicgstab : public logging::logging_mixin {
public:
    //! Type of vectors.
    using vector_type = Vector;

    //! Type of scalars.
    using scalar_type = typename vector_type::Scalar;

    //! Type of matrices.
    using matrix_type = Eigen::MatrixX<scalar_type>;

    //! Constructor.
    bicgstab() : logging::logging_mixin(bicgstab_tag) {}

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
    template <base::concepts::invocable<const vector_type&, vector_type&>
            CoeffFunction>
    void solve(CoeffFunction&& coeff_function, const vector_type& rhs,
        vector_type& solution) {
        using std::abs;

        iterations_ = 0;
        initialize(coeff_function, rhs, solution);

        scalar_type residual_norm = tolerances_.calc_norm(rhs, residual_);
        if (residual_norm <= tolerance_rate_) {
            this->logger().trace()(
                "No iteration needed. residual_norm={}", residual_norm);
            return;
        }

        while (true) {
            coeff_function(p_, ap_);
            const scalar_type as_dot = r0_.dot(ap_);
            if (abs(as_dot) < std::numeric_limits<scalar_type>::min()) {
                this->logger().warning()("No further iteration can be done.");
                return;
            }
            const scalar_type mu = rho_ / r0_.dot(ap_);
            residual_ -= mu * ap_;  // s in reference.

            coeff_function(residual_, as_);
            const scalar_type as_norm2 = as_.squaredNorm();
            solution += mu * p_;
            if (abs(as_norm2) < std::numeric_limits<scalar_type>::min()) {
                initialize(coeff_function, rhs, solution);
                ++iterations_;
                continue;
            }
            const scalar_type omega = residual_.dot(as_) / as_norm2;
            solution += omega * residual_;
            residual_ -= omega * as_;  // r in reference.

            residual_norm = tolerances_.calc_norm(rhs, residual_);
            if (residual_norm <= tolerance_rate_ ||
                iterations_ >= max_iterations_) {
                this->logger().trace()(
                    "Finished iterations: iterations={}, residual_norm={}",
                    iterations_, residual_norm);
                return;
            }

            const scalar_type rho_old = rho_;
            rho_ = r0_.dot(residual_);
            const scalar_type tau = rho_ * mu / (rho_old * omega);
            p_ = residual_ + tau * (p_ - omega * ap_);

            ++iterations_;
        }
    }

    /*!
     * \brief Set the error tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerances(const error_tolerances<vector_type>& val) -> bicgstab& {
        tolerances_ = val;
        return *this;
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    auto iterations() -> index_type { return iterations_; }

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
    template <base::concepts::invocable<const vector_type&, vector_type&>
            CoeffFunction>
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

    //! Tolerances.
    error_tolerances<vector_type> tolerances_{};

    //! Default rate of tolerance in this solver.
    static constexpr auto default_tolerance_rate =
        static_cast<scalar_type>(1e-2);

    //! Rate of tolerance in this solver.
    scalar_type tolerance_rate_{default_tolerance_rate};

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
};

}  // namespace num_collect::ode::impl
