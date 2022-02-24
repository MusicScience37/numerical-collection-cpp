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
 * \brief Definition of implicit_regularized_solver_base class.
 */
#pragma once

#include <cmath>
#include <utility>

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logger.h"

namespace num_collect::regularization {

/*!
 * \brief Base class of solvers using implicit formulas for regularization.
 *
 * \tparam Derived Type of derived class.
 * \tparam Data Type of data.
 */
template <typename Derived, base::concepts::dense_matrix Data>
class implicit_regularized_solver_base {
public:
    //! Type of data.
    using data_type = Data;

    //! Type of scalars.
    using scalar_type =
        typename Eigen::NumTraits<typename data_type::Scalar>::Real;

    /*!
     * \brief Solve for a regularization parameter.
     *
     * \param[in] param Regularization parameter.
     * \param[in,out] solution Solution. (Assumed to be the initial solution.)
     */
    void solve(const scalar_type& param, data_type& solution) {
        return derived().solve(param, solution);
    }

    /*!
     * \brief Calculate the squared norm of the residual.
     *
     * \param[in] param Regularization parameter.
     * \param[in] solution Solution.
     * \return Result.
     */
    [[nodiscard]] auto residual_norm(const scalar_type& param,
        const data_type& solution) const -> scalar_type {
        return derived().residual_norm(param, solution);
    }

    /*!
     * \brief Calculate the regularization term.
     *
     * \param[in] param Regularization parameter.
     * \param[in] solution Solution.
     * \return Result.
     */
    [[nodiscard]] auto regularization_term(const scalar_type& param,
        const data_type& solution) const -> scalar_type {
        return derived().regularization_term(param, solution);
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

protected:
    /*!
     * \brief Construct.
     *
     * \param[in] tag Log tag.
     */
    explicit implicit_regularized_solver_base(logging::log_tag_view tag)
        : logger_(tag) {}

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

    /*!
     * \brief Access the logger.
     *
     * \return Logger.
     */
    [[nodiscard]] auto logger() const noexcept -> const logging::logger& {
        return logger_;
    }

private:
    //! Logger.
    logging::logger logger_;
};

}  // namespace num_collect::regularization
