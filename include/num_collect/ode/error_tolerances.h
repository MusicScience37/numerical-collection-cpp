/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of error_tolerances class.
 */
#pragma once

#include <cmath>

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/util/assert.h"

namespace num_collect::ode {

namespace impl {

/*!
 * \brief Default tolerance of relative errors.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
constexpr Scalar default_tol_rel_error = static_cast<Scalar>(1e-4);

/*!
 * \brief Default tolerance of absolute errors.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
constexpr Scalar default_tol_abs_error = static_cast<Scalar>(1e-4);

}  // namespace impl

/*!
 * \brief Class of error tolerances \cite Hairer1993.
 *
 * \tparam Variable Type of variables.
 */
template <typename Variable>
class error_tolerances;

/*!
 * \brief Class of error tolerances \cite Hairer1993.
 *
 * \tparam Variable Type of variables.
 */
template <base::concepts::dense_vector Variable>
class error_tolerances<Variable> {
public:
    //! Type of variables.
    using variable_type = Variable;

    //! Type of scalars.
    using scalar_type = typename variable_type::Scalar;

    /*!
     * \brief Constructor.
     *
     * \param[in] reference Reference variable. (For determining the size.)
     */
    explicit error_tolerances(variable_type reference)
        : tol_rel_error_(variable_type::Constant(
              reference.size(), impl::default_tol_rel_error<scalar_type>)),
          tol_abs_error_(variable_type::Constant(
              reference.size(), impl::default_tol_abs_error<scalar_type>)),
          norm_weight_(std::sqrt(static_cast<scalar_type>(1) /
              static_cast<scalar_type>(reference.size()))) {}

    /*!
     * \brief Check whether the given error satisfies tolerances.
     *
     * \param[in] variable Variable used for the relative error.
     * \param[in] error Error.
     * \retval true Given error satisfies tolerances.
     * \retval false Given error doesn't satisfy tolerances.
     */
    [[nodiscard]] auto check(const variable_type& variable,
        const variable_type& error) const -> bool {
        return (error.array().abs() <=
            (tol_rel_error_.array() * variable.array().abs() +
                tol_abs_error_.array()))
            .all();
    }

    /*!
     * \brief Calculate the norm of the error determined by tolerances.
     *
     * \param[in] variable Variable used for the relative error.
     * \param[in] error Error.
     * \return Norm value.
     */
    [[nodiscard]] auto calc_norm(const variable_type& variable,
        const variable_type& error) const -> scalar_type {
        return norm_weight_ *
            (error.cwiseQuotient(
                 tol_rel_error_.cwiseProduct(variable.cwiseAbs()) +
                 tol_abs_error_))
                .norm();
    }

    /*!
     * \brief Set the tolerance of relative error.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_rel_error(const variable_type& val) -> error_tolerances& {
        NUM_COLLECT_ASSERT((val.array() >= static_cast<scalar_type>(0)).all());
        tol_rel_error_ = val;
        return *this;
    }

    /*!
     * \brief Set the tolerance of absolute error.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_abs_error(const variable_type& val) -> error_tolerances& {
        NUM_COLLECT_ASSERT((val.array() >= static_cast<scalar_type>(0)).all());
        tol_abs_error_ = val;
        return *this;
    }

private:
    //! Tolerance of relative error.
    variable_type tol_rel_error_;

    //! Tolerance of absolute error.
    variable_type tol_abs_error_;

    //! Weight for error norm.
    scalar_type norm_weight_;
};

}  // namespace num_collect::ode
