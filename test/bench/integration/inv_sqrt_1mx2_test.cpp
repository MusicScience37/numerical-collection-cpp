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
 * \brief Test of integration of \f$ 1/\sqrt{1-x^2} \f$.
 */
#include <cmath>

#include <celero/Celero.h>

#include "log_error_udm.h"
#include "num_collect/constants/pi.h"
#include "num_collect/integration/de_finite_integrator.h"
#include "num_collect/integration/gauss_legendre_integrator.h"

// NOLINTNEXTLINE: external library
CELERO_MAIN

class inv_sqrt_1mx2_fixture : public celero::TestFixture {
public:
    inv_sqrt_1mx2_fixture() = default;

    template <typename Integrator>
    void perform(const Integrator& integrator) {
        const double val = integrator(
            [](double x) { return 1.0 / std::sqrt(1.0 - x * x); }, -1.0, 1.0);
        const double true_val = num_collect::constants::pi<double>;
        log_error_->addValue(std::log10(std::abs(val - true_val)));
    }

    [[nodiscard]] auto getUserDefinedMeasurements() const -> std::vector<
        std::shared_ptr<celero::UserDefinedMeasurement>> override {
        return {log_error_};
    }

private:
    std::shared_ptr<log_error_udm> log_error_{
        std::make_shared<log_error_udm>()};
};

// NOLINTNEXTLINE: initialization is required before benchmark.
static const auto gl_5_integrator =
    num_collect::integration::gauss_legendre_integrator<double>(5);

// NOLINTNEXTLINE: external library
BASELINE_F(integ_inv_sqrt_1mx2, gl_5, inv_sqrt_1mx2_fixture, 0, 0) {
    perform(gl_5_integrator);
}

// NOLINTNEXTLINE: initialization is required before benchmark.
static const auto gl_10_integrator =
    num_collect::integration::gauss_legendre_integrator<double>(10);

// NOLINTNEXTLINE: external library
BENCHMARK_F(integ_inv_sqrt_1mx2, gl_10, inv_sqrt_1mx2_fixture, 0, 0) {
    perform(gl_10_integrator);
}

// NOLINTNEXTLINE: initialization is required before benchmark.
static const auto gl_20_integrator =
    num_collect::integration::gauss_legendre_integrator<double>(20);

// NOLINTNEXTLINE: external library
BENCHMARK_F(integ_inv_sqrt_1mx2, gl_20, inv_sqrt_1mx2_fixture, 0, 0) {
    perform(gl_20_integrator);
}

// NOLINTNEXTLINE: initialization is required before benchmark.
static const auto gl_50_integrator =
    num_collect::integration::gauss_legendre_integrator<double>(50);

// NOLINTNEXTLINE: external library
BENCHMARK_F(integ_inv_sqrt_1mx2, gl_50, inv_sqrt_1mx2_fixture, 0, 0) {
    perform(gl_50_integrator);
}

// NOLINTNEXTLINE: initialization is required before benchmark.
static const auto de_5_integrator =
    num_collect::integration::de_finite_integrator<double>().points(5);

// NOLINTNEXTLINE: external library
BENCHMARK_F(integ_inv_sqrt_1mx2, de_5, inv_sqrt_1mx2_fixture, 0, 0) {
    perform(de_5_integrator);
}

// NOLINTNEXTLINE: initialization is required before benchmark.
static const auto de_10_integrator =
    num_collect::integration::de_finite_integrator<double>().points(10);

// NOLINTNEXTLINE: external library
BENCHMARK_F(integ_inv_sqrt_1mx2, de_10, inv_sqrt_1mx2_fixture, 0, 0) {
    perform(de_10_integrator);
}

// NOLINTNEXTLINE: initialization is required before benchmark.
static const auto de_20_integrator =
    num_collect::integration::de_finite_integrator<double>().points(20);

// NOLINTNEXTLINE: external library
BENCHMARK_F(integ_inv_sqrt_1mx2, de_20, inv_sqrt_1mx2_fixture, 0, 0) {
    perform(de_20_integrator);
}

// NOLINTNEXTLINE: initialization is required before benchmark.
static const auto de_50_integrator =
    num_collect::integration::de_finite_integrator<double>().points(50);

// NOLINTNEXTLINE: external library
BENCHMARK_F(integ_inv_sqrt_1mx2, de_50, inv_sqrt_1mx2_fixture, 0, 0) {
    perform(de_50_integrator);
}
