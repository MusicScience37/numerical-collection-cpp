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
 * \brief Test of iteration_logger class.
 */
#include "num_collect/util/iteration_logger.h"

#include <sstream>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::iteration_logger") {
    num_collect::iteration_logger logger;
    std::ostringstream stream;

    SECTION("construct") {
        logger.write_to(stream);
        REQUIRE(stream.str() == "\n\n");
    }

    SECTION("set items") {
        logger["test1"] = num_collect::index_type(0);
        logger["test2"] = "value";
        constexpr double value3 = 3.141;
        logger["test3"] = value3;
        logger.write_to(stream);
        REQUIRE(stream.str() ==
            "       test1       test2       test3\n"
            "           0       value       3.141\n");
    }

    SECTION("two lines") {
        logger["test1"];
        logger.write_to(stream);
        logger["test1"] = num_collect::index_type(0);
        logger.write_to(stream);
        REQUIRE(stream.str() ==
            "       test1\n"
            "            \n"
            "           0\n");
    }

    SECTION("output period") {
        logger.log_output_period(2);
        logger.title_output_period(1);
        logger["test1"];
        logger["test1"] = num_collect::index_type(0);
        logger.write_to(stream);
        logger.write_to(stream);
        logger.write_to(stream);
        logger.write_to(stream);
        REQUIRE(stream.str() ==
            "       test1\n"
            "           0\n"
            "       test1\n"
            "           0\n");
    }
}
