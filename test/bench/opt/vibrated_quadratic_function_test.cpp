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
 * \brief Test of optimization of quadratic function with vibration.
 */
#include "num_prob_collect/opt/vibrated_quadratic_function.h"

#include <celero/Celero.h>

#include "evaluations_udm.h"
#include "iterations_udm.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/sampling_optimizer.h"

// NOLINTNEXTLINE: external library
CELERO_MAIN

class vibrated_quadratic_function_fixture : public celero::TestFixture {
public:
    vibrated_quadratic_function_fixture() = default;

    template <typename Optimizer>
    void test_optimizer(Optimizer& optimizer) {
        constexpr double tol_value = -1.0 + 1e-3;
        while (optimizer.opt_value() > tol_value) {
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
};

[[nodiscard]] auto search_region() -> std::pair<double, double> {
    constexpr double min_value = -10.0;
    constexpr double max_value = 20.0;
    return {min_value, max_value};
}

// NOLINTNEXTLINE: external library
BASELINE_F(opt_vibrated_quadratic_function, dividing_rectangles,
    vibrated_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::dividing_rectangles<
        num_prob_collect::opt::vibrated_quadratic_function>();
    const auto [lower, upper] = search_region();
    optimizer.init(lower, upper);
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(opt_vibrated_quadratic_function, sampling_optimizer,
    vibrated_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::sampling_optimizer<
        num_prob_collect::opt::vibrated_quadratic_function>();
    const auto [lower, upper] = search_region();
    optimizer.init(lower, upper);
    this->test_optimizer(optimizer);
}
