/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Benchmark of single-variate optimization problems with multiple
 * optima.
 */
#include "num_prob_collect/opt/single_variate_multi_optima_function.h"

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/invocation_context.h>
#include <stat_bench/stat/custom_stat_output.h>

#include "num_collect/base/index_type.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/golden_section_search.h"
#include "num_collect/opt/heuristic_global_optimizer.h"
#include "num_collect/opt/sampling_optimizer.h"

STAT_BENCH_MAIN

class single_variate_multiple_optima_function_fixture
    : public stat_bench::FixtureBase {
public:
    single_variate_multiple_optima_function_fixture() = default;

    template <typename Optimizer>
    void test_optimizer(std::size_t sample_index, Optimizer& optimizer) {
        constexpr double tol_value = 1e-3;
        while (optimizer.opt_value() > tol_value) {
            optimizer.iterate();
        }
        constexpr std::size_t thread_index = 0;
        iterations_stat_->add(thread_index, sample_index,
            static_cast<double>(optimizer.iterations()));
        evaluations_stat_->add(thread_index, sample_index,
            static_cast<double>(optimizer.evaluations()));
    }

    void setup(stat_bench::InvocationContext& context) override {
        const std::size_t num_samples = context.samples();
        functions_.reserve(num_samples);
        functions_.clear();
        std::generate_n(std::back_inserter(functions_), num_samples,
            num_prob_collect::opt::
                random_single_variate_multi_optima_function_generator());

        iterations_stat_ = context.add_custom_stat("iterations");
        evaluations_stat_ = context.add_custom_stat("evaluations");
    }

    [[nodiscard]] auto function(std::size_t i) const
        -> const num_prob_collect::opt::single_variate_multi_optima_function& {
        return functions_.at(i);
    }

    [[nodiscard]] static auto search_region() -> std::pair<double, double> {
        return {num_prob_collect::opt::
                    random_single_variate_multi_optima_function_generator::
                        min_variable,
            num_prob_collect::opt::
                random_single_variate_multi_optima_function_generator::
                    max_variable};
    }

private:
    std::vector<num_prob_collect::opt::single_variate_multi_optima_function>
        functions_;

    std::shared_ptr<stat_bench::stat::CustomStatOutput> iterations_stat_;

    std::shared_ptr<stat_bench::stat::CustomStatOutput> evaluations_stat_;
};

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(single_variate_multiple_optima_function_fixture,
    "opt_single_variate_multiple_optima_function", "dividing_rectangles") {
    STAT_BENCH_MEASURE_INDEXED(
        /*thread_index*/, sample_index, /*iteration_index*/) {
        auto optimizer = num_collect::opt::dividing_rectangles<
            num_prob_collect::opt::single_variate_multi_optima_function>(
            this->function(sample_index));
        const auto [lower, upper] = search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(sample_index, optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(single_variate_multiple_optima_function_fixture,
    "opt_single_variate_multiple_optima_function", "sampling_optimizer") {
    STAT_BENCH_MEASURE_INDEXED(
        /*thread_index*/, sample_index, /*iteration_index*/) {
        auto optimizer = num_collect::opt::sampling_optimizer<
            num_prob_collect::opt::single_variate_multi_optima_function>(
            this->function(sample_index));
        const auto [lower, upper] = search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(sample_index, optimizer);
    };
}
