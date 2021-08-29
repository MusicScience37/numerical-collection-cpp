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
 * \brief Definition of explicit_regularized_solver_base class.
 */
#pragma once

#include <cmath>
#include <utility>

#include "num_collect/util/index_type.h"

namespace num_collect::regularization {

/*!
 * \brief Base class of solvers using explicit formulas for regularization.
 *
 * \tparam Derived Type of derived class.
 * \tparam Scalar Type of scalars.
 */
template <typename Derived, typename Scalar>
class explicit_regularized_solver_base {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Solve for a regularization parameter.
     *
     * \tparam Solution Type of the solution.
     * \param[in] param Regularization parameter.
     * \param[out] solution Solution.
     */
    template <typename Solution>
    void solve(const scalar_type& param, Solution& solution) const {
        return derived().solve(param, solution);
    }

    /*!
     * \brief Calculate the squared norm of the residual.
     *
     * \param[in] param Regularization parameter.
     * \return Result.
     */
    [[nodiscard]] auto residual_norm(const scalar_type& param) const
        -> scalar_type {
        return derived().residual_norm(param);
    }

    /*!
     * \brief Calculate the regularization term.
     *
     * \param[in] param Regularization parameter.
     * \return Result.
     */
    [[nodiscard]] auto regularization_term(const scalar_type& param) const
        -> scalar_type {
        return derived().regularization_term(param);
    }

    /*!
     * \brief Calculate the first-order derivative of the squared norm of the
     * residual.
     *
     * \param[in] param Regularization parameter.
     * \return Result.
     */
    [[nodiscard]] auto first_derivative_of_residual_norm(
        const scalar_type& param) const -> scalar_type {
        return derived().first_derivative_of_residual_norm(param);
    }

    /*!
     * \brief Calculate the first-order derivative of the Regularization term.
     *
     * \param[in] param Regularization parameter.
     * \return Result.
     */
    [[nodiscard]] auto first_derivative_of_regularization_term(
        const scalar_type& param) const -> scalar_type {
        return derived().first_derivative_of_regularization_term(param);
    }

    /*!
     * \brief Calculate the second-order derivative of the squared norm of the
     * residual.
     *
     * \param[in] param Regularization parameter.
     * \return Result.
     */
    [[nodiscard]] auto second_derivative_of_residual_norm(
        const scalar_type& param) const -> scalar_type {
        return derived().second_derivative_of_residual_norm(param);
    }

    /*!
     * \brief Calculate the send-order derivative of the Regularization term.
     *
     * \param[in] param Regularization parameter.
     * \return Result.
     */
    [[nodiscard]] auto second_derivative_of_regularization_term(
        const scalar_type& param) const -> scalar_type {
        return derived().second_derivative_of_regularization_term(param);
    }

    /*!
     * \brief Calculate the sum of filter factors.
     *
     * \param[in] param Regularization parameter.
     * \return Result.
     */
    [[nodiscard]] auto sum_of_filter_factor(const scalar_type& param) const
        -> scalar_type {
        return derived().sum_of_filter_factor(param);
    }

    /*!
     * \brief Get the size of data.
     *
     * \return Size of data.
     */
    [[nodiscard]] auto data_size() const -> index_type {
        return derived().data_size();
    }

    /*!
     * \brief Get the default region to search for the optimal regularization
     * parameter.
     *
     * \return Pair of minimum and maximum regularization parameters.
     */
    [[nodiscard]] auto param_search_region() const
        -> std::pair<scalar_type, scalar_type> {
        return derived().param_search_region();
    }

    /*!
     * \brief Calculate the curvature of L-curve.
     *
     * \param[in] param Regularization parameter.
     * \return Curvature of L-curve.
     */
    [[nodiscard]] auto curvature(const scalar_type& param) const {
        const scalar_type res = residual_norm(param);
        const scalar_type reg = regularization_term(param);
        const scalar_type res1 = first_derivative_of_residual_norm(param);
        const scalar_type res2 = second_derivative_of_residual_norm(param);
        const scalar_type reg1 = first_derivative_of_regularization_term(param);
        const scalar_type reg2 =
            second_derivative_of_regularization_term(param);

        const scalar_type log_res1 = res1 / res;
        const scalar_type log_reg1 = reg1 / reg;
        const scalar_type log_res2 = (res2 * res - res1 * res1) / (res * res);
        const scalar_type log_reg2 = (reg2 * reg - reg1 * reg1) / (reg * reg);

        using std::pow;
        return (log_res1 * log_reg2 - log_res2 * log_reg1) /
            pow(log_res1 * log_res1 + log_reg1 * log_reg1,
                static_cast<scalar_type>(1.5));  // NOLINT
    }

    /*!
     * \brief Calculate GCV function.
     *
     * \param[in] param Regularization parameter.
     * \return Value.
     */
    [[nodiscard]] auto gcv(const scalar_type& param) const -> scalar_type {
        const scalar_type den =
            static_cast<scalar_type>(data_size()) - sum_of_filter_factor(param);
        return residual_norm(param) / (den * den);
    }

protected:
    /*!
     * \brief Access derived object.
     *
     * \return Reference to the derived object.
     */
    [[nodiscard]] auto derived() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }

    /*!
     * \brief Access derived object.
     *
     * \return Reference to the derived object.
     */
    [[nodiscard]] auto derived() const noexcept -> const Derived& {
        return *static_cast<const Derived*>(this);
    }
};

}  // namespace num_collect::regularization
