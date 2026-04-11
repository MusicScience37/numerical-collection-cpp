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
#include <Eigen/SparseCore>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/problems/linear_first_order_dae_problem.h"
#include "num_collect/ode/problems/linear_first_order_ode_problem.h"
#include "num_collect/util/vector.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/implicit_exponential_problem.h"
#include "num_prob_collect/ode/implicit_kaps_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

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

    SECTION(
        "solve a multi-variable problem without mass, with dense Jacobian") {
        using problem_type = num_prob_collect::ode::spring_movement_problem;
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
        const Eigen::Vector2d variable{{1.0, 0.0}};
        problem.evaluate_on(time, variable,
            evaluation_type{.diff_coeff = true, .jacobian = true});

        constexpr double step_size = 1e-2;
        solver.update_jacobian(problem, step_size);

        const Eigen::Vector4d rhs{{3.45, 4.56, 0.678, 0.789}};
        Eigen::Vector4d actual = Eigen::Vector4d::Zero();
        REQUIRE_NOTHROW(solver.solve(rhs, actual));

        const std::complex<double> eigenvalue_inverse = 1.0 / eigenvalue;
        Eigen::Matrix4d coeff;
        coeff << static_cast<double>(1) /
                    (step_size)*eigenvalue_inverse.real() *
                    Eigen::Matrix2d::Identity() -
                problem.jacobian(),
            static_cast<double>(1) / (step_size)*eigenvalue_inverse.imag() *
            Eigen::Matrix2d::Identity(),
            -static_cast<double>(1) / (step_size)*eigenvalue_inverse.imag() *
            Eigen::Matrix2d::Identity(),
            static_cast<double>(1) / (step_size)*eigenvalue_inverse.real() *
                Eigen::Matrix2d::Identity() -
            problem.jacobian();
        const Eigen::Vector4d reconstructed_rhs = coeff * actual;
        CHECK_THAT(reconstructed_rhs, eigen_approx(rhs));
    }

    SECTION("solve a multi-variable problem with mass, with dense Jacobian") {
        using problem_type = num_prob_collect::ode::implicit_kaps_problem;
        using solver_type =
            inexact_newton_decomposed_jacobian_complex_eigen_solver<
                problem_type>;

        const std::complex<double> eigenvalue(1.23, 0.234);
        solver_type solver(eigenvalue);
        CHECK_THAT(solver.eigenvalue().real(),
            Catch::Matchers::WithinRel(eigenvalue.real()));
        CHECK_THAT(solver.eigenvalue().imag(),
            Catch::Matchers::WithinRel(eigenvalue.imag()));

        constexpr double epsilon = 0.1;
        problem_type problem{epsilon};
        constexpr double time = 0.0;
        const Eigen::Vector2d variable{{1.0, 1.0}};
        problem.evaluate_on(time, variable,
            evaluation_type{.diff_coeff = true, .jacobian = true});

        constexpr double step_size = 1e-2;
        solver.update_jacobian(problem, step_size);

        const Eigen::Vector4d rhs{{3.45, 4.56, 0.678, 0.789}};
        Eigen::Vector4d actual = Eigen::Vector4d::Zero();
        REQUIRE_NOTHROW(solver.solve(rhs, actual));

        const std::complex<double> eigenvalue_inverse = 1.0 / eigenvalue;
        Eigen::Matrix4d coeff;
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
        const Eigen::Vector4d reconstructed_rhs = coeff * actual;
        CHECK_THAT(reconstructed_rhs, eigen_approx(rhs));
    }

    SECTION(
        "solve a multi-variable problem without mass, with sparse Jacobian") {
        using vector_type = Eigen::VectorXd;
        using matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
        using problem_type =
            num_collect::ode::problems::linear_first_order_ode_problem<
                vector_type, matrix_type>;
        using solver_type =
            inexact_newton_decomposed_jacobian_complex_eigen_solver<
                problem_type>;

        const std::complex<double> eigenvalue(1.23, 0.234);
        solver_type solver(eigenvalue);
        CHECK_THAT(solver.eigenvalue().real(),
            Catch::Matchers::WithinRel(eigenvalue.real()));
        CHECK_THAT(solver.eigenvalue().imag(),
            Catch::Matchers::WithinRel(eigenvalue.imag()));

        // Use simple matrix for testing.
        constexpr num_collect::index_type dimension = 3;
        num_collect::util::vector<Eigen::Triplet<double>> stiffness_triplets;
        stiffness_triplets.emplace_back(0, 0, 1.0);
        stiffness_triplets.emplace_back(1, 1, 2.0);
        stiffness_triplets.emplace_back(2, 2, 3.0);
        matrix_type stiffness_matrix(dimension, dimension);
        stiffness_matrix.setFromTriplets(
            stiffness_triplets.begin(), stiffness_triplets.end());
        vector_type load_vector = vector_type::Zero(dimension);
        problem_type problem{stiffness_matrix, load_vector};

        constexpr double time = 0.0;
        const vector_type variable = vector_type::Ones(dimension);
        problem.evaluate_on(time, variable,
            evaluation_type{.diff_coeff = true, .jacobian = true});

        constexpr double step_size = 1e-2;
        solver.update_jacobian(problem, step_size);

        const vector_type rhs =
            vector_type{{3.45, 4.56, 5.67, 0.789, 0.890, 0.901}};
        vector_type actual = vector_type::Zero(2 * dimension);
        REQUIRE_NOTHROW(solver.solve(rhs, actual));

        const std::complex<double> eigenvalue_inverse = 1.0 / eigenvalue;
        Eigen::MatrixXd coeff;
        coeff.resize(2 * dimension, 2 * dimension);
        coeff.topLeftCorner(dimension, dimension) = static_cast<double>(1) /
                (step_size)*eigenvalue_inverse.real() *
                Eigen::MatrixXd::Identity(dimension, dimension) -
            problem.jacobian();
        coeff.topRightCorner(dimension, dimension) = static_cast<double>(1) /
            (step_size)*eigenvalue_inverse.imag() *
            Eigen::MatrixXd::Identity(dimension, dimension);
        coeff.bottomLeftCorner(dimension, dimension) = -static_cast<double>(1) /
            (step_size)*eigenvalue_inverse.imag() *
            Eigen::MatrixXd::Identity(dimension, dimension);
        coeff.bottomRightCorner(dimension, dimension) = static_cast<double>(1) /
                (step_size)*eigenvalue_inverse.real() *
                Eigen::MatrixXd::Identity(dimension, dimension) -
            problem.jacobian();
        const Eigen::VectorXd reconstructed_rhs = coeff * actual;
        CHECK_THAT(reconstructed_rhs, eigen_approx(rhs));
    }

    SECTION("solve a multi-variable problem with mass, with sparse Jacobian") {
        using vector_type = Eigen::VectorXd;
        using matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
        using problem_type =
            num_collect::ode::problems::linear_first_order_dae_problem<
                vector_type, matrix_type>;
        using solver_type =
            inexact_newton_decomposed_jacobian_complex_eigen_solver<
                problem_type>;

        const std::complex<double> eigenvalue(1.23, 0.234);
        solver_type solver(eigenvalue);
        CHECK_THAT(solver.eigenvalue().real(),
            Catch::Matchers::WithinRel(eigenvalue.real()));
        CHECK_THAT(solver.eigenvalue().imag(),
            Catch::Matchers::WithinRel(eigenvalue.imag()));

        // Use simple matrix for testing.
        constexpr num_collect::index_type dimension = 3;
        num_collect::util::vector<Eigen::Triplet<double>> stiffness_triplets;
        stiffness_triplets.emplace_back(0, 0, 1.0);
        stiffness_triplets.emplace_back(1, 1, 2.0);
        stiffness_triplets.emplace_back(2, 2, 3.0);
        matrix_type stiffness_matrix(dimension, dimension);
        stiffness_matrix.setFromTriplets(
            stiffness_triplets.begin(), stiffness_triplets.end());
        num_collect::util::vector<Eigen::Triplet<double>> mass_triplets;
        mass_triplets.emplace_back(0, 0, 1.5);
        mass_triplets.emplace_back(1, 1, 2.5);
        mass_triplets.emplace_back(2, 2, 3.5);
        matrix_type mass_matrix(dimension, dimension);
        mass_matrix.setFromTriplets(mass_triplets.begin(), mass_triplets.end());
        vector_type load_vector = vector_type::Zero(dimension);
        problem_type problem{mass_matrix, stiffness_matrix, load_vector};

        constexpr double time = 0.0;
        const vector_type variable = vector_type::Ones(dimension);
        problem.evaluate_on(time, variable,
            evaluation_type{.diff_coeff = true, .jacobian = true});

        constexpr double step_size = 1e-2;
        solver.update_jacobian(problem, step_size);

        const vector_type rhs =
            vector_type{{3.45, 4.56, 5.67, 0.789, 0.890, 0.901}};
        vector_type actual = vector_type::Zero(2 * dimension);
        REQUIRE_NOTHROW(solver.solve(rhs, actual));

        const std::complex<double> eigenvalue_inverse = 1.0 / eigenvalue;
        Eigen::MatrixXd coeff;
        coeff.resize(2 * dimension, 2 * dimension);
        coeff.topLeftCorner(dimension, dimension) = static_cast<double>(1) /
                (step_size)*eigenvalue_inverse.real() * problem.mass() -
            problem.jacobian();
        coeff.topRightCorner(dimension, dimension) = static_cast<double>(1) /
            (step_size)*eigenvalue_inverse.imag() * problem.mass();
        coeff.bottomLeftCorner(dimension, dimension) = -static_cast<double>(1) /
            (step_size)*eigenvalue_inverse.imag() * problem.mass();
        coeff.bottomRightCorner(dimension, dimension) = static_cast<double>(1) /
                (step_size)*eigenvalue_inverse.real() * problem.mass() -
            problem.jacobian();
        const Eigen::VectorXd reconstructed_rhs = coeff * actual;
        CHECK_THAT(reconstructed_rhs, eigen_approx(rhs));
    }
}
