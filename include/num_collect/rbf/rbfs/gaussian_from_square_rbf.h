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
 * \brief Definition of gaussian_from_square_rbf class.
 */
#pragma once

#include <cmath>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/rbfs/differentiated.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/util/assert.h"

namespace num_collect::rbf::rbfs {

/*!
 * \brief Class of RBF which calculates exponential function in Gaussian RBF
 * from the square term of Maclaurin expansion.
 *
 * \tparam Scalar Type of scalars.
 *
 * This RBF calculates the following function:
 * \f[
 * \phi(r) = e^{-r^2} - 1 + r^2
 * = \sum_{k=2}^{\infty} \frac{\left(-r^2\right)^k}{k!}
 * \f]
 *
 * This RBF makes RBF interpolation using polynomials stabler than
 * \ref num_collect::rbf::rbfs::gaussian_rbf
 * by improving linear independence of RBF and polynomials.
 *
 * \warning This RBF should be used with polynomials at least up to quadratic
 * terms.
 */
template <base::concepts::real_scalar Scalar>
class gaussian_from_square_rbf {
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
        NUM_COLLECT_DEBUG_ASSERT(distance_rate >= static_cast<scalar_type>(0));

        constexpr auto threshold = static_cast<scalar_type>(0.1);
        if (distance_rate > threshold) {
            using std::expm1;
            const scalar_type exp_arg = -distance_rate * distance_rate;
            return expm1(exp_arg) - exp_arg;
        }
        // Use Maclaurin expansion for small distance_rate.
        const scalar_type exp_arg = -distance_rate * distance_rate;
        scalar_type term = exp_arg;
        auto result = static_cast<scalar_type>(0);
        constexpr index_type max_term = 10;
        for (index_type i = 2; i <= max_term; ++i) {
            term *= exp_arg;
            term /= static_cast<scalar_type>(i);
            result += term;
        }
        return result;
    }
};

/*!
 * \brief Class of differentiated RBF which calculates exponential function in
 * Gaussian RBF from the square term of Maclaurin expansion.
 *
 * \tparam Scalar Type of scalars.
 *
 * This class calculates the derivative of
 * \ref num_collect::rbf::rbfs::gaussian_from_square_rbf
 * as follows:
 * \f[
 * -\frac{1}{r} \phi'(r) = 2 e^{-r^2} - 2
 * \f]
 */
template <base::concepts::real_scalar Scalar>
class differentiated_gaussian_from_square_rbf {
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
        NUM_COLLECT_DEBUG_ASSERT(distance_rate >= static_cast<scalar_type>(0));
        using std::expm1;
        return static_cast<scalar_type>(2) *
            expm1(-distance_rate * distance_rate);
    }
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::gaussian_from_square_rbf.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
struct differentiated<gaussian_from_square_rbf<Scalar>> {
    //! Type of the differentiated RBF.
    using type = differentiated_gaussian_from_square_rbf<Scalar>;
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::differentiated_gaussian_from_square_rbf.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
struct differentiated<differentiated_gaussian_from_square_rbf<Scalar>> {
    //! Type of the differentiated RBF.
    using type = differentiated_gaussian_rbf<Scalar, 2>;
};

}  // namespace num_collect::rbf::rbfs
