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
 * \brief Definition of formattable_iteration_parameter_value concept.
 */
#pragma once

#include <fmt/format.h>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/iterations/iteration_parameter_formatter_decl.h"

namespace num_collect::logging::concepts {

/*!
 * \brief Concept of formattable parameter values in iterations.
 *
 * \tparam T Type.
 */
template <typename T>
concept formattable_iteration_parameter_value = requires() {
    iterations::iteration_parameter_formatter<T>{};

    requires requires(const iterations::iteration_parameter_formatter<T>& obj,
        const T& value,
        fmt::memory_buffer& buffer) { obj.format(value, buffer); };

    requires requires(const iterations::iteration_parameter_formatter<T>& obj,
        const T& value, index_type width, fmt::memory_buffer& buffer) {
        obj.format_with_alignment(value, width, buffer);
    };
};

}  // namespace num_collect::logging::concepts

#include "num_collect/logging/iterations/iteration_parameter_formatter.h"  // IWYU pragma: keep
