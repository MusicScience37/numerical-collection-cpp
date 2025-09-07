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

namespace num_collect::rbf::rbfs {

/*!
 * \brief Class of RBF which calculates exponential fuction in Gaussian RBF from
 * the square term of Maclaurin expansion.
 *
 * \tparam Scalar Type of scalars.
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

}  // namespace num_collect::rbf::rbfs
