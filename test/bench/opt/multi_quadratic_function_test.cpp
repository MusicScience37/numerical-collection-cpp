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
#include "num_prob_collect/opt/multi_quadratic_function.h"

#include <memory>
#include <string_view>
#include <type_traits>
#include <utility>

#include <stat_bench/bench/invocation_context.h>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/param/parameter_value_vector.h>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/opt/bfgs_optimizer.h"
#include "num_collect/opt/conjugate_gradient_optimizer.h"
#include "num_collect/opt/dfp_optimizer.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/downhill_simplex.h"
#include "num_collect/opt/heuristic_global_optimizer.h"
#include "num_collect/opt/newton_optimizer.h"
#include "num_collect/opt/steepest_descent.h"

STAT_BENCH_MAIN

class multi_quadratic_function_fixture : public stat_bench::FixtureBase {
public:
    multi_quadratic_function_fixture() {
        add_param<num_collect::index_type>("dimension")
            ->add(2)   // NOLINT
            ->add(5)   // NOLINT
            ->add(10)  // NOLINT
#ifdef NDEBUG
            ->add(20)   // NOLINT
            ->add(50)   // NOLINT
            ->add(100)  // NOLINT
#endif
            ;
    }

    void setup(stat_bench::bench::InvocationContext& context) override {
        dimensions_ = context.get_param<num_collect::index_type>("dimension");
    }

    [[nodiscard]] auto init_variable() const -> Eigen::VectorXd {
        constexpr double min_value = 1.0;
        constexpr double max_value = 2.0;
        return Eigen::VectorXd::LinSpaced(dimensions_, min_value, max_value);
    }

    [[nodiscard]] auto search_region() const
        -> std::pair<Eigen::VectorXd, Eigen::VectorXd> {
        constexpr double min_value = -1.0;
        constexpr double max_value = 2.0;
        return {Eigen::VectorXd::Constant(dimensions_, min_value),
            Eigen::VectorXd::Constant(dimensions_, max_value)};
    }

    template <typename Optimizer>
    void test_optimizer(Optimizer& optimizer) {
        constexpr double tol_value = 1e-4;
        constexpr num_collect::index_type max_evaluations = 10000;
        while (optimizer.opt_value() > tol_value) {
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
    num_collect::index_type dimensions_{1};

    num_collect::index_type iterations_{};

    num_collect::index_type evaluations_{};
};

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(multi_quadratic_function_fixture,
    "opt_multi_quadratic_function", "steepest_descent") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::steepest_descent<
            num_prob_collect::opt::multi_quadratic_function>();
        optimizer.init(this->init_variable());
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(multi_quadratic_function_fixture,
    "opt_multi_quadratic_function", "downhill_simplex") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::downhill_simplex<
            num_prob_collect::opt::multi_quadratic_function>();
        optimizer.init(this->init_variable());
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(multi_quadratic_function_fixture,
    "opt_multi_quadratic_function", "newton_optimizer") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::newton_optimizer<
            num_prob_collect::opt::multi_quadratic_function>();
        optimizer.init(this->init_variable());
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(multi_quadratic_function_fixture,
    "opt_multi_quadratic_function", "dfp_optimizer") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::dfp_optimizer<
            num_prob_collect::opt::multi_quadratic_function>();
        optimizer.init(this->init_variable());
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(multi_quadratic_function_fixture,
    "opt_multi_quadratic_function", "bfgs_optimizer") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::bfgs_optimizer<
            num_prob_collect::opt::multi_quadratic_function>();
        optimizer.init(this->init_variable());
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(multi_quadratic_function_fixture,
    "opt_multi_quadratic_function", "conjugate_gradient") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::conjugate_gradient_optimizer<
            num_prob_collect::opt::multi_quadratic_function>();
        optimizer.init(this->init_variable());
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(multi_quadratic_function_fixture,
    "opt_multi_quadratic_function", "dividing_rectangles") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::dividing_rectangles<
            num_prob_collect::opt::multi_quadratic_function>();
        const auto [lower, upper] = this->search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(multi_quadratic_function_fixture,
    "opt_multi_quadratic_function", "heuristic_global_optimizer") {
    STAT_BENCH_MEASURE() {
        auto optimizer = num_collect::opt::heuristic_global_optimizer<
            num_prob_collect::opt::multi_quadratic_function>();
        const auto [lower, upper] = this->search_region();
        optimizer.init(lower, upper);
        this->test_optimizer(optimizer);
    };
}
