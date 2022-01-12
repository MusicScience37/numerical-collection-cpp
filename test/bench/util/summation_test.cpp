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

#include <celero/Celero.h>

#include "log_error_udm.h"
#include "num_collect/constants/pi.h"
#include "num_collect/multi_double/quad.h"
#include "num_collect/util/kahan_adder.h"

// NOLINTNEXTLINE: external library
CELERO_MAIN

class summation_fixture : public celero::TestFixture {
public:
    summation_fixture() = default;

    void set_error(double val, double reference) {
        double error = std::abs(val - reference) / std::abs(reference);
        if (error < std::numeric_limits<double>::min()) {
            error = std::numeric_limits<double>::epsilon();
        }
        log_error_->addValue(std::log10(error));
    }

    [[nodiscard]] auto getUserDefinedMeasurements() const -> std::vector<
        std::shared_ptr<celero::UserDefinedMeasurement>> override {
        return {log_error_};
    }

private:
    std::shared_ptr<log_error_udm> log_error_{
        std::make_shared<log_error_udm>()};
};

#ifndef NDEBUG
constexpr std::size_t zeta4_terms = 100;
#else
constexpr std::size_t zeta4_terms = 100000;
#endif
// NOLINTNEXTLINE: won't throw
const double zeta4_reference = std::pow(num_collect::constants::pid, 4) / 90.0;

// NOLINTNEXTLINE: external library
BASELINE_F(sum_zeta4, ordinary, summation_fixture, 0, 0) {
    double sum = 0.0;
    for (std::size_t i = 1; i <= zeta4_terms; ++i) {
        const auto i_d = static_cast<double>(i);
        const double term = 1.0 / (i_d * i_d * i_d * i_d);
        sum += term;
    }
    set_error(sum, zeta4_reference);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(sum_zeta4, kahan, summation_fixture, 0, 0) {
    auto sum = num_collect::util::kahan_adder<double>();
    for (std::size_t i = 1; i <= zeta4_terms; ++i) {
        const auto i_d = static_cast<double>(i);
        const double term = 1.0 / (i_d * i_d * i_d * i_d);
        sum += term;
    }
    set_error(sum, zeta4_reference);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(sum_zeta4, quad, summation_fixture, 0, 0) {
    auto sum = num_collect::multi_double::quad();
    for (std::size_t i = 1; i <= zeta4_terms; ++i) {
        const auto i_d = static_cast<double>(i);
        const double term = 1.0 / (i_d * i_d * i_d * i_d);
        sum += term;
    }
    set_error(sum.high(), zeta4_reference);
}
