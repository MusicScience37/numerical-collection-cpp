/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of non_positive_definite_rbf class.
 */
#pragma once

#include <cmath>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/util/assert.h"

/*!
 * \brief Class of a non-positive-definite RBF for testing.
 *
 * This RBF is actually a negative definite RBF $r^5$.
 */
template <num_collect::base::concepts::real_scalar Scalar>
class non_positive_definite_rbf {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    //! Whether this RBF is positive definite.
    static constexpr bool is_positive_definite = false;

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
        return static_cast<scalar_type>(pow(distance_rate, 5));
    }
};
