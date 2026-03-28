/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of polyharmonic_spline_rbf class.
 */
#pragma once

#include <cmath>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/functions/pow.h"
#include "num_collect/rbf/rbfs/differentiated.h"
#include "num_collect/util/assert.h"

namespace num_collect::rbf::rbfs {

/*!
 * \brief Class of RBF of polyharmonic splines \cite Fornberg2015.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Degree Degree of the distance.
 */
template <base::concepts::real_scalar Scalar, index_type Degree>
class polyharmonic_spline_rbf;

/*!
 * \brief Class of RBF of polyharmonic splines \cite Fornberg2015 for odd
 * degrees.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Degree Degree of the distance.
 */
template <base::concepts::real_scalar Scalar, index_type Degree>
    requires(Degree >= 1 && (Degree % 2 == 1))
class polyharmonic_spline_rbf<Scalar, Degree> {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance_rate Rate of distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(
        const scalar_type& distance_rate) const noexcept -> scalar_type {
        using std::pow;
        NUM_COLLECT_DEBUG_ASSERT(distance_rate >= static_cast<scalar_type>(0));
        return static_cast<scalar_type>(pow(distance_rate, Degree));
    }
};

/*!
 * \brief Class of RBF of polyharmonic splines \cite Fornberg2015 for even
 * degrees.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Degree Degree of the distance.
 */
template <base::concepts::real_scalar Scalar, index_type Degree>
    requires(Degree >= 1 && (Degree % 2 == 0))
class polyharmonic_spline_rbf<Scalar, Degree> {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance_rate Rate of distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(
        const scalar_type& distance_rate) const noexcept -> scalar_type {
        using std::log;
        using std::pow;
        NUM_COLLECT_DEBUG_ASSERT(distance_rate >= static_cast<scalar_type>(0));
        if (distance_rate == static_cast<scalar_type>(0)) {
            return static_cast<scalar_type>(0);
        }
        return static_cast<scalar_type>(pow(distance_rate, Degree)) *
            log(distance_rate);
    }
};

/* *******************************************************************************
 * Derivatives of polyharmonic spline RBFs for odd degrees.
 * *******************************************************************************/

/*!
 * \brief Class of the derivative of polyharmonic spline RBF for odd degrees.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Degree Degree of the distance.
 * \tparam Order Number of differentiation.
 */
template <base::concepts::real_scalar Scalar, index_type Degree,
    index_type Order>
    requires(Degree >= 1 && (Degree % 2 == 1) && Order > 0)
class odd_degree_differentiated_polyharmonic_spline_rbf {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance_rate Rate of distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(
        const scalar_type& distance_rate) const noexcept -> scalar_type {
        using std::pow;
        NUM_COLLECT_DEBUG_ASSERT(distance_rate >= static_cast<scalar_type>(0));
        constexpr scalar_type coeff = calculate_coeff();
        constexpr index_type degree_after_diff = Degree - 2 * Order;
        if constexpr (degree_after_diff > 0) {
            return coeff * pow(distance_rate, degree_after_diff);
        } else {
            constexpr auto small_number = static_cast<scalar_type>(1e-50);
            if (distance_rate < small_number) {
                constexpr auto value =
                    coeff * functions::pow(small_number, degree_after_diff);
                return value;
            }
            return coeff * pow(distance_rate, degree_after_diff);
        }
    }

private:
    /*!
     * \brief Calculate the coefficient of the derivative of RBF.
     *
     * \return Coefficient of the derivative of RBF.
     */
    [[nodiscard]] static constexpr auto calculate_coeff() noexcept
        -> scalar_type {
        auto coeff = static_cast<scalar_type>(1);
        for (index_type i = 1; i <= Order; ++i) {
            coeff *= -static_cast<scalar_type>(Degree - 2 * (i - 1));
        }
        return coeff;
    }
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::polyharmonic_spline_rbf for odd degrees.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Degree Degree of the distance.
 */
template <base::concepts::real_scalar Scalar, index_type Degree>
    requires(Degree >= 1 && (Degree % 2 == 1))
struct differentiated<polyharmonic_spline_rbf<Scalar, Degree>> {
    //! Type of the differentiated RBF.
    using type =
        odd_degree_differentiated_polyharmonic_spline_rbf<Scalar, Degree, 1>;
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::odd_degree_differentiated_polyharmonic_spline_rbf.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Degree Degree of the distance.
 * \tparam Order Number of differentiation.
 */
template <base::concepts::real_scalar Scalar, index_type Degree,
    index_type Order>
    requires(Degree >= 1 && (Degree % 2 == 1) && Order > 0)
struct differentiated<
    odd_degree_differentiated_polyharmonic_spline_rbf<Scalar, Degree, Order>> {
    //! Type of the differentiated RBF.
    using type = odd_degree_differentiated_polyharmonic_spline_rbf<Scalar,
        Degree, Order + 1>;
};

/* *******************************************************************************
 * Derivatives of polyharmonic spline RBFs for even degrees.
 * *******************************************************************************/

/*!
 * \brief Class of the first-order derivative of polyharmonic spline RBF for
 * even degrees.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Degree Degree of the distance.
 */
template <base::concepts::real_scalar Scalar, index_type Degree>
    requires(Degree >= 1 && (Degree % 2 == 0))
class even_degree_first_differentiated_polyharmonic_spline_rbf {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance_rate Rate of distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(
        const scalar_type& distance_rate) const noexcept -> scalar_type {
        using std::log;
        using std::pow;
        NUM_COLLECT_DEBUG_ASSERT(distance_rate >= static_cast<scalar_type>(0));
        if constexpr (Degree == 2) {
            // Handle specially to prevent pow(0, 0).
            if (distance_rate == static_cast<scalar_type>(0)) {
                // This value should disappear in the calculation of
                // derivatives.
                return static_cast<scalar_type>(-1);
            }
            return -(static_cast<scalar_type>(2) * log(distance_rate) + 1);
        } else {
            // General case.
            if (distance_rate == static_cast<scalar_type>(0)) {
                // Limit of distance_rate -> 0.
                return static_cast<scalar_type>(0);
            }
            return -pow(distance_rate, Degree - 2) *
                (static_cast<scalar_type>(Degree) * log(distance_rate) + 1);
        }
    }
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::polyharmonic_spline_rbf for even degrees.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Degree Degree of the distance.
 */
template <base::concepts::real_scalar Scalar, index_type Degree>
    requires(Degree >= 1 && (Degree % 2 == 0))
struct differentiated<polyharmonic_spline_rbf<Scalar, Degree>> {
    //! Type of the differentiated RBF.
    using type =
        even_degree_first_differentiated_polyharmonic_spline_rbf<Scalar,
            Degree>;
};

}  // namespace num_collect::rbf::rbfs
