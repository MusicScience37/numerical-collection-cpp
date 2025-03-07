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

#include <optional>
#include <string>
#include <vector>

#include "num_collect/base/concepts/invocable.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logger.h"

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
     * \param[in] factory Factory of optimizers.
     * \param[in] tol_value Tolerance of function values.
     */
    template <num_collect::concepts::invocable<> OptimizerFactory>
    void measure(std::string optimizer_name, OptimizerFactory&& factory,
        double tol_value) {
        if (has_measurement_of(optimizer_name)) {
            return;
        }

        auto optimizer = factory();
        measurement data;
        data.optimizer_name = std::move(optimizer_name);
        while (true) {
            optimizer.iterate();
            if (optimizer.evaluations() >= max_evaluations_ ||
                optimizer.opt_value() <= tol_value) {
                break;
            }
            data.evaluations.push_back(optimizer.evaluations());
            data.function_values.push_back(optimizer.opt_value());
        }
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

    [[nodiscard]] auto has_measurement_of(
        const std::string& optimizer_name) const -> bool;

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
