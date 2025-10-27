/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Calculate oct numbers.
 */
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <functional>
#include <ratio>
#include <unordered_map>
#include <vector>

#include <fmt/format.h>

#include "num_collect/base/exception.h"
#include "num_collect/multi_double/oct.h"

/*!
 * \brief Parse an input file.
 *
 * \tparam NumArgs Number of arguments of oct numbers per line.
 * \param[in] file_path Path to the input file.
 * \return Parsed inputs.
 *
 * Input file is written in CSV format.
 * Each line has `NumArgs` sets of four double values.
 */
template <std::size_t NumArgs>
static auto parse_input_file(const std::string& file_path)
    -> std::vector<std::array<num_collect::multi_double::oct, NumArgs>> {
    using num_collect::multi_double::oct;

    std::vector<std::array<oct, NumArgs>> result;

    std::ifstream stream(file_path);
    if (!stream) {
        throw num_collect::file_error(
            fmt::format("Failed to open file: {}", file_path));
    }

    std::string line;
    std::string value_str;  // Keep the allocated memory.
    while (std::getline(stream, line)) {
        std::array<double, NumArgs * 4> values{};
        std::string_view remaining = line;
        for (std::size_t i = 0; i < NumArgs * 4; ++i) {
            const auto comma_pos = remaining.find(',');
            if (comma_pos == std::string_view::npos) {
                value_str = remaining;
                remaining = "";
            } else {
                value_str = remaining.substr(0, comma_pos);
                remaining = remaining.substr(comma_pos + 1);
            }
            char* end_ptr = nullptr;
            values[i] = std::strtod(value_str.data(), &end_ptr);
            if (end_ptr != value_str.data() + value_str.size() ||
                !std::isfinite(values[i])) {
                throw num_collect::invalid_argument(
                    fmt::format("Invalid value: {}", value_str));
            }
        }

        std::array<oct, NumArgs> oct_values;
        for (std::size_t i = 0; i < NumArgs; ++i) {
            oct_values[i] = oct{values[i * 4], values[i * 4 + 1],
                values[i * 4 + 2], values[i * 4 + 3]};
        }
        result.push_back(oct_values);
    }
    return result;
}

/*!
 * \brief Write an output file.
 *
 * \param[in] file_path Path to the output file.
 * \param[in] results Results to write.
 *
 * Output file is written in CSV format.
 * Each line has four double values for an oct number.
 */
static void write_output_file(const std::string& file_path,
    const std::vector<num_collect::multi_double::oct>& results) {
    std::ofstream stream(file_path);
    if (!stream) {
        throw num_collect::file_error(
            fmt::format("Failed to open file: {}", file_path));
    }

    for (const auto& oct_val : results) {
        stream << fmt::format("{:.13a},{:.13a},{:.13a},{:.13a}\n",
            oct_val.term(0), oct_val.term(1), oct_val.term(2), oct_val.term(3));
    }
}

/*!
 * \brief Class to evaluate binary operators.
 */
class binary_operator_evaluator {
public:
    //! Function signature of binary operators.
    using function_signature = num_collect::multi_double::oct(
        num_collect::multi_double::oct, num_collect::multi_double::oct);

    /*!
     * \brief Constructor.
     *
     * \param[in] function Binary operator.
     */
    explicit binary_operator_evaluator(function_signature* function)
        : function_(function) {}

    /*!
     * \brief Evaluate the binary operator.
     *
     * \param[in] input_file_path Path to the input file.
     * \param[in] output_file_path Path to the output file.
     */
    void operator()(const std::string& input_file_path,
        const std::string& output_file_path) {
        auto inputs = parse_input_file<2>(input_file_path);

        std::vector<num_collect::multi_double::oct> results;
        results.reserve(inputs.size());
        const auto start = std::chrono::steady_clock::now();
        for (const auto& input : inputs) {
            results.push_back(function_(input[0], input[1]));
        }
        const auto end = std::chrono::steady_clock::now();
        const auto duration_ms = std::chrono::duration_cast<
            std::chrono::duration<double, std::milli>>(end - start)
                                     .count();
        fmt::print("Time: {:.3e} ms\n", duration_ms);

        write_output_file(output_file_path, results);
    }

private:
    //! Binary operator.
    function_signature* function_;
};

auto main(int argc, char** argv) -> int {
    if (argc != 4) {
        fmt::print(stderr, "Usage: {} <operator> <input> <output>\n", argv[0]);
        return 1;
    }
    const std::string operator_name = argv[1];
    const std::string input_file_path = argv[2];
    const std::string output_file_path = argv[3];

    using num_collect::multi_double::oct;
    std::unordered_map<std::string,
        std::function<void(const std::string&, const std::string&)>>
        operator_map{
            {"operator+",
                binary_operator_evaluator([](oct a, oct b) { return a + b; })},
            {"operator-",
                binary_operator_evaluator([](oct a, oct b) { return a - b; })},
            {"operator*",
                binary_operator_evaluator([](oct a, oct b) { return a * b; })},
            {"operator/",
                binary_operator_evaluator([](oct a, oct b) { return a / b; })},
        };

    const auto iter = operator_map.find(operator_name);
    if (iter == operator_map.end()) {
        fmt::print(stderr, "Unknown operator: {}\n", operator_name);
        return 1;
    }
    const auto& evaluator = iter->second;

    evaluator(input_file_path, output_file_path);

    return 0;
}
