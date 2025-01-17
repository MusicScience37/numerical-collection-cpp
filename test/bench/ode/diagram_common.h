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
#include <unordered_map>
#include <vector>

#include <Eigen/Core>
#include <fmt/format.h>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/norm.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/step_size_limits.h"

constexpr auto benchmark_tag = num_collect::logging::log_tag_view("benchmark");

/*!
 * \brief Prevent ordering of instructions over the position calling this
 * function.
 */
void prevent_ordering();

/*!
 * \brief Create a constant.
 *
 * \tparam T Type.
 * \param[in] scalar Value.
 * \return Value.
 */
template <num_collect::base::concepts::real_scalar T>
[[nodiscard]] inline auto create_constant_variable(
    const T& /*variable*/, const T& scalar) {
    return scalar;
}

/*!
 * \brief Create a constant vector.
 *
 * \tparam T Type.
 * \param[in] variable Variable.
 * \param[in] scalar Scalar in the resulting vector.
 * \return Vector.
 */
template <num_collect::base::concepts::real_scalar_dense_vector T>
[[nodiscard]] inline auto create_constant_variable(
    const T& variable, const typename T::Scalar& scalar) {
    return T::Constant(variable.size(), scalar);
}

/*!
 * \brief Create a map of line styles.
 *
 * \return Map.
 */
[[nodiscard]] auto create_line_dash_map()
    -> std::unordered_map<std::string, std::string>;

/*!
 * \brief Class to perform benchmark.
 */
class bench_executor : public num_collect::logging::logging_mixin {
public:
    /*!
     * \brief Result of benchmarks.
     */
    struct bench_result {
        //! List of solvers.
        std::vector<std::string> solver_list{};

        //! List of error tolerances.
        std::vector<double> tolerance_list{};

        //! List of error rates.
        std::vector<double> error_rate_list{};

        //! List of processing time [sec].
        std::vector<double> time_list{};
    };

    //! Constructor.
    bench_executor();

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
     * \param[in] repetition Number of repetitions.
     * \param[in] tol Tolerance of errors.
     * \param[out] result Result.
     */
    template <typename Problem, typename Solver>
    inline void perform(const std::string& solver_name, const Problem& problem,
        const typename Problem::scalar_type& init_time,
        const typename Problem::scalar_type& end_time,
        const typename Problem::variable_type& init_var,
        const typename Problem::variable_type& reference,
        num_collect::index_type repetition,
        const typename Problem::scalar_type& tol) {
        solver_name_ = solver_name;
        tol_ = tol;

        // First trial with calculation of error.
        {
            Solver solver{problem};

            solver.step_size_controller().limits(step_size_limits_);

            solver.tolerances(num_collect::ode::error_tolerances<
                typename Problem::variable_type>()
                    .tol_rel_error(create_constant_variable(init_var, tol))
                    .tol_abs_error(create_constant_variable(init_var, tol)));

            solver.init(init_time, init_var);
            solver.solve_till(end_time);

            const typename Problem::scalar_type min_error =
                num_collect::norm(reference) *
                std::numeric_limits<typename Problem::scalar_type>::epsilon();
            steps_ = solver.steps();
            error_rate_ =
                std::max(num_collect::norm(solver.variable() - reference),
                    min_error) /
                num_collect::norm(reference);
        }

        const auto start_time = std::chrono::steady_clock::now();

        prevent_ordering();

        for (num_collect::index_type i = 0; i < repetition; ++i) {
            Solver solver{problem};

            solver.step_size_controller().limits(step_size_limits_);

            solver.tolerances(num_collect::ode::error_tolerances<
                typename Problem::variable_type>()
                    .tol_rel_error(tol)
                    .tol_abs_error(tol));

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
        result_.tolerance_list.push_back(tol);
        result_.error_rate_list.push_back(error_rate_);
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

    /*!
     * \brief Change the limits of step sizes.
     *
     * \param[in] limits Limits.
     */
    void step_size_limits(
        const num_collect::ode::step_size_limits<double>& limits);

private:
    //! Iteration logger.
    num_collect::logging::iterations::iteration_logger<> iter_logger_;

    //! Result.
    bench_result result_{};

    //! Name of the solver.
    std::string solver_name_{};

    //! Tolerance.
    double tol_{};

    //! Number of steps.
    num_collect::index_type steps_{};

    //! Mean processing time [sec].
    double mean_processing_time_sec_{};

    //! Error rate.
    double error_rate_{};

    //! Limits of step sizes.
    num_collect::ode::step_size_limits<double> step_size_limits_{};
};

/*!
 * \brief Configure logging for benchmarks.
 */
void configure_logging();
