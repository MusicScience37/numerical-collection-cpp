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
 * \brief Definition of gaussian_m1_rbf class.
 */
#pragma once

#include <cmath>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/rbf/rbfs/differentiated.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"

namespace num_collect::rbf::rbfs {

/*!
 * \brief Class of RBF which is Gaussian minus 1.
 * \f$ \exp(-r^2) - 1 \f$.
 *
 * \tparam Scalar Type of scalars.
 *
 * This RBF makes RBF interpolation using polynomials stabler than
 * \ref num_collect::rbf::rbfs::gaussian_rbf
 * by improving linear independence of RBF and polynomials.
 *
 * \warning This RBF should not be used without constant terms.
 */
template <base::concepts::real_scalar Scalar>
class gaussian_m1_rbf {
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
        using std::expm1;
        return expm1(-distance_rate * distance_rate);
    }
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::gaussian_m1_rbf.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
struct differentiated<gaussian_m1_rbf<Scalar>> {
    //! Type of the differentiated RBF.
    using type = differentiated_gaussian_rbf<Scalar, 1>;
};

}  // namespace num_collect::rbf::rbfs
