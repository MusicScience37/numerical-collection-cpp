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
 * \brief Definition of range_adaptor_closure class.
 */
#pragma once

#include <functional>
#include <ranges>

namespace num_collect::util::impl {

/*!
 * \brief Base class of light implementation of range adaptor closures.
 *
 * \tparam Derived Type of derived class.
 *
 * This class implements a part of functionality of
 * `std::ranges::range_adaptor_closure` in C++23
 * for some compilers that do not support it yet.
 * This class only supports `operator|` for ranges.
 * So only the case of `range | this_closure` or
 * `range | another_closure | this_closure` is supported.
 * The case of `another_closure | this_closure` without ranges are not
 * supported.
 *
 * Derived classes should implement `operator()` to apply the closure to a
 * range as follows:
 *
 * ~~~{.cpp}
 * template <std::ranges::viewable_range Range>
 * auto operator()(Range&& range) const {
 *     // Implementation to apply the closure to the range.
 * }
 * ~~~
 */
template <typename Derived>
class light_range_adaptor_closure {};

/*!
 * \brief Apply a range adaptor closure to a range.
 *
 * \tparam Range Type of the range.
 * \tparam Closure Type of the range adaptor closure.
 * \param[in] range Range to apply the closure to.
 * \param[in] closure Range adaptor closure to apply.
 * \return Result of applying the closure to the range.
 */
template <std::ranges::viewable_range Range, typename Closure>
    requires std::derived_from<Closure, light_range_adaptor_closure<Closure>> &&
    std::invocable<Closure, Range>
[[nodiscard]] auto operator|(Range&& range, Closure&& closure) {
    return std::invoke(
        std::forward<Closure>(closure), std::forward<Range>(range));
}

#if defined(NUM_COLLECT_DOCUMENTATION)

/*!
 * \brief Base class of range adaptor closures.
 *
 * \tparam Derived Type of derived class.
 *
 * \note This type is an alias of `std::ranges::range_adaptor_closure` if
 * supported by the compiler. Otherwise it is an alias of
 * \ref light_range_adaptor_closure.
 */
template <typename Derived>
using range_adaptor_closure = std::ranges::range_adaptor_closure<Derived>;

#elif defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 202202L

template <typename Derived>
using range_adaptor_closure = std::ranges::range_adaptor_closure<Derived>;
#else

template <typename Derived>
using range_adaptor_closure = light_range_adaptor_closure<Derived>;

#endif

}  // namespace num_collect::util::impl
