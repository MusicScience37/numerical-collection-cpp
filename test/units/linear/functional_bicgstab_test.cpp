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
 * \brief Test of functional_bicgstab class.
 */
#include "num_collect/linear/functional_bicgstab.h"

#include <limits>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include "comparison_approvals.h"
#include "fmt_approval_tests.h"

TEST_CASE("num_collect::linear::functional_bicgstab") {
    using num_collect::linear::functional_bicgstab;

    SECTION("solve") {
        const Eigen::Matrix3d coeff{
            {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 10.0}};
        const auto coeff_function = [coeff_ptr = &coeff](
                                        const auto& target, auto& result) {
            result = (*coeff_ptr) * target;
        };
        const Eigen::Vector3d true_sol{{1.0, 2.0, -3.0}};
        const Eigen::Vector3d rhs = coeff * true_sol;

        functional_bicgstab<Eigen::Vector3d> solver;
        Eigen::Vector3d sol = Eigen::Vector3d::Zero();
        solver.solve(coeff_function, rhs, sol);

        comparison_approvals::verify_with_reference(sol, true_sol);
    }

    SECTION("solve with preconditioner") {
        const Eigen::Matrix3d coeff{
            {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 10.0}};
        const auto coeff_function = [coeff_ptr = &coeff](
                                        const auto& target, auto& result) {
            result = (*coeff_ptr) * target;
        };
        const Eigen::Vector3d true_sol{{1.0, 2.0, -3.0}};
        const Eigen::Vector3d rhs = coeff * true_sol;

        functional_bicgstab<Eigen::Vector3d> solver;
        solver.prepare_preconditioner(coeff.diagonal());
        Eigen::Vector3d sol = Eigen::Vector3d::Zero();
        solver.solve(coeff_function, rhs, sol);

        comparison_approvals::verify_with_reference(sol, true_sol);
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

        functional_bicgstab<Eigen::Vector3d> solver;
        Eigen::Vector3d sol = true_sol;
        solver.solve(coeff_function, rhs, sol);

        CHECK(solver.iterations() == 0);
        comparison_approvals::verify_with_reference(sol, true_sol);
    }

    SECTION("set maximum number of iterations") {
        functional_bicgstab<Eigen::VectorXd> solver;

        CHECK_NOTHROW(solver.max_iterations(1));
        CHECK_THROWS(solver.max_iterations(0));
    }

    SECTION("set relative tolerance") {
        functional_bicgstab<Eigen::VectorXd> solver;

        CHECK_NOTHROW(solver.tolerance(0.5));
        CHECK_NOTHROW(solver.tolerance(1.0));
        CHECK_NOTHROW(solver.tolerance(std::numeric_limits<double>::epsilon()));
        CHECK_THROWS(solver.tolerance(1.01));
        CHECK_THROWS(
            solver.tolerance(std::numeric_limits<double>::epsilon() / 2));
    }
}
