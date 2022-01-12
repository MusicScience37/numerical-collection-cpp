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
 * \brief Test of source_info_view class.
 */
#include "num_collect/util/source_info_view.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

static auto source_info_view_test1(num_collect::util::source_info_view info =
                                       num_collect::util::source_info_view())
    -> num_collect::util::source_info_view {
    return info;
}

static auto source_info_view_test2() -> num_collect::util::source_info_view {
    return source_info_view_test1();
}

TEST_CASE("num_collect::source_info_view") {
    SECTION("get information") {
        const num_collect::util::source_info_view info =
            source_info_view_test2();

#if NUM_COLLECT_HAS_SOURCE_LOCATION
        CHECK_THAT(std::string(info.file_path()),
            Catch::Matchers::Contains("source_info_view_test.cpp"));
        CHECK(info.line() == 32);
        CHECK(info.column() >= 0);
        CHECK_THAT(std::string(info.function_name()),
            Catch::Matchers::Contains("source_info_view_test2"));
#else
        (void)info;
#endif
    }
}
