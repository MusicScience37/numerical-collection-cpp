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

#include <celero/Celero.h>

#include "evaluations_udm.h"
#include "iterations_udm.h"
#include "num_collect/opt/bfgs_optimizer.h"
#include "num_collect/opt/dfp_optimizer.h"
#include "num_collect/opt/downhill_simplex.h"
#include "num_collect/opt/steepest_descent.h"

// NOLINTNEXTLINE: external library
CELERO_MAIN

class powell4_function_fixture : public celero::TestFixture {
public:
    powell4_function_fixture() = default;

    template <typename Optimizer>
    void test_optimizer(Optimizer& optimizer) {
        constexpr double tol_value = 1e-2;
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

[[nodiscard]] auto init_var() -> Eigen::Vector4d {
    // NOLINTNEXTLINE
    return (Eigen::Vector4d() << 1.0, -2.0, -3.0, 2.0).finished();
}

// NOLINTNEXTLINE: external library
BASELINE_F(
    opt_powell4_function, steepest_descent, powell4_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::steepest_descent<
        num_prob_collect::opt::powell4_function>();
    optimizer.init(init_var());
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(
    opt_powell4_function, downhill_simplex, powell4_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::downhill_simplex<
        num_prob_collect::opt::powell4_function>();
    optimizer.init(init_var());
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(
    opt_powell4_function, dfp_optimizer, powell4_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::dfp_optimizer<
        num_prob_collect::opt::powell4_function>();
    optimizer.init(init_var());
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(
    opt_powell4_function, bfgs_optimizer, powell4_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::bfgs_optimizer<
        num_prob_collect::opt::powell4_function>();
    optimizer.init(init_var());
    this->test_optimizer(optimizer);
}
