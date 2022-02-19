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
 * \brief Definition of invocable concept.
 */
#pragma once

#include <functional>

namespace num_collect {
inline namespace base {
namespace concepts {

/*!
 * \brief Concept of functions invocable with given arguments.
 *
 * \tparam Func Type of the function.
 * \tparam Args Type of arguments.
 */
template <typename Func, typename... Args>
concept invocable = requires(Func&& func, Args&&... args) {
    std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
};

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
