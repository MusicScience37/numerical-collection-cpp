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

#include <fmt/format.h>

#include "num_collect/logging/sinks/log_sink_base.h"
#include "trompeloeil_catch2.h"

namespace num_collect_test::logging {

class mock_log_sink final : public num_collect::logging::sinks::log_sink_base {
public:
    // NOLINTNEXTLINE(bugprone-exception-escape)
    void write(std::chrono::system_clock::time_point time, std::string_view tag,
        num_collect::logging::log_level level,
        num_collect::util::source_info_view source,
        std::string_view body) noexcept override {
        write_impl(time, tag, level, source, body);
    }

    // NOLINTNEXTLINE
    MAKE_MOCK5(write_impl,
        void(std::chrono::system_clock::time_point time, std::string_view tag,
            num_collect::logging::log_level level,
            num_collect::util::source_info_view source, std::string_view body),
        noexcept);

    mock_log_sink() = default;
    mock_log_sink(const mock_log_sink&) = delete;
    mock_log_sink(mock_log_sink&&) = delete;
    auto operator=(const mock_log_sink&) -> mock_log_sink& = delete;
    auto operator=(mock_log_sink&&) -> mock_log_sink& = delete;
    ~mock_log_sink() override = default;
};

}  // namespace num_collect_test::logging
