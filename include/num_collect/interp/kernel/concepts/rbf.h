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
 * \brief Definition of rbf concept.
 */
#pragma once

#include <utility>

#include "num_collect/base/concepts/implicitly_convertible_to.h"  // IWYU pragma: keep

namespace num_collect::interp::kernel::concepts {

/*!
 * \brief Concept of radial basis functions (RBFs).
 *
 * \tparam T Type.
 */
template <typename T>
concept rbf = requires(const T& obj) {
                  typename T::arg_type;
                  typename T::value_type;
                  {
                      obj(std::declval<typename T::arg_type>())
                      } -> base::concepts::implicitly_convertible_to<
                          typename T::value_type>;
              };

}  // namespace num_collect::interp::kernel::concepts
