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
 * \brief Test to write logs.
 */
#include <future>
#include <string>
#include <string_view>

#include <ApprovalTests.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/process.hpp>
#include <catch2/catch_test_macros.hpp>

constexpr auto writer_filepath =
    std::string_view("../bin/num_collect_test_integ_logging_write_log_writer");

TEST_CASE("logging to console") {
    SECTION("write") {
        boost::asio::io_context context;
        std::future<std::string> std_out_future;
        std::future<std::string> std_err_future;
        boost::process::child writer_process{std::string(writer_filepath),
            boost::process::std_in.close(),
            boost::process::std_out > std_out_future,
            boost::process::std_err > std_err_future, context};

        context.run();
        const auto std_out = std_out_future.get();
        const auto std_err = std_err_future.get();

        ApprovalTests::Approvals::verify(std_out,
            ApprovalTests::Options(
                [sc1 = ApprovalTests::Scrubbers::createRegexScrubber(
                     R"(\d\d\d\d-\d\d-\d\dT\d\d:\d\d:\d\d.\d\d\d\d\d\d(\+|-)\d\d\d\d)",
                     "<time-stamp>")](
                    const std::string& str) { return sc1(str); })
                .fileOptions()
                .withFileExtension(".txt"));
    }
}
