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
 * \brief Definition of gaussian_rbf class.
 */
#pragma once

#include <cmath>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/functions/pow.h"
#include "num_collect/rbf/rbfs/differentiated.h"

namespace num_collect::rbf::rbfs {

/*!
 * \brief Class of Gaussian RBF \cite Fornberg2015.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class gaussian_rbf {
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
        using std::exp;
        return exp(-distance_rate * distance_rate);
    }
};

/*!
 * \brief Class of differentiated Gaussian RBF.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Order Number of differentiation.
 */
template <base::concepts::real_scalar Scalar, index_type Order>
    requires(Order > 0)
class differentiated_gaussian_rbf {
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
        using std::exp;
        constexpr scalar_type coeff =
            functions::pow(static_cast<scalar_type>(2), Order);
        return coeff * exp(-distance_rate * distance_rate);
    }
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::gaussian_rbf.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
struct differentiated<gaussian_rbf<Scalar>> {
    //! Type of the differentiated RBF.
    using type = differentiated_gaussian_rbf<Scalar, 1>;
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::differentiated_gaussian_rbf.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Order Number of differentiation.
 */
template <base::concepts::real_scalar Scalar, index_type Order>
struct differentiated<differentiated_gaussian_rbf<Scalar, Order>> {
    //! Type of the differentiated RBF.
    using type = differentiated_gaussian_rbf<Scalar, Order + 1>;
};

}  // namespace num_collect::rbf::rbfs
