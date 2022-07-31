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
// IWYU pragma: no_include "boost/asio/detail/impl/scheduler.ipp"
// IWYU pragma: no_include "boost/asio/impl/any_io_executor.ipp"
// IWYU pragma: no_include "boost/asio/impl/io_context.ipp"
// IWYU pragma: no_include "boost/process/child.hpp"
// IWYU pragma: no_include "boost/process/detail/child_decl.hpp"
// IWYU pragma: no_include "boost/process/io.hpp"

#include <functional>
#include <future>
#include <istream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <ApprovalTests.hpp>
#include <boost/asio/io_context.hpp>  // IWYU pragma: keep
#include <boost/process.hpp>          // IWYU pragma: keep
#include <catch2/catch_test_macros.hpp>

constexpr auto writer_filepath =
    std::string_view("../bin/num_collect_test_integ_logging_write_log_writer");

static auto read_file(const std::string& filepath) -> std::string {
    std::ifstream stream(filepath);
    REQUIRE(stream);
    return std::string(std::istreambuf_iterator<char>(stream),
        std::istreambuf_iterator<char>());
}

TEST_CASE("write_logs") {
    SECTION("to console") {
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

        CHECK(std_err == "");  // NOLINT
    }

    SECTION("to file") {
        const auto log_filepath =
            std::string("num_collect_test_integ_logging_write_log");
        boost::asio::io_context context;
        std::future<std::string> std_out_future;
        std::future<std::string> std_err_future;
        boost::process::child writer_process{std::string(writer_filepath), "-o",
            log_filepath, boost::process::std_in.close(),
            boost::process::std_out > std_out_future,
            boost::process::std_err > std_err_future, context};

        context.run();
        const auto std_out = std_out_future.get();
        const auto std_err = std_err_future.get();

        CHECK(std_out == "");  // NOLINT
        CHECK(std_err == "");  // NOLINT

        ApprovalTests::Approvals::verify(read_file(log_filepath),
            ApprovalTests::Options(
                [sc1 = ApprovalTests::Scrubbers::createRegexScrubber(
                     R"(\d\d\d\d-\d\d-\d\dT\d\d:\d\d:\d\d.\d\d\d\d\d\d(\+|-)\d\d\d\d)",
                     "<time-stamp>")](
                    const std::string& str) { return sc1(str); })
                .fileOptions()
                .withFileExtension(".txt"));
    }
}
