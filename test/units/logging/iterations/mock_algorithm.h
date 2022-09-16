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
 * \brief Definition of mock_algorithm class.
 */
#pragma once

#include <utility>

#include "trompeloeil_catch2.h"

namespace num_collect_test::logging::iterations {

template <typename Value1, typename Value2>
class mock_algorithm {
public:
    explicit mock_algorithm(Value1 value1_in) : value1(std::move(value1_in)) {}

    Value1 value1;  // NOLINT

    // NOLINTNEXTLINE
    MAKE_MOCK0(get_impl, Value2(), const);

    [[nodiscard]] auto get() const -> Value2 { return get_impl(); }
};

}  // namespace num_collect_test::logging::iterations
