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
 * \brief Definition of auto_diff_fixture class.
 */
#include <cmath>

#include <Eigen/Core>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/invocation_context.h>

class auto_diff_fixture : public stat_bench::FixtureBase {
public:
    auto_diff_fixture() = default;

    void check_error(double actual, double expected) {
        error_ = std::abs(actual - expected);
    }

    template <typename Derived>
    void check_error(const Eigen::MatrixBase<Derived>& actual,
        const Eigen::MatrixBase<Derived>& expected) {
        error_ = (actual - expected).norm();
    }

    void tear_down(stat_bench::InvocationContext& context) override {
        context.add_custom_output("error", error_);
    }

private:
    double error_{};
};
