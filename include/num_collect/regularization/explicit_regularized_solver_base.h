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

#include <utility>

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
     * \brief Get the default region to search for the optimal regularization
     * parameter.
     *
     * \return Pair of minimum and maximum regularization parameters.
     */
    [[nodiscard]] auto param_search_region() const
        -> std::pair<scalar_type, scalar_type> {
        return derived().param_search_region();
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
