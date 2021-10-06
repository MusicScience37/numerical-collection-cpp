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
 * \brief declaration and implementation of gaussian_rbf class
 */
#pragma once

#include <cmath>

namespace num_collect::interp::kernel {

/*!
 * \brief Class to calculate Gaussian RBF.
 *
 * \tparam Scalar Type of scalars.
 */
template <typename Scalar>
class gaussian_rbf {
public:
    //! Type of arguments.
    using arg_type = Scalar;

    //! Type of function values.
    using value_type = Scalar;

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] arg_type Argument.
     * \return Value.
     */
    [[nodiscard]] auto operator()(const arg_type& arg_type) const
        -> value_type {
        using std::exp;
        return exp(-arg_type * arg_type);
    }
};

}  // namespace num_collect::interp::kernel
