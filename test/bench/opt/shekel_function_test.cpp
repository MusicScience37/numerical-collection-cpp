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
 * \brief Test of optimization of Shekel function.
 */
#include "num_prob_collect/opt/shekel_function.h"

#include <stat_bench/bench/invocation_context.h>
#include <stat_bench/benchmark_macros.h>

#include "num_collect/base/exception.h"
#include "num_collect/opt/adaptive_diagonal_curves.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/heuristic_global_optimizer.h"

STAT_BENCH_MAIN

class shekel_function_fixture : public stat_bench::FixtureBase {
public:
    shekel_function_fixture() {
        add_param<int>("num_terms")
            ->add(5)   // NOLINT
            ->add(7)   // NOLINT
            ->add(10)  // NOLINT
            ;
    }

    void setup(stat_bench::bench::InvocationContext& context) override {
        num_terms_ = context.get_param<int>("num_terms");
    }

    [[nodiscard]] auto function() const
        -> num_prob_collect::opt::shekel_function {
        return num_prob_collect::opt::shekel_function(num_terms_);
    }

    [[nodiscard]] static auto search_region()
        -> std::pair<Eigen::Vector4d, Eigen::Vector4d> {
        constexpr double min_value = 0.0;
        constexpr double max_value = 10.0;
        return {Eigen::Vector4d::Constant(min_value),
            Eigen::Vector4d::Constant(max_value)};
    }

    [[nodiscard]] auto minimum_value() const -> double {
        constexpr double elem = 4.0;
        auto func = function();
        func.evaluate_on(Eigen::Vector4d::Constant(elem));
        return func.value();
    }

    template <typename Optimizer>
    void test_optimizer(Optimizer& optimizer) {
        constexpr double tol_value = 1e-4;
        const auto value_bound = minimum_value() + tol_value;
#ifndef NDEBUG
        constexpr num_collect::index_type max_evaluations = 1000;
#else
        constexpr num_collect::index_type max_evaluations = 100000;
#endif
        while (optimizer.opt_value() > value_bound) {
            if (optimizer.evaluations() >= max_evaluations) {
                throw num_collect::algorithm_failure("Failed to converge.");
                return;
            }
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

    int num_terms_{};
};

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(
    shekel_function_fixture, "opt_shekel_function", "dividing_rectangles") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::dividing_rectangles<
            num_prob_collect::opt::shekel_function>(this->function());
        const auto [lower, upper] = shekel_function_fixture::search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(shekel_function_fixture, "opt_shekel_function",
    "adaptive_diagonal_curves") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::adaptive_diagonal_curves<
            num_prob_collect::opt::shekel_function>(this->function());
        const auto [lower, upper] = shekel_function_fixture::search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(shekel_function_fixture, "opt_shekel_function",
    "heuristic_global_optimizer") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::heuristic_global_optimizer<
            num_prob_collect::opt::shekel_function>(this->function());
        const auto [lower, upper] = shekel_function_fixture::search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(shekel_function_fixture, "opt_shekel_function",
    "heuristic_global_optimizer_light") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::heuristic_global_optimizer<
            num_prob_collect::opt::shekel_function>(this->function());
        const auto [lower, upper] = shekel_function_fixture::search_region();
        optimizer.light_mode();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(shekel_function_fixture, "opt_shekel_function",
    "heuristic_global_optimizer_heavy") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::heuristic_global_optimizer<
            num_prob_collect::opt::shekel_function>(this->function());
        const auto [lower, upper] = shekel_function_fixture::search_region();
        optimizer.heavy_mode();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}
