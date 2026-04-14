/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of radau2a_table class.
 */
#include "num_collect/ode/runge_kutta/impl/radau2a_table.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "eigen_approx.h"
#include "num_collect/ode/runge_kutta/radau2a3_formula.h"
#include "num_collect/ode/runge_kutta/radau2a5_formula.h"
#include "num_prob_collect/ode/exponential_problem.h"

TEST_CASE("num_collect::ode::runge_kutta::impl::radau2a_table") {
    using num_collect::ode::runge_kutta::impl::get_radau2a_table;
    using num_collect::ode::runge_kutta::impl::radau2a_table;

    SECTION("get coefficients of Radau IIA method of order 3") {
        const radau2a_table<2>& table = get_radau2a_table<2>();

        const auto slope_coeffs = table.slope_coeffs<double>();
        const auto time_coeffs = table.time_coeffs<double>();

        using reference_formula =
            num_collect::ode::runge_kutta::radau2a3_formula<
                num_prob_collect::ode::exponential_problem>;
        const auto reference_slope_coeffs = reference_formula::slope_coeffs();
        const auto reference_time_coeffs = reference_formula::time_coeffs();

        CHECK_THAT(slope_coeffs, eigen_approx(reference_slope_coeffs));
        CHECK_THAT(time_coeffs, eigen_approx(reference_time_coeffs));
    }

    SECTION("get coefficients of Radau IIA method of order 5") {
        const radau2a_table<3>& table = get_radau2a_table<3>();

        const auto slope_coeffs = table.slope_coeffs<double>();
        const auto time_coeffs = table.time_coeffs<double>();

        using reference_formula =
            num_collect::ode::runge_kutta::radau2a5_formula<
                num_prob_collect::ode::exponential_problem>;
        const auto reference_slope_coeffs = reference_formula::slope_coeffs();
        const auto reference_time_coeffs = reference_formula::time_coeffs();

        CHECK_THAT(slope_coeffs, eigen_approx(reference_slope_coeffs));
        CHECK_THAT(time_coeffs, eigen_approx(reference_time_coeffs));
    }
}
