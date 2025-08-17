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
 * \brief Definition of function_value_history_writer class.
 */
#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <limits>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "num_collect/base/index_type.h"

/*!
 * \brief Class to write history of function values.
 */
class function_value_history_writer {
public:
    /*!
     * \brief Get the instance of this class.
     *
     * \return Instance.
     */
    [[nodiscard]] static auto instance() -> function_value_history_writer&;

    /*!
     * \brief Measure an optimizer.
     *
     * \tparam OptimizerFactory Type of a factory of optimizers.
     * \param[in] problem_name Name of the problem.
     * \param[in] optimizer_name Name of the optimizer.
     * \param[in] factory Factory of optimizers.
     * \param[in] tol_value Tolerance of function values.
     */
    template <std::invocable<> OptimizerFactory>
    void measure(std::string problem_name, std::string optimizer_name,
        OptimizerFactory&& factory, double tol_value, double min_value = 0.0) {
        remove_old_measurement_of(problem_name, optimizer_name);

        auto optimizer = factory();
        measurement data;
        data.problem_name = std::move(problem_name);
        data.optimizer_name = std::move(optimizer_name);
        if (optimizer.evaluations() > 0) {
            data.evaluations.push_back(optimizer.evaluations());
            data.function_values.push_back(optimizer.opt_value() - min_value);
        }
        while (true) {
            optimizer.iterate();
            if (optimizer.evaluations() >= max_evaluations_) {
                data.evaluations.push_back(max_evaluations_);
                data.function_values.push_back(
                    optimizer.opt_value() - min_value);
                break;
            }
            data.evaluations.push_back(optimizer.evaluations());
            data.function_values.push_back(optimizer.opt_value() - min_value);
            if (optimizer.opt_value() <= min_value + tol_value) {
                break;
            }
        }
        measurements_.push_back(std::move(data));
    }

    /*!
     * \brief Measure an optimizer.
     *
     * \tparam OptimizerFactory Type of a factory of optimizers.
     * \param[in] problem_name Name of the problem.
     * \param[in] optimizer_name Name of the optimizer.
     * \param[in] factory Factory of optimizers.
     * \param[in] tol_value Tolerance of function values.
     * \param[in] num_samples Number of samples.
     */
    template <std::invocable<std::size_t> OptimizerFactory>
    void measure_multiple(std::string problem_name, std::string optimizer_name,
        OptimizerFactory&& factory, double tol_value, std::size_t num_samples) {
        remove_old_measurement_of(problem_name, optimizer_name);

        std::vector<double> evaluations_to_values_lower;
        std::vector<double> evaluations_to_values_upper;
        evaluations_to_values_lower.resize(
            static_cast<std::size_t>(max_evaluations_),
            std::numeric_limits<double>::max());
        evaluations_to_values_upper.resize(
            static_cast<std::size_t>(max_evaluations_),
            std::numeric_limits<double>::min());
        for (std::size_t i = 0; i < num_samples; ++i) {
            auto optimizer = factory(i);
            while (optimizer.opt_value() > tol_value) {
                if (optimizer.evaluations() >= max_evaluations_) {
                    const auto evaluations =
                        static_cast<std::size_t>(max_evaluations_ - 1);
                    evaluations_to_values_lower[evaluations] =
                        std::min(evaluations_to_values_lower[evaluations],
                            optimizer.opt_value());
                    evaluations_to_values_upper[evaluations] =
                        std::max(evaluations_to_values_upper[evaluations],
                            optimizer.opt_value());
                    break;
                }

                const auto evaluations =
                    static_cast<std::size_t>(optimizer.evaluations());
                evaluations_to_values_lower[evaluations] =
                    std::min(evaluations_to_values_lower[evaluations],
                        optimizer.opt_value());
                evaluations_to_values_upper[evaluations] =
                    std::max(evaluations_to_values_upper[evaluations],
                        optimizer.opt_value());

                optimizer.iterate();
            }
        }

        measurement data;
        data.problem_name = std::move(problem_name);
        data.optimizer_name = std::move(optimizer_name);

        double value = std::numeric_limits<double>::max();
        data.evaluations.reserve(evaluations_to_values_lower.size());
        data.function_values.reserve(evaluations_to_values_lower.size());
        for (std::size_t i = 0; i < evaluations_to_values_lower.size() - 1;
            ++i) {
            const auto evaluations = static_cast<num_collect::index_type>(i);
            const auto& function_value = evaluations_to_values_lower[i];
            if (function_value < value) {
                data.evaluations.push_back(evaluations);
                data.function_values.push_back(function_value);
                value = function_value;
            }
        }
        {
            const auto evaluations = static_cast<num_collect::index_type>(
                evaluations_to_values_lower.size() - 1);
            const auto& function_value = evaluations_to_values_lower.back();
            if (function_value <= value) {
                data.evaluations.push_back(evaluations);
                data.function_values.push_back(function_value);
            }
        }

        value = std::numeric_limits<double>::min();
        data.evaluations_upper.emplace();
        data.function_values_upper.emplace();
        data.evaluations_upper->reserve(evaluations_to_values_lower.size());
        data.function_values_upper->reserve(evaluations_to_values_lower.size());
        for (std::size_t i = evaluations_to_values_upper.size() - 1; i > 0;
            --i) {
            const auto evaluations = static_cast<num_collect::index_type>(i);
            const auto& function_value = evaluations_to_values_upper[i];
            if (function_value > value) {
                data.evaluations_upper->push_back(evaluations);
                data.function_values_upper->push_back(function_value);
                value = function_value;
            }
        }
        std::ranges::reverse(*data.evaluations_upper);
        std::ranges::reverse(*data.function_values_upper);

        measurements_.push_back(std::move(data));
    }

    /*!
     * \brief Write measurements to a file.
     *
     * \param[in] file_path File path.
     */
    void write_measurements(const std::string& file_path) const;

    /*!
     * \brief Set the maximum number of function evaluations.
     *
     * \param[in] value Value.
     */
    void set_max_evaluations(num_collect::index_type value) {
        max_evaluations_ = value;
    }

    /*!
     * \brief Struct of a result of a measurement for an optimizer.
     */
    struct measurement {
        //! Name of the problem.
        std::string problem_name;

        //! Name of the optimizer.
        std::string optimizer_name;

        //! Number of function evaluations. (Lower bound for statistical output.)
        std::vector<num_collect::index_type> evaluations;

        //! Function values. (Lower bound for statistical output.)
        std::vector<double> function_values;

        //! Number of function evaluations for the upper bound.
        std::optional<std::vector<num_collect::index_type>> evaluations_upper;

        //! Function values for the upper bound.
        std::optional<std::vector<double>> function_values_upper;
    };

private:
    /*!
     * \brief Constructor.
     */
    function_value_history_writer();

    void remove_old_measurement_of(
        const std::string& problem_name, const std::string& optimizer_name);

    //! Measurements.
    std::vector<measurement> measurements_;

    //! Default value of the maximum number of function evaluations.
    static constexpr num_collect::index_type default_max_evaluations = 1000;

    //! Maximum number of function evaluations.
    num_collect::index_type max_evaluations_{default_max_evaluations};
};

/*!
 * \brief Main function with function_value_history_writer.
 *
 * \param[in] argc Number of arguments.
 * \param[in] argv Arguments.
 * \return Exit code.
 */
auto main_with_function_value_history_writer(int argc, const char** argv)
    -> int;
