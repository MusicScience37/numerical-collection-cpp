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
 * \brief Test of inexact_newton_decomposed_jacobian_complex_eigen_solver class.
 */
#include "num_collect/ode/runge_kutta/impl/inexact_newton_decomposed_jacobian_complex_eigen_solver.h"

#include <complex>

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/implicit_exponential_problem.h"

TEST_CASE(
    "num_collect::ode::runge_kutta::impl::inexact_newton_decomposed_jacobian_"
    "complex_eigen_solver") {
    using num_collect::ode::evaluation_type;
    using num_collect::ode::runge_kutta::impl::
        inexact_newton_decomposed_jacobian_complex_eigen_solver;

    SECTION("solve a scalar problem without mass") {
        using problem_type = num_prob_collect::ode::exponential_problem;
        using solver_type =
            inexact_newton_decomposed_jacobian_complex_eigen_solver<
                problem_type>;

        const std::complex<double> eigenvalue(1.23, 0.234);
        solver_type solver(eigenvalue);
        CHECK_THAT(solver.eigenvalue().real(),
            Catch::Matchers::WithinRel(eigenvalue.real()));
        CHECK_THAT(solver.eigenvalue().imag(),
            Catch::Matchers::WithinRel(eigenvalue.imag()));

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double variable = 1.0;
        problem.evaluate_on(time, variable,
            evaluation_type{.diff_coeff = true, .jacobian = true});

        constexpr double step_size = 1e-2;
        solver.update_jacobian(problem, step_size);

        const Eigen::Vector2d rhs{{3.45, 0.678}};
        Eigen::Vector2d actual = Eigen::Vector2d::Zero();
        REQUIRE_NOTHROW(solver.solve(rhs, actual));

        const std::complex<double> eigenvalue_inverse = 1.0 / eigenvalue;
        Eigen::Matrix2d coeff;
        coeff << static_cast<double>(1) /
                    (step_size)*eigenvalue_inverse.real() -
                problem.jacobian(),
            static_cast<double>(1) / (step_size)*eigenvalue_inverse.imag(),
            -static_cast<double>(1) / (step_size)*eigenvalue_inverse.imag(),
            static_cast<double>(1) / (step_size)*eigenvalue_inverse.real() -
            problem.jacobian();
        const Eigen::Vector2d reconstructed_rhs = coeff * actual;
        CHECK_THAT(reconstructed_rhs, eigen_approx(rhs));
    }

    SECTION("solve a scalar problem with mass") {
        using problem_type =
            num_prob_collect::ode::implicit_exponential_problem;
        using solver_type =
            inexact_newton_decomposed_jacobian_complex_eigen_solver<
                problem_type>;

        const std::complex<double> eigenvalue(1.23, 0.234);
        solver_type solver(eigenvalue);
        CHECK_THAT(solver.eigenvalue().real(),
            Catch::Matchers::WithinRel(eigenvalue.real()));
        CHECK_THAT(solver.eigenvalue().imag(),
            Catch::Matchers::WithinRel(eigenvalue.imag()));

        problem_type problem;
        constexpr double time = 0.0;
        constexpr double variable = 1.0;
        problem.evaluate_on(time, variable,
            evaluation_type{.diff_coeff = true, .jacobian = true});

        constexpr double step_size = 1e-2;
        solver.update_jacobian(problem, step_size);

        const Eigen::Vector2d rhs{{3.45, 0.678}};
        Eigen::Vector2d actual = Eigen::Vector2d::Zero();
        REQUIRE_NOTHROW(solver.solve(rhs, actual));

        const std::complex<double> eigenvalue_inverse = 1.0 / eigenvalue;
        Eigen::Matrix2d coeff;
        coeff << static_cast<double>(1) /
                    (step_size)*eigenvalue_inverse.real() * problem.mass() -
                problem.jacobian(),
            static_cast<double>(1) / (step_size)*eigenvalue_inverse.imag() *
            problem.mass(),
            -static_cast<double>(1) / (step_size)*eigenvalue_inverse.imag() *
            problem.mass(),
            static_cast<double>(1) / (step_size)*eigenvalue_inverse.real() *
                problem.mass() -
            problem.jacobian();
        const Eigen::Vector2d reconstructed_rhs = coeff * actual;
        CHECK_THAT(reconstructed_rhs, eigen_approx(rhs));
    }
}
