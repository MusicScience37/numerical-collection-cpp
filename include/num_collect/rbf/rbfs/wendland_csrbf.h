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
 * \brief Definition of wendland_csrbf class.
 */
#pragma once

#include <cmath>
#include <limits>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/rbfs/differentiated.h"

namespace num_collect::rbf::rbfs {

/*!
 * \brief Class of Wendland's Compactly Supported RBF \cite Wendland1995.
 *
 * \tparam Scalar Type of scalars.
 * \tparam L Degree of the truncated power function before differentiation.
 * When variables has d dimensions, L should be at least d/2+K+1.
 * For L lesser than 1, this RBF is not defined.
 * \tparam K Number of integration operator applied to the truncated power
 * function. The resulting RBF is 2K-th order differentiable.
 */
template <base::concepts::real_scalar Scalar, index_type L, index_type K>
class wendland_csrbf;

/*!
 * \brief Class of Wendland's Compactly Supported RBF \cite Wendland1995.
 *
 * \tparam Scalar Type of scalars.
 * \tparam L Degree of the truncated power function before differentiation.
 * When variables has d dimensions, L should be at least d/2+k+1.
 * For L lesser than 1, this RBF is not defined.
 */
template <base::concepts::real_scalar Scalar, index_type L>
    requires(L >= 1)
class wendland_csrbf<Scalar, L, 0> {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Get the boundary of the support of this CSRBF.
     *
     * \return Boundary.
     */
    [[nodiscard]] static constexpr auto support_boundary() noexcept
        -> scalar_type {
        return static_cast<scalar_type>(1);
    }

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance_rate Rate of distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(
        const scalar_type& distance_rate) const noexcept -> scalar_type {
        using std::pow;
        if (distance_rate > static_cast<scalar_type>(1)) {
            return static_cast<scalar_type>(0);
        }
        // pow function wrongly returns double even when scalar_type is float.
        return static_cast<scalar_type>(
            pow(static_cast<scalar_type>(1) - distance_rate, L));
    }
};

/*!
 * \brief Class of Wendland's Compactly Supported RBF \cite Wendland1995.
 *
 * \tparam Scalar Type of scalars.
 * \tparam L Degree of the truncated power function before differentiation.
 * When variables has d dimensions, L should be at least d/2+k+1.
 * For L lesser than 1, this RBF is not defined.
 */
template <base::concepts::real_scalar Scalar, index_type L>
    requires(L >= 1)
class wendland_csrbf<Scalar, L, 1> {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Get the boundary of the support of this CSRBF.
     *
     * \return Boundary.
     */
    [[nodiscard]] static constexpr auto support_boundary() noexcept
        -> scalar_type {
        return static_cast<scalar_type>(1);
    }

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance_rate Rate of distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(
        const scalar_type& distance_rate) const noexcept -> scalar_type {
        using std::pow;
        if (distance_rate > static_cast<scalar_type>(1)) {
            return static_cast<scalar_type>(0);
        }

        static constexpr scalar_type scale_coeff = static_cast<scalar_type>(1) /
            (static_cast<scalar_type>(L + 1) * static_cast<scalar_type>(L + 2));

        // pow function wrongly returns double even when scalar_type is float.
        const auto pow_value = static_cast<scalar_type>(
            pow(static_cast<scalar_type>(1) - distance_rate, L + 1));

        static constexpr auto first_order_term_coeff =
            static_cast<scalar_type>(L + 1);
        static constexpr auto constant_term = static_cast<scalar_type>(1);
        return scale_coeff * pow_value *
            (first_order_term_coeff * distance_rate + constant_term);
    }
};

/*!
 * \brief Class of Wendland's Compactly Supported RBF \cite Wendland1995.
 *
 * \tparam Scalar Type of scalars.
 * \tparam L Degree of the truncated power function before differentiation.
 * When variables has d dimensions, L should be at least d/2+k+1.
 * For L lesser than 1, this RBF is not defined.
 */
template <base::concepts::real_scalar Scalar, index_type L>
    requires(L >= 1)
class wendland_csrbf<Scalar, L, 2> {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Get the boundary of the support of this CSRBF.
     *
     * \return Boundary.
     */
    [[nodiscard]] static constexpr auto support_boundary() noexcept
        -> scalar_type {
        return static_cast<scalar_type>(1);
    }

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance_rate Rate of distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(
        const scalar_type& distance_rate) const noexcept -> scalar_type {
        using std::pow;
        if (distance_rate > static_cast<scalar_type>(1)) {
            return static_cast<scalar_type>(0);
        }

        static constexpr scalar_type scale_coeff = static_cast<scalar_type>(1) /
            (static_cast<scalar_type>(L + 1) * static_cast<scalar_type>(L + 2) *
                static_cast<scalar_type>(L + 3) *
                static_cast<scalar_type>(L + 4));

        // pow function wrongly returns double even when scalar_type is float.
        const auto pow_value = static_cast<scalar_type>(
            pow(static_cast<scalar_type>(1) - distance_rate, L + 2));

        static constexpr auto second_order_term_coeff =
            static_cast<scalar_type>(L + 1) * static_cast<scalar_type>(L + 3);
        static constexpr auto first_order_term_coeff =
            static_cast<scalar_type>(3) * static_cast<scalar_type>(L + 2);
        static constexpr auto constant_term = static_cast<scalar_type>(3);
        return scale_coeff * pow_value *
            (second_order_term_coeff * distance_rate * distance_rate +
                first_order_term_coeff * distance_rate + constant_term);
    }
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::wendland_csrbf.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar, index_type L, index_type K>
    requires(K >= 1)
struct differentiated<wendland_csrbf<Scalar, L, K>> {
    //! Type of the differentiated RBF.
    using type = wendland_csrbf<Scalar, L, K - 1>;
};

/*!
 * \brief Class of differentiated Wendland's Compactly Supported RBF with
 * order k=0.
 *
 * \tparam Scalar Type of scalars.
 * \tparam L Degree of the truncated power function before differentiation.
 */
template <base::concepts::real_scalar Scalar, index_type L>
class differentiated_wendland_csrbf_k0 {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Get the boundary of the support of this CSRBF.
     *
     * \return Boundary.
     */
    [[nodiscard]] static constexpr auto support_boundary() noexcept
        -> scalar_type {
        return static_cast<scalar_type>(1);
    }

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance_rate Rate of distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(
        const scalar_type& distance_rate) const noexcept -> scalar_type {
        using std::pow;
        if (distance_rate > static_cast<scalar_type>(1)) {
            return static_cast<scalar_type>(0);
        }
        if (distance_rate < std::numeric_limits<scalar_type>::epsilon()) {
            return static_cast<scalar_type>(0);
        }
        constexpr auto coeff = static_cast<scalar_type>(L);
        return coeff *
            // pow function wrongly returns double even when scalar_type is
            // float.
            static_cast<scalar_type>(
                pow(static_cast<scalar_type>(1) - distance_rate, L - 1)) /
            distance_rate;
    }
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::wendland_csrbf.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar, index_type L>
struct differentiated<wendland_csrbf<Scalar, L, 0>> {
    //! Type of the differentiated RBF.
    using type = differentiated_wendland_csrbf_k0<Scalar, L>;
};

}  // namespace num_collect::rbf::rbfs
