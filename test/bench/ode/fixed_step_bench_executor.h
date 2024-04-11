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
 * \brief Common functions for diagrams.
 */
#pragma once

#include <algorithm>
#include <chrono>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

#include <Eigen/Core>
#include <fmt/format.h>

#include "diagram_common.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/norm.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/logging_mixin.h"

/*!
 * \brief Class to perform benchmark.
 */
class fixed_step_bench_executor : public num_collect::logging::logging_mixin {
public:
    /*!
     * \brief Result of benchmarks.
     */
    struct bench_result {
        //! List of solvers.
        std::vector<std::string> solver_list{};

        //! List of step sizes.
        std::vector<double> step_size_list{};

        //! List of error rates.
        std::vector<double> error_rate_list{};

        //! List of changes of energy.
        std::vector<double> energy_change_list{};

        //! List of processing time [sec].
        std::vector<double> time_list{};
    };

    //! Constructor.
    fixed_step_bench_executor();

    /*!
     * \brief Perform a benchmark.
     *
     * \tparam Problem Type of the problem.
     * \tparam Solver Type of the solver.
     * \param[in] solver_name Name of the solver.
     * \param[in] problem Problem.
     * \param[in] init_time Initial time.
     * \param[in] end_time End time.
     * \param[in] init_var Initial variable.
     * \param[in] reference Reference solution at the end.
     * \param[in] energy Reference energy,
     * \param[in] repetition Number of repetitions.
     * \param[in] step_size Step size.
     * \param[in] energy_function Function to calculate energy.
     */
    template <typename Problem, typename Solver, typename EnergyFunction>
    inline void perform(const std::string& solver_name, const Problem& problem,
        const typename Problem::scalar_type& init_time,
        const typename Problem::scalar_type& end_time,
        const typename Problem::variable_type& init_var,
        const typename Problem::variable_type& reference,
        num_collect::index_type repetition,
        const typename Problem::scalar_type& step_size,
        EnergyFunction&& energy_function) {
        solver_name_ = solver_name;
        step_size_ = step_size;

        // First trial with calculation of error.
        {
            Solver solver{problem};

            solver.step_size(step_size);

            solver.init(init_time, init_var);
            solver.solve_till(end_time);

            steps_ = solver.steps();

            const typename Problem::scalar_type min_error =
                num_collect::norm(reference) *
                std::numeric_limits<typename Problem::scalar_type>::epsilon();
            error_rate_ =
                std::max(num_collect::norm(solver.variable() - reference),
                    min_error) /
                num_collect::norm(reference);

            energy_change_ = std::abs(
                energy_function(init_var) - energy_function(solver.variable()));
        }

        const auto start_time = std::chrono::steady_clock::now();

        prevent_ordering();

        for (num_collect::index_type i = 0; i < repetition; ++i) {
            Solver solver{problem};

            solver.step_size(step_size);

            solver.init(init_time, init_var);
            solver.solve_till(end_time);

            prevent_ordering();
        }

        const auto finish_time = std::chrono::steady_clock::now();
        const auto total_processing_time = finish_time - start_time;
        const double total_processing_time_sec =
            std::chrono::duration_cast<std::chrono::duration<double>>(
                total_processing_time)
                .count();
        mean_processing_time_sec_ =
            total_processing_time_sec / static_cast<double>(repetition);

        result_.solver_list.push_back(solver_name);
        result_.step_size_list.push_back(step_size);
        result_.error_rate_list.push_back(error_rate_);
        result_.energy_change_list.push_back(energy_change_);
        result_.time_list.push_back(mean_processing_time_sec_);

        iter_logger_.write_iteration();
    }

    /*!
     * \brief Write the result.
     *
     * \param[in] problem_name Name of the problem.
     * \param[in] problem_description Description of the problem to show in
     * diagrams.
     * \param[in] result Result.
     * \param[in] output_directory Output directory.
     */
    void write_result(std::string_view problem_name,
        std::string_view problem_description,
        std::string_view output_directory);

private:
    //! Iteration logger.
    num_collect::logging::iterations::iteration_logger<> iter_logger_;

    //! Result.
    bench_result result_{};

    //! Name of the solver.
    std::string solver_name_{};

    //! Step size.
    double step_size_{};

    //! Number of steps.
    num_collect::index_type steps_{};

    //! Mean processing time [sec].
    double mean_processing_time_sec_{};

    //! Error rate.
    double error_rate_{};

    //! Change of energy.
    double energy_change_{};
};
