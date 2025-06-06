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
 * \brief Test of optimization of Powell function in 4 dimensions.
 */
#include "num_prob_collect/opt/powell4_function.h"

#include <utility>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/invocation_context.h>

#include "function_value_history_writer.h"
#include "num_collect/base/index_type.h"
#include "num_collect/opt/bfgs_optimizer.h"
#include "num_collect/opt/concepts/optimizer.h"
#include "num_collect/opt/dfp_optimizer.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/downhill_simplex.h"
#include "num_collect/opt/heuristic_global_optimizer.h"
#include "num_collect/opt/steepest_descent.h"

constexpr double tol_value = 1e-2;

class powell4_function_fixture : public stat_bench::FixtureBase {
public:
    powell4_function_fixture() = default;

    template <num_collect::opt::concepts::optimizer Optimizer>
    void test_optimizer(Optimizer& optimizer) {
        while (optimizer.opt_value() > tol_value) {
            optimizer.iterate();
        }
        iterations_ = optimizer.iterations();
        evaluations_ = optimizer.evaluations();
    }

    template <std::invocable<> OptimizerFactory>
    void test_optimizer(
        OptimizerFactory&& factory, const std::string& optimizer_name) {
        function_value_history_writer::instance().measure(
            "powell4_function", optimizer_name, factory, tol_value);

        STAT_BENCH_MEASURE() {
            auto optimizer = factory();
            test_optimizer(optimizer);
        };
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

[[nodiscard]] auto init_var() -> Eigen::Vector4d {
    // NOLINTNEXTLINE
    return (Eigen::Vector4d() << 1.0, -2.0, -3.0, 2.0).finished();
}

[[nodiscard]] auto search_region()
    -> std::pair<Eigen::Vector4d, Eigen::Vector4d> {
    constexpr double min_value = -4.0;
    constexpr double max_value = 5.0;
    return {Eigen::Vector4d::Constant(min_value),
        Eigen::Vector4d::Constant(max_value)};
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    powell4_function_fixture, "opt_powell4_function", "steepest_descent") {
    test_optimizer(
        [] {
            auto optimizer = num_collect::opt::steepest_descent<
                num_prob_collect::opt::powell4_function>();
            optimizer.init(init_var());
            return optimizer;
        },
        "steepest_descent");
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    powell4_function_fixture, "opt_powell4_function", "downhill_simplex") {
    test_optimizer(
        [] {
            auto optimizer = num_collect::opt::downhill_simplex<
                num_prob_collect::opt::powell4_function>();
            optimizer.init(init_var());
            return optimizer;
        },
        "downhill_simplex");
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    powell4_function_fixture, "opt_powell4_function", "dfp_optimizer") {
    test_optimizer(
        [] {
            auto optimizer = num_collect::opt::dfp_optimizer<
                num_prob_collect::opt::powell4_function>();
            optimizer.init(init_var());
            return optimizer;
        },
        "dfp_optimizer");
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    powell4_function_fixture, "opt_powell4_function", "bfgs_optimizer") {
    test_optimizer(
        [] {
            auto optimizer = num_collect::opt::bfgs_optimizer<
                num_prob_collect::opt::powell4_function>();
            optimizer.init(init_var());
            return optimizer;
        },
        "bfgs_optimizer");
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    powell4_function_fixture, "opt_powell4_function", "dividing_rectangles") {
    test_optimizer(
        [] {
            auto optimizer = num_collect::opt::dividing_rectangles<
                num_prob_collect::opt::powell4_function>();
            const auto [lower, upper] = search_region();
            optimizer.init(lower, upper);
            return optimizer;
        },
        "dividing_rectangles");
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(powell4_function_fixture, "opt_powell4_function",
    "heuristic_global_optimizer") {
    test_optimizer(
        [] {
            auto optimizer = num_collect::opt::heuristic_global_optimizer<
                num_prob_collect::opt::powell4_function>();
            const auto [lower, upper] = search_region();
            optimizer.init(lower, upper);
            return optimizer;
        },
        "heuristic_global_optimizer");
}

auto main(int argc, const char** argv) -> int {
    return main_with_function_value_history_writer(argc, argv);
}
