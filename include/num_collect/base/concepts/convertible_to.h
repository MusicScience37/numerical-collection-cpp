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
 * \brief Definition of convertible_to concept.
 */
#pragma once

#include <concepts>

namespace num_collect {
inline namespace base {
namespace concepts {

/*!
 * \brief Concept to check if `From` can be converted to `To`.
 *
 * \tparam From Type to convert from.
 * \tparam To Type to convert to.
 *
 * \note This concept was originally implemented when some of C++ standard
 * library implementations did not have `std::convertible_to`.
 */
template <typename From, typename To>
concept convertible_to = std::convertible_to<From, To>;

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
