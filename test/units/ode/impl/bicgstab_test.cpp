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
 * \brief Test of bicgstab function.
 */
#include "num_collect/ode/impl/bicgstab.h"

#include <string>

#include <ApprovalTests.hpp>
#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "comparison_approvals.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/util/format_dense_vector.h"

TEST_CASE("num_collect::ode::impl::bicgstab") {
    using num_collect::ode::error_tolerances;
    using num_collect::ode::impl::bicgstab;

    SECTION("solve") {
        const Eigen::Matrix3d coeff{
            {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 10.0}};
        const auto coeff_function = [coeff_ptr = &coeff](
                                        const auto& target, auto& result) {
            result = (*coeff_ptr) * target;
        };
        const Eigen::Vector3d true_sol{{1.0, 2.0, -3.0}};
        const Eigen::Vector3d rhs = coeff * true_sol;

        bicgstab<Eigen::Vector3d> solver;
        Eigen::Vector3d sol = Eigen::Vector3d::Zero();
        solver.solve(coeff_function, rhs, sol);

        ApprovalTests::Approvals::verify(
            fmt::format("Actual:     {0:.7e}\n"
                        "Reference:  {1:.7e}\n"
                        "Iterations: {2}",
                num_collect::util::format_dense_vector(sol),
                num_collect::util::format_dense_vector(true_sol),
                solver.iterations()));
    }

    SECTION("solve from true solution") {
        const Eigen::Matrix3d coeff{
            {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 10.0}};
        const auto coeff_function = [coeff_ptr = &coeff](
                                        const auto& target, auto& result) {
            result = (*coeff_ptr) * target;
        };
        const Eigen::Vector3d true_sol{{1.0, 2.0, -3.0}};
        const Eigen::Vector3d rhs = coeff * true_sol;

        bicgstab<Eigen::Vector3d> solver;
        solver.tolerances(error_tolerances<Eigen::Vector3d>());
        Eigen::Vector3d sol = true_sol;
        solver.solve(coeff_function, rhs, sol);

        comparison_approvals::verify_with_reference(sol, true_sol);

        CHECK(solver.iterations() == 0);
    }
}
