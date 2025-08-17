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

// IWYU pragma: no_include <string_view>
// IWYU pragma: no_include "num_collect/util/source_info_view.h"

#include <concepts>
#include <limits>
#include <type_traits>  // IWYU pragma: keep
#include <typeinfo>

#include <fmt/format.h>  // IWYU pragma: keep

#include "num_collect/base/exception.h"
#include "num_collect/logging/logging_macros.h"

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
template <std::integral To, std::integral From>
[[nodiscard]] inline auto safe_cast(const From& value) -> To {
    // Check upper bound.
    if constexpr (std::numeric_limits<To>::digits <
        std::numeric_limits<From>::digits) {
        if (value > static_cast<From>(std::numeric_limits<To>::max())) {
            NUM_COLLECT_LOG_AND_THROW(unsafe_cast,
                "unsafe cast of value {} from {} to {} (upper bound)", value,
                typeid(From).name(), typeid(To).name());
        }
    }

    // Check lower bound.
    if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>) {
        if (value < static_cast<From>(0)) {
            NUM_COLLECT_LOG_AND_THROW(unsafe_cast,
                "unsafe cast of value {} from {} to {} (lower bound)", value,
                typeid(From).name(), typeid(To).name());
        }
    }
    if constexpr (std::is_signed_v<From> && std::is_signed_v<To> &&
        (std::numeric_limits<To>::digits < std::numeric_limits<From>::digits)) {
        if (value < static_cast<From>(std::numeric_limits<To>::min())) {
            NUM_COLLECT_LOG_AND_THROW(unsafe_cast,
                "unsafe cast of value {} from {} to {} (lower bound)", value,
                typeid(From).name(), typeid(To).name());
        }
    }

    return static_cast<To>(value);
}

}  // namespace num_collect::util
