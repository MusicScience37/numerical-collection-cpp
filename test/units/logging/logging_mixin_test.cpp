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
 * \brief Test of logging_mixin class.
 */
#include "num_collect/logging/logging_mixin.h"

#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_tag_config.h"

TEST_CASE("num_collect::logging::logging_mixin") {
    using num_collect::logging::log_tag_config;
    using num_collect::logging::log_tag_view;
    using num_collect::logging::logging_mixin;

    SECTION("use logger") {
        constexpr auto tag =
            log_tag_view("num_collect::logging::logging_mixin_test1");
        const num_collect::index_type iteration_label_period = 123;
        const auto config =
            log_tag_config().iteration_label_period(iteration_label_period);
        CHECK_NOTHROW(set_config_of(tag, config));

        const auto mixin = logging_mixin(tag);
        CHECK(mixin.logger().config().iteration_label_period() ==
            iteration_label_period);
    }
}
