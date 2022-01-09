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
 * \brief Test of log_config class.
 */
#include "num_collect/logging/log_config.h"

#include <catch2/catch_test_macros.hpp>

#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/util/index_type.h"

TEST_CASE("num_collect::logging::log_config") {
    using num_collect::logging::log_config;
    using num_collect::logging::log_tag_config;
    using num_collect::logging::log_tag_view;

    SECTION("get the configuration of a non-existing tag") {
        const auto tag = log_tag_view("num_collect::logging::log_config_test1");

        CHECK_NOTHROW((void)log_config::instance().get_config_of(tag));
    }

    SECTION("set the configuration of a non-existing tag") {
        const auto tag = log_tag_view("num_collect::logging::log_config_test2");
        const num_collect::index_type iteration_label_period = 123;
        const auto config =
            log_tag_config().iteration_label_period(iteration_label_period);

        CHECK_NOTHROW(log_config::instance().set_config_of(tag, config));
        CHECK_NOTHROW(log_config::instance()
                          .get_config_of(tag)
                          .iteration_label_period() == iteration_label_period);
    }

    SECTION("set the configuration of a existing tag") {
        const auto tag = log_tag_view("num_collect::logging::log_config_test3");
        const num_collect::index_type iteration_label_period = 123;
        const auto config =
            log_tag_config().iteration_label_period(iteration_label_period);

        CHECK_NOTHROW((void)log_config::instance().get_config_of(tag));
        CHECK_NOTHROW(log_config::instance().set_config_of(tag, config));
        CHECK_NOTHROW(log_config::instance()
                          .get_config_of(tag)
                          .iteration_label_period() == iteration_label_period);
    }
}
