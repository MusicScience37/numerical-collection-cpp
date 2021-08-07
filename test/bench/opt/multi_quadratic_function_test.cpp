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

#include <celero/Celero.h>

#include "evaluations_udm.h"
#include "iterations_udm.h"
#include "num_collect/opt/bfgs_optimizer.h"
#include "num_collect/opt/conjugate_gradient_optimizer.h"
#include "num_collect/opt/dfp_optimizer.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/downhill_simplex.h"
#include "num_collect/opt/newton_optimizer.h"
#include "num_collect/opt/steepest_descent.h"

// NOLINTNEXTLINE: external library
CELERO_MAIN

class multi_quadratic_function_fixture : public celero::TestFixture {
public:
    multi_quadratic_function_fixture() = default;

    [[nodiscard]] auto getExperimentValues() const
        -> std::vector<celero::TestFixture::ExperimentValue> override {
#ifndef NDEBUG
        constexpr auto dimensions_vector =
            std::array<std::int64_t, 3>{2, 5, 10};
        constexpr std::int64_t coeff = 10;
#else
        constexpr auto dimensions_vector =
            std::array<std::int64_t, 6>{2, 5, 10, 20, 50, 100};
        constexpr std::int64_t coeff = 1000;
#endif
        std::vector<celero::TestFixture::ExperimentValue> problem_space;
        problem_space.reserve(dimensions_vector.size());
        for (const auto dimensions : dimensions_vector) {
            problem_space.emplace_back(dimensions, coeff / dimensions);
        }
        return problem_space;
    }

    void setUp(
        const celero::TestFixture::ExperimentValue& experiment_value) override {
        dimensions_ = experiment_value.Value;
        is_failure_ = false;
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
        if (is_failure_) {
            return;
        }
        while (optimizer.opt_value() > tol_value) {
            if (optimizer.evaluations() >= max_evaluations) {
                is_failure_ = true;
                return;
            }
            optimizer.iterate();
        }
        iterations_->addValue(optimizer.iterations());
        evaluations_->addValue(optimizer.evaluations());
    }

    [[nodiscard]] auto getUserDefinedMeasurements() const -> std::vector<
        std::shared_ptr<celero::UserDefinedMeasurement>> override {
        return {iterations_, evaluations_};
    }

private:
    std::shared_ptr<iterations_udm> iterations_{
        std::make_shared<iterations_udm>()};

    std::shared_ptr<evaluations_udm> evaluations_{
        std::make_shared<evaluations_udm>()};

    num_collect::index_type dimensions_{1};

    bool is_failure_{false};
};

// NOLINTNEXTLINE: external library
BASELINE_F(opt_multi_quadratic_function, steepest_descent,
    multi_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::steepest_descent<
        num_prob_collect::opt::multi_quadratic_function>();
    optimizer.init(this->init_variable());
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(opt_multi_quadratic_function, downhill_simplex,
    multi_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::downhill_simplex<
        num_prob_collect::opt::multi_quadratic_function>();
    optimizer.init(this->init_variable());
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(opt_multi_quadratic_function, newton_optimizer,
    multi_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::newton_optimizer<
        num_prob_collect::opt::multi_quadratic_function>();
    optimizer.init(this->init_variable());
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(opt_multi_quadratic_function, dfp_optimizer,
    multi_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::dfp_optimizer<
        num_prob_collect::opt::multi_quadratic_function>();
    optimizer.init(this->init_variable());
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(opt_multi_quadratic_function, bfgs_optimizer,
    multi_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::bfgs_optimizer<
        num_prob_collect::opt::multi_quadratic_function>();
    optimizer.init(this->init_variable());
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(opt_multi_quadratic_function, conjugate_gradient_optimizer,
    multi_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::conjugate_gradient_optimizer<
        num_prob_collect::opt::multi_quadratic_function>();
    optimizer.init(this->init_variable());
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(opt_multi_quadratic_function, dividing_rectangles,
    multi_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::dividing_rectangles<
        num_prob_collect::opt::multi_quadratic_function>();
    const auto [lower, upper] = this->search_region();
    optimizer.init(lower, upper);
    this->test_optimizer(optimizer);
}
