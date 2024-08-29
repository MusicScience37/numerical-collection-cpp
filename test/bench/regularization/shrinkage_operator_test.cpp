/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Benchmark for implementation of shrinkage operator.
 */
#include <random>

#include <Eigen/Core>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/invocation_context.h>

#include "num_collect/base/index_type.h"

STAT_BENCH_MAIN

class shrinkage_operator_fixture : public stat_bench::FixtureBase {
public:
    shrinkage_operator_fixture() {
        add_param<num_collect::index_type>("size")
            ->add(10)   // NOLINT
            ->add(100)  // NOLINT
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(1000)    // NOLINT
            ->add(10000)   // NOLINT
            ->add(100000)  // NOLINT
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        const num_collect::index_type size =
            context.get_param<num_collect::index_type>("size");
        std::mt19937 engine;  // NOLINT
        std::uniform_real_distribution<double> dist{-1.0, 1.0};
        target_ =
            Eigen::VectorXd::NullaryExpr(size, [&]() { return dist(engine); });
    }

    [[nodiscard]] auto target() const -> const Eigen::VectorXd& {
        return target_;
    }

    static constexpr double threshold = 0.5;

private:
    Eigen::VectorXd target_;
};

STAT_BENCH_CASE_F(shrinkage_operator_fixture, "shrink", "coefficient-wise") {
    Eigen::VectorXd result = this->target();
    STAT_BENCH_MEASURE() {
        result = this->target();
        for (num_collect::index_type i = 0; i < result.size(); ++i) {
            if (result(i) > threshold) {
                result(i) -= threshold;
            } else if (result(i) < -threshold) {
                result(i) += threshold;
            } else {
                result(i) = 0.0;
            }
        }
    };
}

STAT_BENCH_CASE_F(
    shrinkage_operator_fixture, "shrink", "coefficient-wise-openmp") {
    Eigen::VectorXd result = this->target();
    STAT_BENCH_MEASURE() {
        result = this->target();
#pragma omp parallel for
        for (num_collect::index_type i = 0; i < result.size(); ++i) {
            if (result(i) > threshold) {
                result(i) -= threshold;
            } else if (result(i) < -threshold) {
                result(i) += threshold;
            } else {
                result(i) = 0.0;
            }
        }
    };
}

STAT_BENCH_CASE_F(
    shrinkage_operator_fixture, "shrink", "coefficient-wise-openmp-sized") {
    Eigen::VectorXd result = this->target();
    constexpr num_collect::index_type parallelized_size = 10000;
    STAT_BENCH_MEASURE() {
        result = this->target();
#pragma omp parallel for if (result.size() > parallelized_size)
        for (num_collect::index_type i = 0; i < result.size(); ++i) {
            if (result(i) > threshold) {
                result(i) -= threshold;
            } else if (result(i) < -threshold) {
                result(i) += threshold;
            } else {
                result(i) = 0.0;
            }
        }
    };
}

STAT_BENCH_CASE_F(shrinkage_operator_fixture, "shrink", "vectorized") {
    Eigen::VectorXd result = this->target();
    STAT_BENCH_MEASURE() {
        const Eigen::VectorXd& target = this->target();
        result = (target.cwiseAbs().array() - threshold)
                     .cwiseMax(0.0)
                     .cwiseProduct(target.array().sign());
    };
}
