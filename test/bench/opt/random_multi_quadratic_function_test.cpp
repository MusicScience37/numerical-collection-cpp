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
 * \brief Benchmark of optimization using multi-variate quadratic functions.
 */
#include "num_prob_collect/opt/random_multi_quadratic_function.h"

#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/invocation_context.h>
#include <stat_bench/param/parameter_value_vector.h>
#include <stat_bench/plot_option.h>

#include "function_value_history_writer.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/opt/bfgs_optimizer.h"
#include "num_collect/opt/concepts/optimizer.h"
#include "num_collect/opt/conjugate_gradient_optimizer.h"
#include "num_collect/opt/dfp_optimizer.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/downhill_simplex.h"
#include "num_collect/opt/heuristic_global_optimizer.h"
#include "num_collect/opt/newton_optimizer.h"
#include "num_collect/opt/steepest_descent.h"

constexpr double tol_value = 1e-4;
constexpr num_collect::index_type max_evaluations = 10000;

class random_multi_quadratic_function_fixture : public stat_bench::FixtureBase {
public:
    random_multi_quadratic_function_fixture() = default;

    template <num_collect::opt::concepts::optimizer Optimizer>
    void test_optimizer(std::size_t sample_index, Optimizer& optimizer) {
        while (optimizer.opt_value() > tol_value) {
            if (optimizer.evaluations() >= max_evaluations) {
                throw num_collect::algorithm_failure("Failed to converge.");
                return;
            }
            optimizer.iterate();
        }
        constexpr std::size_t thread_index = 0;
        iterations_stat_->add(thread_index, sample_index,
            static_cast<double>(optimizer.iterations()));
        evaluations_stat_->add(thread_index, sample_index,
            static_cast<double>(optimizer.evaluations()));
    }

    template <std::invocable<std::size_t> OptimizerFactory>
    void test_optimizer(
        OptimizerFactory&& factory, const std::string& optimizer_name) {
        function_value_history_writer::instance().measure_multiple(
            fmt::format("random_multi_quadratic_function_{}", dimensions_),
            optimizer_name, factory, tol_value,
            stat_bench::current_invocation_context().samples());

        STAT_BENCH_MEASURE_INDEXED(
            /*thread_index*/, sample_index, /*iteration_index*/) {
            auto optimizer = factory(sample_index);
            test_optimizer(sample_index, optimizer);
        };
    }

    void setup(stat_bench::InvocationContext& context) override {
        dimensions_ = context.get_param<num_collect::index_type>("dimension");

        const std::size_t num_samples = context.samples();
        functions_.reserve(num_samples);
        functions_.clear();
        std::generate_n(std::back_inserter(functions_), num_samples,
            num_prob_collect::opt::random_multi_quadratic_function_generator(
                dimensions_));

        iterations_stat_ = context.add_custom_stat("iterations");
        evaluations_stat_ = context.add_custom_stat("evaluations");
    }

    [[nodiscard]] auto init_variable() const -> Eigen::VectorXd {
        return Eigen::VectorXd::Zero(dimensions_);
    }

    [[nodiscard]] auto function(std::size_t i) const
        -> const num_prob_collect::opt::random_multi_quadratic_function& {
        return functions_.at(i);
    }

    [[nodiscard]] auto search_region() const
        -> std::pair<Eigen::VectorXd, Eigen::VectorXd> {
        constexpr double min_value = num_prob_collect::opt::
            random_multi_quadratic_function_generator::min_variable;
        constexpr double max_value = num_prob_collect::opt::
            random_multi_quadratic_function_generator::max_variable;
        return {Eigen::VectorXd::Constant(dimensions_, min_value),
            Eigen::VectorXd::Constant(dimensions_, max_value)};
    }

private:
    num_collect::index_type dimensions_{1};

    std::vector<num_prob_collect::opt::random_multi_quadratic_function>
        functions_;

    std::shared_ptr<stat_bench::stat::CustomStatOutput> iterations_stat_;

    std::shared_ptr<stat_bench::stat::CustomStatOutput> evaluations_stat_;
};

class small_random_multi_quadratic_function_fixture
    : public random_multi_quadratic_function_fixture {
public:
    small_random_multi_quadratic_function_fixture() {
        add_param<num_collect::index_type>("dimension")
            ->add(2)  // NOLINT
            ->add(5)  // NOLINT
            ;
    }
};

