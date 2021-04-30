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

#include "iterations_udm.h"
#include "num_collect/opt/bfgs_optimizer.h"
#include "num_collect/opt/dfp_optimizer.h"
#include "num_collect/opt/downhill_simplex.h"
#include "num_collect/opt/newton_optimizer.h"
#include "num_collect/opt/steepest_descent.h"

// NOLINTNEXTLINE: external library
CELERO_MAIN

class multi_quadratic_function_fixture : public celero::TestFixture {
public:
    multi_quadratic_function_fixture() = default;

    template <typename Optimizer>
    void test_optimizer(Optimizer& optimizer) {
        constexpr double tol_value = 1e-6;
        while (optimizer.opt_value() > tol_value) {
            optimizer.iterate();
        }
        iterations_->addValue(optimizer.iterations());
    }

    [[nodiscard]] auto getUserDefinedMeasurements() const -> std::vector<
        std::shared_ptr<celero::UserDefinedMeasurement>> override {
        return {iterations_};
    }

private:
    std::shared_ptr<iterations_udm> iterations_{
        std::make_shared<iterations_udm>()};
};

// NOLINTNEXTLINE: external library
BASELINE_F(opt_multi_quadratic_function, steepest_descent,
    multi_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::steepest_descent<
        num_prob_collect::opt::multi_quadratic_function>();
    const Eigen::VectorXd init_var =
        (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
    optimizer.init(init_var);
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(opt_multi_quadratic_function, downhill_simplex,
    multi_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::downhill_simplex<
        num_prob_collect::opt::multi_quadratic_function>();
    const Eigen::VectorXd init_var =
        (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
    optimizer.init(init_var);
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(opt_multi_quadratic_function, newton_optimizer,
    multi_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::newton_optimizer<
        num_prob_collect::opt::multi_quadratic_function>();
    const Eigen::VectorXd init_var =
        (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
    optimizer.init(init_var);
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(opt_multi_quadratic_function, dfp_optimizer,
    multi_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::dfp_optimizer<
        num_prob_collect::opt::multi_quadratic_function>();
    const Eigen::VectorXd init_var =
        (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
    optimizer.init(init_var);
    this->test_optimizer(optimizer);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(opt_multi_quadratic_function, bfgs_optimizer,
    multi_quadratic_function_fixture, 0, 0) {
    auto optimizer = num_collect::opt::bfgs_optimizer<
        num_prob_collect::opt::multi_quadratic_function>();
    const Eigen::VectorXd init_var =
        (Eigen::VectorXd(3) << 0.0, 1.0, 2.0).finished();
    optimizer.init(init_var);
    this->test_optimizer(optimizer);
}
