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
 * \brief Declaration of generate_radau2a_coeffs_impl function.
 */
#pragma once

#include "num_collect/base/index_type.h"
#include "num_collect/impl/num_collect_export.h"
#include "num_collect/logging/log_tag_view.h"

namespace num_collect::ode::runge_kutta::impl {

//! Log tag of radau2a_table_generator.
constexpr auto radau2a_table_generator_log_tag = logging::log_tag_view(
    "num_collect::ode::runge_kutta::impl::radau2a_table_generator");

/*!
 * \brief Generate coefficients of Butcher tableau of Radau IIA method.
 *
 * \param[in] stages Number of stages.
 * \param[out] slope_coeffs Coefficients of intermediate slopes in the formula.
 * Coefficients are stored in column-major order as in Eigen.
 * Size of the matrix is `stages` x `stages`.
 * \param[out] time_coeffs Coefficients of time in the formula.
 * Size of the vector is `stages`.
 *
 * \note This function was extracted into a pre-built library to avoid excessive
 * memory usage during compilation. A function using similar matrix
 * decompositions required 2.5 GB of memory per compilation unit, and it caused
 * PC to freeze every time when compiling.
 */
NUM_COLLECT_EXPORT void generate_radau2a_coeffs_impl(
    index_type stages, long double* slope_coeffs, long double* time_coeffs);

}  // namespace num_collect::ode::runge_kutta::impl
