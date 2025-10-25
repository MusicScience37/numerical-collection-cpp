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
 * \brief Definition of mock_log_sink class.
 */
#pragma once

#include <string_view>

#include <fmt/format.h>

#include "num_collect/logging/log_level.h"
#include "num_collect/logging/sinks/log_sink.h"
#include "num_collect/logging/time_stamp.h"
#include "num_collect/util/source_info_view.h"
#include "trompeloeil_catch2.h"

namespace num_collect_test::logging {

class mock_log_sink {
public:
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MAKE_MOCK5(write_impl,
        void(num_collect::logging::time_stamp time, std::string_view tag,
            num_collect::logging::log_level level,
            num_collect::util::source_info_view source, std::string_view body),
        noexcept);

    [[nodiscard]] auto to_log_sink() -> num_collect::logging::sinks::log_sink {
        return num_collect::logging::sinks::log_sink(
            this,
            [](void* ptr, num_collect::logging::time_stamp time,
                std::string_view tag, num_collect::logging::log_level level,
                num_collect::util::source_info_view source,
                std::string_view body) noexcept {
                static_cast<mock_log_sink*>(ptr)->write_impl(
                    time, tag, level, source, body);
            },
            [](void* /*ptr*/) noexcept {
                // NOP
            });
    }
};

}  // namespace num_collect_test::logging