class large_random_multi_quadratic_function_fixture
    : public random_multi_quadratic_function_fixture {
public:
    large_random_multi_quadratic_function_fixture() {
        add_param<num_collect::index_type>("dimension")
            ->add(2)   // NOLINT
            ->add(5)   // NOLINT
            ->add(10)  // NOLINT
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(20)   // NOLINT
            ->add(50)   // NOLINT
            ->add(100)  // NOLINT
            ->add(200)  // NOLINT
#endif
            ;
    }
};

STAT_BENCH_GROUP("opt_random_multi_quadratic_function")
    .add_parameter_to_time_line_plot(
        "dimension", stat_bench::PlotOption::log_parameter)
    .add_parameter_to_time_violin_plot("dimension")
    .add_parameter_to_output_line_plot("dimension", "evaluations",
        stat_bench::PlotOption::log_parameter |
            stat_bench::PlotOption::log_output);

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(large_random_multi_quadratic_function_fixture,
    "opt_random_multi_quadratic_function", "steepest_descent") {
    test_optimizer(
        [this](std::size_t sample_index) {
            auto optimizer = num_collect::opt::steepest_descent<
                num_prob_collect::opt::random_multi_quadratic_function>(
                this->function(sample_index));
            optimizer.init(this->init_variable());
            return optimizer;
        },
        "steepest_descent");
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(small_random_multi_quadratic_function_fixture,
    "opt_random_multi_quadratic_function", "downhill_simplex") {
    test_optimizer(
        [this](std::size_t sample_index) {
            auto optimizer = num_collect::opt::downhill_simplex<
                num_prob_collect::opt::random_multi_quadratic_function>(
                this->function(sample_index));
            optimizer.init(this->init_variable());
            return optimizer;
        },
        "downhill_simplex");
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(large_random_multi_quadratic_function_fixture,
    "opt_random_multi_quadratic_function", "newton_optimizer") {
    STAT_BENCH_MEASURE_INDEXED(
        /*thread_index*/, sample_index, /*iteration_index*/) {
        auto optimizer = num_collect::opt::newton_optimizer<
            num_prob_collect::opt::random_multi_quadratic_function>(
            this->function(sample_index));
        optimizer.init(this->init_variable());
        this->test_optimizer(sample_index, optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(large_random_multi_quadratic_function_fixture,
    "opt_random_multi_quadratic_function", "dfp_optimizer") {
    test_optimizer(
        [this](std::size_t sample_index) {
            auto optimizer = num_collect::opt::dfp_optimizer<
                num_prob_collect::opt::random_multi_quadratic_function>(
                this->function(sample_index));
            optimizer.init(this->init_variable());
            return optimizer;
        },
        "dfp_optimizer");
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(large_random_multi_quadratic_function_fixture,
    "opt_random_multi_quadratic_function", "bfgs_optimizer") {
    test_optimizer(
        [this](std::size_t sample_index) {
            auto optimizer = num_collect::opt::bfgs_optimizer<
                num_prob_collect::opt::random_multi_quadratic_function>(
                this->function(sample_index));
            optimizer.init(this->init_variable());
            return optimizer;
        },
        "bfgs_optimizer");
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(large_random_multi_quadratic_function_fixture,
    "opt_random_multi_quadratic_function", "conjugate_gradient") {
    test_optimizer(
        [this](std::size_t sample_index) {
            auto optimizer = num_collect::opt::conjugate_gradient_optimizer<
                num_prob_collect::opt::random_multi_quadratic_function>(
                this->function(sample_index));
            optimizer.init(this->init_variable());
            return optimizer;
        },
        "conjugate_gradient");
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(small_random_multi_quadratic_function_fixture,
    "opt_random_multi_quadratic_function", "dividing_rectangles") {
    test_optimizer(
        [this](std::size_t sample_index) {
            auto optimizer = num_collect::opt::dividing_rectangles<
                num_prob_collect::opt::random_multi_quadratic_function>(
                this->function(sample_index));
            const auto [lower, upper] = this->search_region();
            optimizer.init(lower, upper);
            return optimizer;
        },
        "dividing_rectangles");
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(small_random_multi_quadratic_function_fixture,
    "opt_random_multi_quadratic_function", "heuristic_global_optimizer") {
    test_optimizer(
        [this](std::size_t sample_index) {
            auto optimizer = num_collect::opt::heuristic_global_optimizer<
                num_prob_collect::opt::random_multi_quadratic_function>(
                this->function(sample_index));
            const auto [lower, upper] = this->search_region();
            optimizer.init(lower, upper);
            return optimizer;
        },
        "heuristic_global_optimizer");
}

auto main(int argc, const char** argv) -> int {
    function_value_history_writer::instance().set_max_evaluations(
        max_evaluations);
    return main_with_function_value_history_writer(argc, argv);
}
