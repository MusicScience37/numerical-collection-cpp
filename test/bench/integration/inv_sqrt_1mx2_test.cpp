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
#include <exception>
#include <unordered_set>
#include <vector>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/current_invocation_context.h>
#include <stat_bench/invocation_context.h>

#include "num_collect/constants/pi.h"
#include "num_collect/integration/de_finite_integrator.h"
#include "num_collect/integration/gauss_legendre_integrator.h"
#include "num_collect/integration/gauss_legendre_kronrod_integrator.h"

STAT_BENCH_MAIN

template <typename Integrator>
void perform(const Integrator& integrator) {
    double val{};
    STAT_BENCH_MEASURE() {
        val = integrator(
            [](double x) { return 1.0 / std::sqrt(1.0 - x * x); }, -1.0, 1.0);
    };
    const double true_val = num_collect::constants::pi<double>;
    stat_bench::current_invocation_context().add_custom_output(
        "error", std::abs(val - true_val));
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("integ_inv_sqrt_1mx2", "gauss_legendre_5") {
    const auto integrator =
        num_collect::integration::gauss_legendre_integrator<double(double)>(5);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("integ_inv_sqrt_1mx2", "gauss_legendre_10") {
    const auto integrator =
        num_collect::integration::gauss_legendre_integrator<double(double)>(10);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("integ_inv_sqrt_1mx2", "gauss_legendre_20") {
    const auto integrator =
        num_collect::integration::gauss_legendre_integrator<double(double)>(20);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("integ_inv_sqrt_1mx2", "gauss_legendre_50") {
    const auto integrator =
        num_collect::integration::gauss_legendre_integrator<double(double)>(50);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("integ_inv_sqrt_1mx2", "gauss_legendre_kronrod_3") {
    const auto integrator =
        num_collect::integration::gauss_legendre_kronrod_integrator<double(
            double)>(3);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("integ_inv_sqrt_1mx2", "gauss_legendre_kronrod_5") {
    const auto integrator =
        num_collect::integration::gauss_legendre_kronrod_integrator<double(
            double)>(5);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("integ_inv_sqrt_1mx2", "gauss_legendre_kronrod_7") {
    const auto integrator =
        num_collect::integration::gauss_legendre_kronrod_integrator<double(
            double)>(7);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("integ_inv_sqrt_1mx2", "gauss_legendre_kronrod_10") {
    const auto integrator =
        num_collect::integration::gauss_legendre_kronrod_integrator<double(
            double)>(10);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("integ_inv_sqrt_1mx2", "de_finite_5") {
    const auto integrator =
        num_collect::integration::de_finite_integrator<double(double)>().points(
            5);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("integ_inv_sqrt_1mx2", "de_finite_10") {
    const auto integrator =
        num_collect::integration::de_finite_integrator<double(double)>().points(
            10);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("integ_inv_sqrt_1mx2", "de_finite_20") {
    const auto integrator =
        num_collect::integration::de_finite_integrator<double(double)>().points(
            20);
    perform(integrator);
}

// NOLINTNEXTLINE
STAT_BENCH_CASE("integ_inv_sqrt_1mx2", "de_finite_50") {
    const auto integrator =
        num_collect::integration::de_finite_integrator<double(double)>().points(
            50);
    perform(integrator);
}
