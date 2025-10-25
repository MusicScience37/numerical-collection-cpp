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
 * \brief Test of summation.
 */
#include <cmath>
#include <cstddef>
#include <limits>
#include <string>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/invocation_context.h>

#include "num_collect/constants/pi.h"
#include "num_collect/constants/pow.h"
#include "num_collect/multi_double/quad.h"
#include "num_collect/util/kahan_adder.h"

class summation_fixture : public stat_bench::FixtureBase {
public:
    summation_fixture() = default;

    void set_error(double val, double reference) {
        double error = std::abs(val - reference) / std::abs(reference);
        if (error < std::numeric_limits<double>::min()) {
            error = std::numeric_limits<double>::epsilon();
        }
        error_ = error;
    }

    void tear_down(stat_bench::InvocationContext& context) override {
        context.add_custom_output("error", error_);
    }

private:
    double error_{};
};

#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
constexpr std::size_t zeta4_terms = 100;
#else
constexpr std::size_t zeta4_terms = 100000;
#endif
constexpr double zeta4_reference =
    num_collect::constants::pow(num_collect::constants::pid, 4) / 90.0;

STAT_BENCH_CASE_F(summation_fixture, "sum_zeta4", "ordinary") {
    double result = 0.0;
    STAT_BENCH_MEASURE() {
        double sum = 0.0;
        for (std::size_t i = 1; i <= zeta4_terms; ++i) {
            const auto i_d = static_cast<double>(i);
            const double term = 1.0 / (i_d * i_d * i_d * i_d);
            sum += term;
        }
        result = sum;
    };
    set_error(result, zeta4_reference);
}

STAT_BENCH_CASE_F(summation_fixture, "sum_zeta4", "kahan") {
    double result = 0.0;
    STAT_BENCH_MEASURE() {
        auto sum = num_collect::util::kahan_adder<double>();
        for (std::size_t i = 1; i <= zeta4_terms; ++i) {
            const auto i_d = static_cast<double>(i);
            const double term = 1.0 / (i_d * i_d * i_d * i_d);
            sum += term;
        }
        result = sum.sum();
    };
    set_error(result, zeta4_reference);
}

STAT_BENCH_CASE_F(summation_fixture, "sum_zeta4", "quad") {
    double result = 0.0;
    STAT_BENCH_MEASURE() {
        auto sum = num_collect::multi_double::quad();
        for (std::size_t i = 1; i <= zeta4_terms; ++i) {
            const auto i_d = static_cast<double>(i);
            const double term = 1.0 / (i_d * i_d * i_d * i_d);
            sum += term;
        }
        result = sum.high();
    };
    set_error(result, zeta4_reference);
}

STAT_BENCH_MAIN
