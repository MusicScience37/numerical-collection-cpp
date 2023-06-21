/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of hash_function concept.
 */
#pragma once

#include <cstddef>

#include "num_collect/base/concepts/same_as.h"  // IWYU pragma: keep

namespace num_collect::util::concepts {

/*!
 * \brief Concept of classes of hash functions.
 *
 * \tparam Hash Type of the hash function.
 * \tparam Key Type of keys.
 */
template <typename Hash, typename Key>
concept hash_function = requires(const Hash& hash, const Key& key) {
                            {
                                hash(key)
                                } -> base::concepts::same_as<std::size_t>;
                        };

}  // namespace num_collect::util::concepts
