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
 * \brief Definition of coefficients for regularization parameters.
 */
#pragma once

namespace num_collect::regularization::impl {

/*!
 * \brief Coefficient (maximum parameter to be searched) / (maximum singular
 * value or eigen value).
 *
 * \tparam T Type of scalars.
 */
template <typename T>
constexpr auto weak_coeff_max_param = static_cast<T>(1e+2);

/*!
 * \brief Coefficient (minimum parameter to be searched) / (maximum singular
 * value or eigen value).
 *
 * \tparam T Type of scalars.
 */
template <typename T>
constexpr auto weak_coeff_min_param = static_cast<T>(1e-4);

}  // namespace num_collect::regularization::impl
