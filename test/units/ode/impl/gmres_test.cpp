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
 * \brief Test of gmres function.
 */
#include "num_collect/ode/impl/gmres.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

#include "comparison_approvals.h"
#include "num_collect/util/format_dense_vector.h"

TEST_CASE("num_collect::ode::impl::gmres") {
    using num_collect::ode::impl::gmres;

    SECTION("solve at once") {
        const Eigen::Matrix3d coeff{
            {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 10.0}};
        const auto coeff_function = [coeff_ptr = &coeff](
                                        const auto& target, auto& result) {
            result = (*coeff_ptr) * target;
        };
        const Eigen::Vector3d true_sol{{1.0, 2.0, -3.0}};
        const Eigen::Vector3d rhs = coeff * true_sol;

        gmres<Eigen::Vector3d> solver;
        Eigen::Vector3d sol = Eigen::Vector3d::Zero();
        solver.solve(coeff_function, rhs, sol);

        comparison_approvals::verify_with_reference(sol, true_sol);
    }

    SECTION("solve iteratively") {
        const Eigen::Matrix3d coeff{
            {1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}, {7.0, 8.0, 10.0}};
        const auto coeff_function = [coeff_ptr = &coeff](
                                        const auto& target, auto& result) {
            result = (*coeff_ptr) * target;
        };
        const Eigen::Vector3d true_sol{{1.0, 2.0, -3.0}};
        const Eigen::Vector3d rhs = coeff * true_sol;

        gmres<Eigen::Vector3d> solver;
        solver.max_subspace_dim(2);
        Eigen::Vector3d sol = Eigen::Vector3d::Zero();
        solver.solve(coeff_function, rhs, sol);
        const Eigen::Vector3d sol1 = sol;
        solver.solve(coeff_function, rhs, sol);
        const Eigen::Vector3d sol2 = sol;
        solver.solve(coeff_function, rhs, sol);
        const Eigen::Vector3d sol3 = sol;
        solver.solve(coeff_function, rhs, sol);
        const Eigen::Vector3d sol4 = sol;
        solver.solve(coeff_function, rhs, sol);
        const Eigen::Vector3d sol5 = sol;
        solver.solve(coeff_function, rhs, sol);
        const Eigen::Vector3d sol6 = sol;
        solver.solve(coeff_function, rhs, sol);
        const Eigen::Vector3d sol7 = sol;
        solver.solve(coeff_function, rhs, sol);
        const Eigen::Vector3d sol8 = sol;
        solver.solve(coeff_function, rhs, sol);
        const Eigen::Vector3d sol9 = sol;
        solver.solve(coeff_function, rhs, sol);

        ApprovalTests::Approvals::verify(fmt::format(
            R"(
sol1:     {:.6e}
sol2:     {:.6e}
sol3:     {:.6e}
sol4:     {:.6e}
sol5:     {:.6e}
sol6:     {:.6e}
sol7:     {:.6e}
sol8:     {:.6e}
sol9:     {:.6e}
sol10:    {:.6e}
true_sol: {:.6e})",
            num_collect::util::format_dense_vector(sol1),
            num_collect::util::format_dense_vector(sol2),
            num_collect::util::format_dense_vector(sol3),
            num_collect::util::format_dense_vector(sol4),
            num_collect::util::format_dense_vector(sol5),
            num_collect::util::format_dense_vector(sol6),
            num_collect::util::format_dense_vector(sol7),
            num_collect::util::format_dense_vector(sol8),
            num_collect::util::format_dense_vector(sol9),
            num_collect::util::format_dense_vector(sol),
            num_collect::util::format_dense_vector(true_sol)));
    }
}
