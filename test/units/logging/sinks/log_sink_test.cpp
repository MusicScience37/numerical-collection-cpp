/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of log_sink class.
 */
#include "num_collect/logging/sinks/log_sink.h"

#include <string>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/time_stamp.h"
#include "num_collect/util/source_info_view.h"

TEST_CASE("num_collect::logging::sinks::log_sink") {
    using num_collect::index_type;
    using num_collect::logging::log_level;
    using num_collect::logging::time_stamp;
    using num_collect::logging::sinks::log_sink;
    using num_collect::util::source_info_view;

    struct user_data_type {
        int write_count{0};
        time_stamp last_time{time_stamp::now()};
        std::string_view last_tag{};
        log_level last_level{log_level::info};
        source_info_view last_source{};
        std::string_view last_body{};
        int finalize_count{0};
    };
    user_data_type user_data{};
    const log_sink::write_function_type write_function =
        [](void* user_data, time_stamp time, std::string_view tag,
            log_level level, source_info_view source,
            std::string_view body) noexcept {
            auto* const ptr = static_cast<user_data_type*>(user_data);
            ptr->write_count++;
            ptr->last_time = time;
            ptr->last_tag = tag;
            ptr->last_level = level;
            ptr->last_source = source;
            ptr->last_body = body;
        };
    const log_sink::finalizer_type finalize_function =
        [](void* user_data) noexcept {
            static_cast<user_data_type*>(user_data)->finalize_count++;
        };

    SECTION("create and destroy") {
        CHECK(user_data.write_count == 0);
        CHECK(user_data.finalize_count == 0);

        {
            log_sink sink{&user_data, write_function, finalize_function};
        }

        CHECK(user_data.write_count == 0);
        CHECK(user_data.finalize_count == 1);
    }

    const auto test_write = [&user_data](const log_sink& sink) {
        const auto time =
            time_stamp(1703334965, 123456789);  // 2023-12-23T12:36:05.123456789
        const auto tag = std::string_view("Tag");
        const auto level = log_level::summary;
        const auto file_path = std::string_view("filepath");
        const index_type line = 123;
        const index_type column = 7;
        const auto function_name = std::string_view("function");
        const auto source =
            source_info_view(file_path, line, column, function_name);
        const auto body = std::string_view("body");
        sink.write(time, tag, level, source, body);

        CHECK(user_data.last_time.seconds() == time.seconds());
        CHECK(user_data.last_time.nanoseconds() == time.nanoseconds());
        CHECK(user_data.last_tag == tag);
        CHECK(user_data.last_level == level);
        CHECK(user_data.last_source.file_path() == source.file_path());
        CHECK(user_data.last_source.line() == source.line());
        CHECK(user_data.last_source.column() == source.column());
        CHECK(user_data.last_source.function_name() == source.function_name());
        CHECK(user_data.last_body == body);
    };

    SECTION("write a log") {
        log_sink sink{&user_data, write_function, finalize_function};

        test_write(sink);
        CHECK(user_data.write_count == 1);
        CHECK(user_data.finalize_count == 0);
    }

    SECTION("copy constructor") {
        {
            log_sink sink{&user_data, write_function, finalize_function};

            log_sink copy{sink};  // NOLINT

            test_write(sink);
            test_write(copy);
        }
        CHECK(user_data.write_count == 2);
        CHECK(user_data.finalize_count == 1);
    }

    SECTION("copy assignment") {
        user_data_type another_user_data{};
        {
            log_sink sink{&user_data, write_function, finalize_function};

            log_sink copy{
                &another_user_data, write_function, finalize_function};
            copy = sink;  // NOLINT

            test_write(sink);
            test_write(copy);
        }
        CHECK(user_data.write_count == 2);
        CHECK(user_data.finalize_count == 1);
        CHECK(another_user_data.write_count == 0);
        CHECK(another_user_data.finalize_count == 1);
    }

    SECTION("move constructor") {
        {
            log_sink sink{&user_data, write_function, finalize_function};

            log_sink copy{std::move(sink)};

            test_write(copy);
        }
        CHECK(user_data.write_count == 1);
        CHECK(user_data.finalize_count == 1);
    }

    SECTION("move assignment") {
        user_data_type another_user_data{};
        {
            log_sink sink{&user_data, write_function, finalize_function};

            log_sink copy{
                &another_user_data, write_function, finalize_function};
            copy = std::move(sink);

            test_write(copy);
        }
        CHECK(user_data.write_count == 1);
        CHECK(user_data.finalize_count == 1);
        CHECK(another_user_data.write_count == 0);
        CHECK(another_user_data.finalize_count == 1);
    }
}
