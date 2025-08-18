/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of operator_conjugate_gradient class.
 */
#pragma once

#include <concepts>
#include <limits>

#include <Eigen/Core>

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/logging/logging_mixin.h"

namespace num_collect::linear::impl {

//! Log tag of operator_conjugate_gradient.
constexpr auto operator_conjugate_gradient_tag = logging::log_tag_view(
    "num_collect::linear::impl::operator_conjugate_gradient");

/*!
 * \brief Class to perform conjugate gradient (CG) method \cite Golub2013 for
 * linear operators.
 *
 * \tparam Vector Type of vectors.
 */
template <base::concepts::dense_vector Vector>
class operator_conjugate_gradient : public logging::logging_mixin {
public:
    //! Type of vectors.
    using vector_type = Vector;

    //! Type of scalars.
    using scalar_type = typename vector_type::Scalar;

    /*!
     * \brief Constructor.
     */
    operator_conjugate_gradient()
        : logging::logging_mixin(operator_conjugate_gradient_tag) {}

    /*!
     * \brief Solve.
     *
     * coeff_function is assumed to have a signature like:
     *
     * ~~~{.cpp}
     * void coeff_function(const vector_type& target, vector_type& result);
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
        iterations_ = 0;

        coeff_function(solution, residual_);
        residual_ = rhs - residual_;
        const scalar_type tolerance =
            tolerance_rate_ * tolerance_rate_ * rhs.squaredNorm();
        NUM_COLLECT_LOG_TRACE(this->logger(), "tolerance={}", tolerance);
        scalar_type residual_norm = residual_.squaredNorm();
        if (residual_norm <= tolerance) {
            NUM_COLLECT_LOG_SUMMARY(this->logger(),
                "No iteration needed. residual_norm={}", residual_norm);
            return;
        }
        p_ = residual_;
        while (iterations_ < max_iterations_) {
            coeff_function(p_, coeff_p_);
            const scalar_type alpha = residual_.squaredNorm() /
                (p_.dot(coeff_p_) +
                    std::numeric_limits<scalar_type>::epsilon());
            solution += alpha * p_;
            previous_residual_ = residual_;
            residual_ -= alpha * coeff_p_;
            residual_norm = residual_.squaredNorm();
            if (residual_norm <= tolerance) {
                break;
            }
            const scalar_type beta = residual_norm /
                (previous_residual_.squaredNorm() +
                    std::numeric_limits<scalar_type>::epsilon());
            p_ = residual_ + beta * p_;
            ++iterations_;
        }
        NUM_COLLECT_LOG_SUMMARY(this->logger(),
            "Finished iterations: iterations={}, residual_norm={}", iterations_,
            residual_norm);
    }

    /*!
     * \brief Set the rate of tolerances.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tolerance_rate(const scalar_type& val)
        -> operator_conjugate_gradient& {
        tolerance_rate_ = val;
        return *this;
    }

    /*!
     * \brief Set the maximum number of iterations.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_iterations(const index_type& val) -> operator_conjugate_gradient& {
        max_iterations_ = val;
        return *this;
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    auto iterations() -> index_type { return iterations_; }

private:
    //! Number of iterations.
    index_type iterations_{};

    //! Default maximum number of iterations.
    static constexpr index_type default_max_iterations = 1000;

    //! Maximum number of iterations.
    index_type max_iterations_{default_max_iterations};

    //! Default rate of tolerance.
    static constexpr scalar_type default_tolerance_rate =
        Eigen::NumTraits<scalar_type>::dummy_precision();

    //! Rate of tolerance.
    scalar_type tolerance_rate_{default_tolerance_rate};

    //! Residual vector.
    vector_type residual_{};

    /*!
     * \name Intermidiate variables.
     */
    ///@{
    //! Intermediate variable.
    vector_type previous_residual_{};
    vector_type p_{};
    vector_type coeff_p_{};
    ///@}
};

}  // namespace num_collect::linear::impl
