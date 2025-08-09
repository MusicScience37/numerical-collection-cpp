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
 * \brief Definition of invocable_as concept.
 */
#pragma once

#include <functional>
#include <type_traits>
#include <utility>

#include "num_collect/base/concepts/implicitly_convertible_to.h"

namespace num_collect {
inline namespace base {
namespace concepts {

namespace impl {

/*!
 * \brief Concept to check if Func can be invoked with Args... and the result is
 * convertible to Result.
 *
 * \tparam Func Type of the function.
 * \tparam Result Expected result type.
 * \tparam Args Argument types.
 */
template <typename Func, typename Result, typename... Args>
concept invocable_as_impl = requires(Func&& func, Args&&... args) {
    {
        std::invoke(std::forward<Func>(func), std::forward<Args>(args)...)
    } -> implicitly_convertible_to<Result>;
};

}  // namespace impl

/*!
 * \brief Check whether the given function is invocable as a function with the
 * given signature.
 *
 * \tparam Func Type of the function.
 * \tparam Signature Type of the function signature.
 */
template <typename Func, typename Signature>
struct is_invocable_as : public std::false_type {};

/*!
 * \brief Check if Func can be invoked as a function with the given signature.
 *
 * \tparam Func Type of the function.
 * \tparam Result Expected result type.
 * \tparam Args Argument types.
 */
template <typename Func, typename Result, typename... Args>
struct is_invocable_as<Func, Result(Args...)> {
public:
    //! Result value.
    static constexpr bool value =
        impl::invocable_as_impl<Func, Result, Args...>;
};

/*!
 * \brief Check if Func can be invoked as a function with the given signature.
 *
 * \tparam Func Type of the function.
 * \tparam Signature Function signature type (e.g., Result(Args...)).
 */
template <typename Func, typename Signature>
constexpr bool is_invocable_as_v = is_invocable_as<Func, Signature>::value;

/*!
 * \brief Concept to check if Func can be invoked as a function with the given
 * signature.
 *
 * \tparam Func Type of the function.
 * \tparam Signature Function signature type (e.g., Result(Args...)).
 */
template <typename Func, typename Signature>
concept invocable_as = is_invocable_as_v<Func, Signature>;

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
