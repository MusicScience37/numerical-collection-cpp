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
 * \brief Test of log_sink_factory_table class.
 */
#include "num_collect/logging/config/log_sink_factory_table.h"

#include <memory>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include "../mock_log_sink.h"
#include "mock_log_sink_factory.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/sinks/log_sink_base.h"
#include "trompeloeil_catch2.h"

TEST_CASE("num_collect::logging::config::log_sink_factory_table") {
    using num_collect::logging::config::default_log_sink_name;
    using num_collect::logging::config::log_sink_factory_table;
    using num_collect::logging::impl::get_default_log_sink;
    using num_collect::logging::sinks::log_sink_base;
    using num_collect_test::logging::mock_log_sink;
    using num_collect_test::logging::config::mock_log_sink_factory;
    using trompeloeil::_;

    SECTION("create log sinks") {
        log_sink_factory_table table;

        const auto name1 = std::string("sink1");
        const auto sink1 = std::make_shared<mock_log_sink>();
        const auto factory1 = std::make_shared<mock_log_sink_factory>();
        table.append(name1, factory1);

        const auto name2 = std::string("sink2");
        const auto sink2 = std::make_shared<mock_log_sink>();
        const auto factory2 = std::make_shared<mock_log_sink_factory>();
        table.append(name2, factory2);

        {
            // NOLINTNEXTLINE
            REQUIRE_CALL(*factory1, create(_)).TIMES(1).RETURN(sink1);

            std::shared_ptr<log_sink_base> sink;
            CHECK_NOTHROW(sink = table.get(name1));
            CHECK(static_cast<void*>(sink.get()) ==
                static_cast<void*>(sink1.get()));
        }

        {
            // NOLINTNEXTLINE
            REQUIRE_CALL(*factory2, create(_)).TIMES(1).RETURN(sink2);

            std::shared_ptr<log_sink_base> sink;
            CHECK_NOTHROW(sink = table.get(name2));
            CHECK(static_cast<void*>(sink.get()) ==
                static_cast<void*>(sink2.get()));
        }
    }

    SECTION("get cached log sinks") {
        log_sink_factory_table table;

        const auto name1 = std::string("sink1");
        const auto sink1 = std::make_shared<mock_log_sink>();
        const auto factory1 = std::make_shared<mock_log_sink_factory>();
        table.append(name1, factory1);

        const auto name2 = std::string("sink2");
        const auto sink2 = std::make_shared<mock_log_sink>();
        const auto factory2 = std::make_shared<mock_log_sink_factory>();
        table.append(name2, factory2);

        {
            // NOLINTNEXTLINE
            REQUIRE_CALL(*factory1, create(_)).TIMES(1).RETURN(sink1);

            std::shared_ptr<log_sink_base> sink;
            CHECK_NOTHROW(sink = table.get(name1));
            CHECK(static_cast<void*>(sink.get()) ==
                static_cast<void*>(sink1.get()));
        }

        {
            // NOLINTNEXTLINE
            FORBID_CALL(*factory1, create(_));

            std::shared_ptr<log_sink_base> sink;
            CHECK_NOTHROW(sink = table.get(name1));
            CHECK(static_cast<void*>(sink.get()) ==
                static_cast<void*>(sink1.get()));
        }
    }

    SECTION("get default log sink") {
        log_sink_factory_table table;

        std::shared_ptr<log_sink_base> sink;
        CHECK_NOTHROW(sink = table.get(std::string{default_log_sink_name}));
        CHECK(static_cast<void*>(sink.get()) ==
            static_cast<void*>(get_default_log_sink().get()));
    }

    SECTION("duplicate name of log sinks") {
        log_sink_factory_table table;

        const auto name1 = std::string("sink1");
        const auto sink1 = std::make_shared<mock_log_sink>();
        const auto factory1 = std::make_shared<mock_log_sink_factory>();
        CHECK_NOTHROW(table.append(name1, factory1));
        CHECK_THROWS(table.append(name1, factory1));
    }

    SECTION("create log sinks with references") {
        log_sink_factory_table table;

        const auto name1 = std::string("sink1");
        const auto sink1 = std::make_shared<mock_log_sink>();
        const auto factory1 = std::make_shared<mock_log_sink_factory>();
        table.append(name1, factory1);

        const auto name2 = std::string("sink2");
        const auto sink2 = std::make_shared<mock_log_sink>();
        const auto factory2 = std::make_shared<mock_log_sink_factory>();
        table.append(name2, factory2);

        {
            std::shared_ptr<log_sink_base> inner_sink;
            REQUIRE_CALL(*factory1, create(_))
                .TIMES(1)
                .LR_SIDE_EFFECT(inner_sink = _1.get(name2))  // NOLINT
                .RETURN(sink1);                              // NOLINT
            REQUIRE_CALL(*factory2, create(_))
                .TIMES(1)
                .RETURN(sink2);  // NOLINT

            std::shared_ptr<log_sink_base> sink;
            CHECK_NOTHROW(sink = table.get(name1));
            CHECK(static_cast<void*>(sink.get()) ==
                static_cast<void*>(sink1.get()));
            CHECK(static_cast<void*>(inner_sink.get()) ==
                static_cast<void*>(sink2.get()));
        }
    }

    SECTION("create log sinks with looped references") {
        log_sink_factory_table table;

        const auto name1 = std::string("sink1");
        const auto sink1 = std::make_shared<mock_log_sink>();
        const auto factory1 = std::make_shared<mock_log_sink_factory>();
        table.append(name1, factory1);

        const auto name2 = std::string("sink2");
        const auto sink2 = std::make_shared<mock_log_sink>();
        const auto factory2 = std::make_shared<mock_log_sink_factory>();
        table.append(name2, factory2);

        {
            ALLOW_CALL(*factory1, create(_))
                .SIDE_EFFECT((void)_1.get(name2))  // NOLINT
                .RETURN(sink1);                    // NOLINT
            ALLOW_CALL(*factory2, create(_))
                .SIDE_EFFECT((void)_1.get(name1))  // NOLINT
                .RETURN(sink2);                    // NOLINT

            CHECK_THROWS(table.get(name1));
            CHECK_THROWS(table.get(name2));
        }
    }
}
