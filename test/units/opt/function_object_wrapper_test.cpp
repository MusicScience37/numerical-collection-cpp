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
 * \brief Test of function_object_wrapper class.
 */
#include "num_collect/opt/function_object_wrapper.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/opt/downhill_simplex.h"
#include "num_collect/opt/golden_section_search.h"

TEST_CASE("num_collect::opt::function_object_wrapper") {
    SECTION("uni-variate problem") {
        auto opt = num_collect::opt::golden_section_search(
            num_collect::opt::make_function_object_wrapper<double(double)>(
                [](double x) { return x * x; }));

        constexpr double left = -1.0;
        constexpr double right = 2.0;
        opt.init(left, right);
        opt.solve();
        constexpr double tol_sol = 1e-2;
        REQUIRE_THAT(
            opt.opt_variable(), Catch::Matchers::WithinAbs(0.0, tol_sol));
    }

    SECTION("multi-variate problem") {
        auto func = num_collect::opt::make_function_object_wrapper<double(
            Eigen::Vector2d)>(
            [](const Eigen::Vector2d& x) { return x.squaredNorm(); });
        auto opt = num_collect::opt::downhill_simplex<decltype(func)>(func);

        const auto init_var = Eigen::Vector2d(1.0, 1.0);
        opt.init(init_var);
        opt.solve();
        constexpr double tol_sol = 1e-2;
        REQUIRE_THAT(opt.opt_variable(),
            eigen_approx(Eigen::Vector2d(0.0, 0.0), tol_sol));
    }
}
