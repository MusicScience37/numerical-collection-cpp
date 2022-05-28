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
 * \brief Definition of concurrent_map class.
 */
#pragma once

#include <functional>
#include <memory>
#include <utility>

#include <hash_tables/maps/separate_shared_chain_map_mt.h>

#include "num_collect/util/comparators.h"
#include "num_collect/util/concepts/comparator.h"
#include "num_collect/util/concepts/hash_function.h"

namespace num_collect::util {

/*!
 * \brief Type of concurrent maps.
 *
 * \tparam Key Type of keys.
 * \tparam Value Type of mapped values.
 * \tparam Hash Type of the hash function.
 * \tparam KeyEqual Type of the function to check equality of keys.
 * \tparam Allocator Type of the allocator.
 */
template <typename Key, typename Value,
    concepts::hash_function<Key> Hash = std::hash<Key>,
    concepts::comparator<Key, Key> KeyEqual = equal<Key>,
    typename Allocator = std::allocator<std::pair<Key, Value>>>
using concurrent_map = hash_tables::maps::separate_shared_chain_map_mt<Key,
    Value, Hash, KeyEqual, Allocator>;

}  // namespace num_collect::util
