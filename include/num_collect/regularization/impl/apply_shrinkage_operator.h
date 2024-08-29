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
 * \brief Definition of apply_shrinkage_operator function.
 */
#pragma once

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"

namespace num_collect::regularization::impl {

/*!
 * \brief Apply shrinkage operator to a vector.
 *
 * \tparam Vector Type of the vector.
 * \param[in,out] target Vector to apply shrinkage operator.
 * \param[in] threshold Threshold.
 */
template <base::concepts::real_scalar_dense_vector Vector>
void apply_shrinkage_operator(
    Vector& target, typename Vector::Scalar threshold) {
    // Determined by benchmarks.
    constexpr num_collect::index_type parallelized_size = 10000;
#pragma omp parallel for if (target.size() > parallelized_size)
    for (num_collect::index_type i = 0; i < target.size(); ++i) {
        if (target(i) > threshold) {
            target(i) -= threshold;
        } else if (target(i) < -threshold) {
            target(i) += threshold;
        } else {
            target(i) = 0.0;
        }
    }
}

}  // namespace num_collect::regularization::impl
