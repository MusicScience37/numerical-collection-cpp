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

#include <celero/Celero.h>

#include "evaluations_udm.h"
#include "iterations_udm.h"
#include "num_collect/opt/dividing_rectangles.h"

// NOLINTNEXTLINE: external library
CELERO_MAIN

constexpr std::int64_t samples = 30;
#ifndef NDEBUG
constexpr std::int64_t iterations = 1;
#else
constexpr std::int64_t iterations = 100;
#endif

class shekel_function_fixture : public celero::TestFixture {
public:
    shekel_function_fixture() = default;

    [[nodiscard]] auto getExperimentValues() const
        -> std::vector<celero::TestFixture::ExperimentValue> override {
        std::vector<celero::TestFixture::ExperimentValue> problem_space;
        problem_space.emplace_back(5, iterations);   // NOLINT
        problem_space.emplace_back(7, iterations);   // NOLINT
        problem_space.emplace_back(10, iterations);  // NOLINT
        return problem_space;
    }

    void setUp(
        const celero::TestFixture::ExperimentValue& experiment_value) override {
        num_terms_ = static_cast<int>(experiment_value.Value);
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
        constexpr num_collect::index_type max_evaluations = 100000;
        if (is_failure_) {
            return;
        }
        while (optimizer.opt_value() > value_bound) {
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

    int num_terms_{};

    bool is_failure_{false};
};

// NOLINTNEXTLINE: external library
BASELINE_F(opt_shekel_function, dividing_rectangles, shekel_function_fixture,
    samples, iterations) {
    auto optimizer = num_collect::opt::dividing_rectangles<
        num_prob_collect::opt::shekel_function>(this->function());
    const auto [lower, upper] = shekel_function_fixture::search_region();
    optimizer.init(lower, upper);
    this->test_optimizer(optimizer);
}
