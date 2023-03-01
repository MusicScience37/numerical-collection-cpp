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
 * \brief Test of optimization of quadratic function.
 */
#include "num_prob_collect/opt/quadratic_function.h"

#include <utility>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/invocation_context.h>

#include "num_collect/base/index_type.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/golden_section_search.h"
#include "num_collect/opt/heuristic_global_optimizer.h"
#include "num_collect/opt/sampling_optimizer.h"

STAT_BENCH_MAIN

class quadratic_function_fixture : public stat_bench::FixtureBase {
public:
    quadratic_function_fixture() = default;

    template <typename Optimizer>
    void test_optimizer(Optimizer& optimizer) {
        constexpr double tol_value = 1e-3;
        while (optimizer.opt_value() > tol_value) {
            optimizer.iterate();
        }
        iterations_ = optimizer.iterations();
        evaluations_ = optimizer.evaluations();
    }

    void tear_down(stat_bench::InvocationContext& context) override {
        context.add_custom_output(
            "iterations", static_cast<double>(iterations_));
        context.add_custom_output(
            "evaluations", static_cast<double>(evaluations_));
    }

private:
    num_collect::index_type iterations_{};

    num_collect::index_type evaluations_{};
};

[[nodiscard]] auto search_region() -> std::pair<double, double> {
    constexpr double min_value = -10.0;
    constexpr double max_value = 20.0;
    return {min_value, max_value};
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(quadratic_function_fixture, "opt_quadratic_function",
    "golden_section_search") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::golden_section_search<
            num_prob_collect::opt::quadratic_function>();
        const auto [lower, upper] = search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(quadratic_function_fixture, "opt_quadratic_function",
    "dividing_rectangles") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::dividing_rectangles<
            num_prob_collect::opt::quadratic_function>();
        const auto [lower, upper] = search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(quadratic_function_fixture, "opt_quadratic_function",
    "sampling_optimizer") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::sampling_optimizer<
            num_prob_collect::opt::quadratic_function>();
        const auto [lower, upper] = search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(quadratic_function_fixture, "opt_quadratic_function",
    "heuristic_global_optimizer") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::heuristic_global_optimizer<
            num_prob_collect::opt::quadratic_function>();
        const auto [lower, upper] = search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}
