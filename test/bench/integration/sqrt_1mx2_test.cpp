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
 * \brief Test of integration of \f$ \sqrt{1-x^2} \f$.
 */
#include <cmath>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/current_invocation_context.h>
#include <stat_bench/invocation_context.h>

#include "fixtures.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/pi.h"
#include "num_collect/integration/de_finite_integrator.h"
#include "num_collect/integration/gauss_legendre_integrator.h"
#include "num_collect/integration/gauss_legendre_kronrod_integrator.h"
#include "num_collect/integration/tanh_finite_integrator.h"

STAT_BENCH_MAIN

template <typename Integrator>
void perform(const Integrator& integrator) {
    double val{};
    STAT_BENCH_MEASURE() {
        val = integrator(
            [](double x) { return std::sqrt(1.0 - x * x); }, -1.0, 1.0);
    };
    const double true_val = 0.5 * num_collect::constants::pi<double>;
    stat_bench::current_invocation_context().add_custom_output(
        "error", std::abs(val - true_val));
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(gauss_legendre_fixture, "integ_sqrt_1mx2", "gauss_legendre") {
    const auto degree = stat_bench::current_invocation_context()
                            .get_param<num_collect::index_type>("degree");
    const auto integrator =
        num_collect::integration::gauss_legendre_integrator<double(double)>(
            degree);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(gauss_legendre_kronrod_fixture, "integ_sqrt_1mx2",
    "gauss_legendre_kronrod") {
    const auto degree = stat_bench::current_invocation_context()
                            .get_param<num_collect::index_type>("degree");
    const auto integrator =
        num_collect::integration::gauss_legendre_kronrod_integrator<double(
            double)>(degree);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(de_finite_fixture, "integ_sqrt_1mx2", "de_finite") {
    const auto points = stat_bench::current_invocation_context()
                            .get_param<num_collect::index_type>("points");
    num_collect::integration::de_finite_integrator<double(double)> integrator;
    integrator.points(points);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(tanh_finite_fixture, "integ_sqrt_1mx2", "tanh_finite") {
    const auto points = stat_bench::current_invocation_context()
                            .get_param<num_collect::index_type>("points");
    num_collect::integration::tanh_finite_integrator<double(double)> integrator;
    integrator.points(points);
    perform(integrator);
}
