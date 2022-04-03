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
 * \brief Test of optimization of Rosenbrock function in 3 dimensions.
 */
#include "num_prob_collect/opt/rosenbrock_function.h"

#include <stat_bench/bench/invocation_context.h>
#include <stat_bench/benchmark_macros.h>

#include "num_collect/opt/bfgs_optimizer.h"
#include "num_collect/opt/dfp_optimizer.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/downhill_simplex.h"
#include "num_collect/opt/heuristic_global_optimizer.h"
#include "num_collect/opt/steepest_descent.h"

STAT_BENCH_MAIN

class rosenbrock_function_fixture : public stat_bench::FixtureBase {
public:
    rosenbrock_function_fixture() = default;

    template <typename Optimizer>
    void test_optimizer(Optimizer& optimizer) {
        constexpr double tol_value = 1e-2;
        while (optimizer.opt_value() > tol_value) {
            optimizer.iterate();
        }
        iterations_ = optimizer.iterations();
        evaluations_ = optimizer.evaluations();
    }

    void tear_down(stat_bench::bench::InvocationContext& context) override {
        context.add_custom_output(
            "iterations", static_cast<double>(iterations_));
        context.add_custom_output(
            "evaluations", static_cast<double>(evaluations_));
    }

private:
    num_collect::index_type iterations_{};

    num_collect::index_type evaluations_{};
};

[[nodiscard]] auto init_var() -> Eigen::Vector3d {
    // NOLINTNEXTLINE
    return (Eigen::Vector3d() << 0, 0, 0).finished();
}

[[nodiscard]] auto search_region()
    -> std::pair<Eigen::Vector3d, Eigen::Vector3d> {
    constexpr double min_value = -5.0;
    constexpr double max_value = 10.0;
    return {Eigen::Vector3d::Constant(min_value),
        Eigen::Vector3d::Constant(max_value)};
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(rosenbrock_function_fixture, "opt_rosenbrock_function",
    "steepest_descent") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::steepest_descent<
            num_prob_collect::opt::rosenbrock_function>();
        optimizer.init(init_var());
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(rosenbrock_function_fixture, "opt_rosenbrock_function",
    "downhill_simplex") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::downhill_simplex<
            num_prob_collect::opt::rosenbrock_function>();
        optimizer.init(init_var());
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    rosenbrock_function_fixture, "opt_rosenbrock_function", "dfp_optimizer") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::dfp_optimizer<
            num_prob_collect::opt::rosenbrock_function>();
        optimizer.init(init_var());
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    rosenbrock_function_fixture, "opt_rosenbrock_function", "bfgs_optimizer") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::bfgs_optimizer<
            num_prob_collect::opt::rosenbrock_function>();
        optimizer.init(init_var());
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(rosenbrock_function_fixture, "opt_rosenbrock_function",
    "dividing_rectangles") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::dividing_rectangles<
            num_prob_collect::opt::rosenbrock_function>();
        const auto [lower, upper] = search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(rosenbrock_function_fixture, "opt_rosenbrock_function",
    "heuristic_global_optimizer") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::heuristic_global_optimizer<
            num_prob_collect::opt::rosenbrock_function>();
        const auto [lower, upper] = search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}
