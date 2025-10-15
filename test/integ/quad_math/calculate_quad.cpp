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
 * \brief Calculate quad numbers.
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
#include "num_collect/multi_double/quad.h"
#include "num_collect/multi_double/quad_math.h"

/*!
 * \brief Parse an input file.
 *
 * \tparam NumArgs Number of arguments of quad numbers per line.
 * \param[in] file_path Path to the input file.
 * \return Parsed inputs.
 *
 * Input file is written in CSV format.
 * Each line has `NumArgs` pairs of double values.
 */
template <std::size_t NumArgs>
static auto parse_input_file(const std::string& file_path)
    -> std::vector<std::array<num_collect::multi_double::quad, NumArgs>> {
    std::vector<std::array<num_collect::multi_double::quad, NumArgs>> result;

    std::ifstream stream(file_path);
    if (!stream) {
        throw num_collect::file_error(
            fmt::format("Failed to open file: {}", file_path));
    }

    std::string line;
    std::string value_str;  // Keep the allocated memory.
    while (std::getline(stream, line)) {
        std::array<double, NumArgs * 2> values{};
        std::string_view remaining = line;
        for (std::size_t i = 0; i < NumArgs * 2; ++i) {
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
        std::array<num_collect::multi_double::quad, NumArgs> quad_values;
        for (std::size_t i = 0; i < NumArgs; ++i) {
            quad_values[i] = num_collect::multi_double::quad{
                values[i * 2], values[i * 2 + 1]};
        }
        result.push_back(quad_values);
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
 * Each line has a pair of double values.
 */
static void write_output_file(const std::string& file_path,
    const std::vector<num_collect::multi_double::quad>& results) {
    std::ofstream stream(file_path);
    if (!stream) {
        throw num_collect::file_error(
            fmt::format("Failed to open file: {}", file_path));
    }

    for (const auto& quad : results) {
        stream << fmt::format("{:.13a},{:.13a}\n", quad.high(), quad.low());
    }
}

/*!
 * \brief Class to evaluate binary operators.
 */
class binary_operator_evaluator {
public:
    //! Function signature of binary operators.
    using function_signature = num_collect::multi_double::quad(
        num_collect::multi_double::quad, num_collect::multi_double::quad);

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

        std::vector<num_collect::multi_double::quad> results;
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

/*!
 * \brief Class to evaluate unary operators.
 */
class unary_operator_evaluator {
public:
    //! Function signature of unary operators.
    using function_signature = num_collect::multi_double::quad(
        num_collect::multi_double::quad);

    /*!
     * \brief Constructor.
     *
     * \param[in] function Unary operator.
     */
    explicit unary_operator_evaluator(function_signature* function)
        : function_(function) {}

    /*!
     * \brief Evaluate the unary operator.
     *
     * \param[in] input_file_path Path to the input file.
     * \param[in] output_file_path Path to the output file.
     */
    void operator()(const std::string& input_file_path,
        const std::string& output_file_path) {
        auto inputs = parse_input_file<1>(input_file_path);

        std::vector<num_collect::multi_double::quad> results;
        results.reserve(inputs.size());
        const auto start = std::chrono::steady_clock::now();
        for (const auto& input : inputs) {
            results.push_back(function_(input[0]));
        }
        const auto end = std::chrono::steady_clock::now();
        const auto duration_ms = std::chrono::duration_cast<
            std::chrono::duration<double, std::milli>>(end - start)
                                     .count();
        fmt::print("Time: {:.3e} ms\n", duration_ms);

        write_output_file(output_file_path, results);
    }

private:
    //! Unary operator.
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

    using num_collect::multi_double::quad;
    std::unordered_map<std::string,
        std::function<void(const std::string&, const std::string&)>>
        operator_map{
            {"operator+", binary_operator_evaluator([](quad a, quad b) {
                 return a + b;
             })},
            {"operator-", binary_operator_evaluator([](quad a, quad b) {
                 return a - b;
             })},
            {"operator*", binary_operator_evaluator([](quad a, quad b) {
                 return a * b;
             })},
            {"operator/", binary_operator_evaluator([](quad a, quad b) {
                 return a / b;
             })},
            {"abs", unary_operator_evaluator([](quad a) { return abs(a); })},
            {"sqrt", unary_operator_evaluator([](quad a) { return sqrt(a); })},
            {"exp", unary_operator_evaluator([](quad a) { return exp(a); })},
            {"expm1",
                unary_operator_evaluator([](quad a) { return expm1(a); })},
            {"log", unary_operator_evaluator([](quad a) { return log(a); })},
            {"log1p",
                unary_operator_evaluator([](quad a) { return log1p(a); })},
            {"log10",
                unary_operator_evaluator([](quad a) { return log10(a); })},
            {"pow", binary_operator_evaluator([](quad a, quad b) {
                 return pow(a, b);
             })},
            {"pow_int", binary_operator_evaluator([](quad a, quad b) {
                 return pow(a, static_cast<int>(b.high()));
             })},
            {"sin", unary_operator_evaluator([](quad a) { return sin(a); })},
            {"cos", unary_operator_evaluator([](quad a) { return cos(a); })},
            {"tan", unary_operator_evaluator([](quad a) { return tan(a); })},
            {"asin", unary_operator_evaluator([](quad a) { return asin(a); })},
            {"acos", unary_operator_evaluator([](quad a) { return acos(a); })},
            {"atan", unary_operator_evaluator([](quad a) { return atan(a); })},
            {"atan2", binary_operator_evaluator([](quad a, quad b) {
                 return atan2(a, b);
             })},
            {"sinh", unary_operator_evaluator([](quad a) { return sinh(a); })},
            {"cosh", unary_operator_evaluator([](quad a) { return cosh(a); })},
            {"tanh", unary_operator_evaluator([](quad a) { return tanh(a); })},
            {"asinh",
                unary_operator_evaluator([](quad a) { return asinh(a); })},
            {"acosh",
                unary_operator_evaluator([](quad a) { return acosh(a); })},
            {"floor",
                unary_operator_evaluator([](quad a) { return floor(a); })},
            {"ceil", unary_operator_evaluator([](quad a) { return ceil(a); })},
            {"trunc",
                unary_operator_evaluator([](quad a) { return trunc(a); })},
            {"round",
                unary_operator_evaluator([](quad a) { return round(a); })},
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
