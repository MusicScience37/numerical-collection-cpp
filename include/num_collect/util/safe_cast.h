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
 * \brief Definition of safe_cast function.
 */
#pragma once

#include <limits>
#include <type_traits>
#include <typeinfo>

#include <fmt/core.h>

#include "num_collect/base/exception.h"

namespace num_collect::util {

/*!
 * \brief Class of exception on unsafe casts.
 */
class unsafe_cast : public num_collect_exception {
public:
    using num_collect_exception::num_collect_exception;
};

/*!
 * \brief Cast safely.
 *
 * \tparam To Type to cast to.
 * \tparam From Type to cast from.
 * \param[in] value Value.
 * \return Casted value.
 */
template <typename To, typename From,
    std::enable_if_t<std::is_integral_v<To> && std::is_integral_v<From>,
        void*> = nullptr>
[[nodiscard]] inline auto safe_cast(const From& value) -> To {
    // Check upper bound.
    if constexpr (std::numeric_limits<To>::digits <
        std::numeric_limits<From>::digits) {
        if (value > static_cast<From>(std::numeric_limits<To>::max())) {
            throw unsafe_cast(fmt::format(
                "unsafe cast of value {} from {} to {} (upper bound)", value,
                typeid(From).name(), typeid(To).name()));
        }
    }

    // Check lower bound.
    if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>) {
        if (value < static_cast<From>(0)) {
            throw unsafe_cast(fmt::format(
                "unsafe cast of value {} from {} to {} (lower bound)", value,
                typeid(From).name(), typeid(To).name()));
        }
    }
    if constexpr (std::is_signed_v<From> && std::is_signed_v<To> &&
        (std::numeric_limits<To>::digits < std::numeric_limits<From>::digits)) {
        if (value < static_cast<From>(std::numeric_limits<To>::min())) {
            throw unsafe_cast(fmt::format(
                "unsafe cast of value {} from {} to {} (lower bound)", value,
                typeid(From).name(), typeid(To).name()));
        }
    }

    return static_cast<To>(value);
}

}  // namespace num_collect::util
