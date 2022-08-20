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
 * \brief Test of log_tag_config_tree class.
 */
#include "num_collect/logging/impl/log_tag_config_tree.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"

TEST_CASE("num_collect::logging::impl::log_tag_config_tree") {
    using num_collect::logging::log_tag_config;
    using num_collect::logging::log_tag_view;
    using num_collect::logging::impl::log_tag_config_tree;

    log_tag_config_tree tree;

    SECTION("get the top level configuration") {
        const auto tag = log_tag_view("");
        const auto expected_config = log_tag_config();
        CHECK(tree.get_config_of(tag).iteration_label_period() ==
            expected_config.iteration_label_period());
    }

    SECTION("set and get the top level configuration") {
        const auto tag = log_tag_view("");
        const auto expected_config =
            log_tag_config().iteration_label_period(123);
        CHECK_NOTHROW(tree.set_config_of(tag, expected_config));
        CHECK(tree.get_config_of(tag).iteration_label_period() ==
            expected_config.iteration_label_period());
    }

    SECTION("set and get the child level configuration") {
        const auto parent_tag = log_tag_view("test");
        const auto child_tag = log_tag_view("test::log::tag");
        const auto expected_config =
            log_tag_config().iteration_label_period(123);
        CHECK_NOTHROW(tree.set_config_of(parent_tag, expected_config));
        CHECK(tree.get_config_of(child_tag).iteration_label_period() ==
            expected_config.iteration_label_period());
    }
}
