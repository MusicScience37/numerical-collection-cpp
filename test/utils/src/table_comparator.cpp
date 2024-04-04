/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Implementation of table_comparator class.
 */
#include "table_comparator.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <string>
#include <utility>

#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>

#include "num_collect/base/index_type.h"

table_comparator::table_comparator(std::vector<bool> checked_columns,
    num_collect::index_type num_rows, num_collect::index_type precision)
    : checked_columns_(std::move(checked_columns)),
      num_rows_(num_rows),
      precision_(std::min(precision, max_precision)),
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
      tol_rel_error_(std::pow(10.0, -precision)),
      // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
      tol_abs_error_(std::pow(10.0, -precision)) {}

auto table_comparator::contentsAreEquivalent(
    std::string received_path, std::string approved_path) const -> bool {
    std::ifstream received_stream{received_path};
    std::ifstream approved_stream{approved_path};
    if (received_stream.fail() || approved_stream.fail()) {
        INFO("Failed to open files of approval tests.");
        return false;
    }

    // Ignore labels at the first line.
    constexpr std::streamsize max_size = 10000;
    received_stream.ignore(max_size, '\n');
    approved_stream.ignore(max_size, '\n');
    if (received_stream.fail() || approved_stream.fail()) {
        INFO("Failed to read files of approval tests.");
        return false;
    }

    for (num_collect::index_type i = 0; i < num_rows_; ++i) {
        for (bool is_checked_column : checked_columns_) {
            double received_value{};
            received_stream >> received_value;
            double approved_value{};
            approved_stream >> approved_value;
            if (received_stream.fail() || approved_stream.fail()) {
                INFO("Failed to read line " << i);
                return false;
            }
            if (is_checked_column) {
                const double error = std::abs(received_value - approved_value);
                const bool is_almost_equal =
                    (error < tol_rel_error_ * std::abs(approved_value)) ||
                    (error < tol_abs_error_);
                if (!is_almost_equal) {
                    INFO("Line " << i << " is different.");
                    return false;
                }
            }
        }
    }

    return true;
}
