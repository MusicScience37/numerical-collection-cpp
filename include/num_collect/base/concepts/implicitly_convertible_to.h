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
 * \brief Definition of implicitly_convertible_to concept.
 */
#pragma once

namespace num_collect {
inline namespace base {
namespace concepts {

/*!
 * \brief Concept to check if `From` is implicitly convertible to `To`.
 *
 * \tparam From Type to convert from.
 * \tparam To Type to convert to.
 */
template <typename From, typename To>
concept implicitly_convertible_to =
    requires(const From& from, To& to) { to = from; };

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
