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
 * \brief Test of inexact_newton_update_equation_solver class.
 */
#include "num_collect/ode/runge_kutta/inexact_newton_update_equation_solver.h"

#include <limits>

#include <catch2/catch_test_macros.hpp>

#include "comparison_approvals.h"
#include "num_collect/base/constants.h"
#include "num_collect/ode/concepts/update_equation_solver.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/ode/problems/linear_first_order_dae_problem.h"
#include "num_collect/ode/problems/linear_first_order_ode_problem.h"
#include "num_collect/util/vector.h"
#include "num_prob_collect/ode/exponential_problem.h"
#include "num_prob_collect/ode/implicit_exponential_problem.h"
#include "num_prob_collect/ode/implicit_kaps_problem.h"
#include "num_prob_collect/ode/no_jacobian_implicit_kaps_problem.h"
#include "num_prob_collect/ode/no_jacobian_kaps_problem.h"
#include "num_prob_collect/ode/spring_movement_problem.h"

TEST_CASE(
    "num_collect::ode::runge_kutta::inexact_newton_update_equation_solver("
    "scalar)") {
    using problem_type = num_prob_collect::ode::exponential_problem;
    using solver_type =
        num_collect::ode::runge_kutta::inexact_newton_update_equation_solver<
            problem_type>;

    SECTION("check concept") {
        STATIC_REQUIRE(
            num_collect::ode::concepts::update_equation_solver<solver_type>);
    }

    SECTION("solve for Hammer-Hollingsworth method") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-3;
        const double init_var = 1.0;

        constexpr double a21 = 1.0 / 3.0;
        constexpr double a22 = 1.0 / 3.0;
        constexpr double b2 = a21 + a22;
        constexpr double c1 = 1.0 / 4.0;
        constexpr double c2 = 3.0 / 4.0;

        // First stage.
        problem.evaluate_on(init_time, init_var,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const double k1 = problem.diff_coeff();

        // Second stage.
        solver.update_jacobian(
            problem, init_time + b2 * step_size, step_size, init_var, a22);
        double z2{0.0};
        solver.init(step_size * a21 * k1, z2);
        solver.solve();
        problem.evaluate_on(init_time + b2 * step_size, init_var + z2,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const double k2 = (z2 - solver.solution_offset()) / (step_size * a22);

        const double variable = init_var + step_size * (c1 * k1 + c2 * k2);
        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(variable, reference);
    }

    SECTION("failure in solving an equation") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1.0;
        constexpr double init_var = 1.0;
        constexpr double slope_coeff = 1.0;

        CHECK_THROWS(solver.update_jacobian(
            problem, init_time, step_size, init_var, slope_coeff));
    }

    SECTION("call solve function before initialization") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-3;
        constexpr double init_var = 1.0;
        constexpr double slope_coeff = 1.0;
        const double solution_offset = 1.0;

        CHECK_THROWS(solver.solve());

        solver.update_jacobian(
            problem, init_time, step_size, init_var, slope_coeff);
        CHECK_THROWS(solver.solve());

        double solution{0.0};
        solver.init(solution_offset, solution);
        CHECK_NOTHROW(solver.solve());
    }

    SECTION("use mass if exists") {
        using problem_type =
            num_prob_collect::ode::implicit_exponential_problem;
        using solver_type = num_collect::ode::runge_kutta::
            inexact_newton_update_equation_solver<problem_type>;
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-4;
        constexpr double init_var = 1.0;

        constexpr double a21 = 1.0 / 3.0;
        constexpr double a22 = 1.0 / 3.0;
        constexpr double b2 = a21 + a22;
        constexpr double c1 = 1.0 / 4.0;
        constexpr double c2 = 3.0 / 4.0;

        // First stage.
        problem.evaluate_on(init_time, init_var,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const double k1 = problem.diff_coeff();

        // Second stage.
        solver.update_jacobian(
            problem, init_time + b2 * step_size, step_size, init_var, a22);
        double z2{0.0};
        solver.init(step_size * a21 * k1, z2);
        solver.solve();
        problem.evaluate_on(init_time + b2 * step_size, init_var + z2,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const double k2 = (z2 - solver.solution_offset()) / (step_size * a22);

        const double variable = init_var + step_size * (c1 * k1 + c2 * k2);
        const double reference = std::exp(step_size);
        comparison_approvals::verify_with_reference(variable, reference);
    }
}

TEST_CASE(
    "num_collect::ode::runge_kutta::inexact_newton_update_equation_solver("
    "vector)") {
    using problem_type = num_prob_collect::ode::spring_movement_problem;
    using solver_type =
        num_collect::ode::runge_kutta::inexact_newton_update_equation_solver<
            problem_type>;

    SECTION("check concept") {
        STATIC_REQUIRE(
            num_collect::ode::concepts::update_equation_solver<solver_type>);
    }

    SECTION("solve for Hammer-Hollingsworth method") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-3;
        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 0.0);

        constexpr double a21 = 1.0 / 3.0;
        constexpr double a22 = 1.0 / 3.0;
        constexpr double b2 = a21 + a22;
        constexpr double c1 = 1.0 / 4.0;
        constexpr double c2 = 3.0 / 4.0;

        // First stage.
        problem.evaluate_on(init_time, init_var,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const Eigen::Vector2d k1 = problem.diff_coeff();

        // Second stage.
        solver.update_jacobian(
            problem, init_time + b2 * step_size, step_size, init_var, a22);
        Eigen::Vector2d z2 = Eigen::Vector2d::Zero();
        solver.init(step_size * a21 * k1, z2);
        solver.solve();
        problem.evaluate_on(init_time + b2 * step_size, init_var + z2,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const Eigen::Vector2d k2 =
            (z2 - solver.solution_offset()) / (step_size * a22);

        const Eigen::Vector2d variable =
            init_var + step_size * (c1 * k1 + c2 * k2);
        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::cos(step_size), std::sin(step_size));
        comparison_approvals::verify_with_reference(variable, reference);
    }

    SECTION("failure in solving an equation") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-3;
        const Eigen::Vector2d init_var{
            {1.0, std::numeric_limits<double>::quiet_NaN()}};
        constexpr double slope_coeff = 1.0;
        const Eigen::Vector2d solution_offset{{0.0, 0.0}};

        solver.update_jacobian(
            problem, init_time, step_size, init_var, slope_coeff);
        Eigen::Vector2d solution = Eigen::Vector2d::Zero();
        solver.init(solution_offset, solution);
        CHECK_THROWS(solver.solve());
    }

    SECTION("call solve function before initialization") {
        solver_type solver;

        problem_type problem;
        constexpr double init_time = 0.0;
        constexpr double step_size = 1e-3;
        const Eigen::Vector2d init_var{{1.0, 0.0}};
        constexpr double slope_coeff = 1.0;
        const Eigen::Vector2d solution_offset{{0.0, 0.0}};

        CHECK_THROWS(solver.solve());

        solver.update_jacobian(
            problem, init_time, step_size, init_var, slope_coeff);
        CHECK_THROWS(solver.solve());

        Eigen::Vector2d solution = Eigen::Vector2d::Zero();
        solver.init(solution_offset, solution);
        CHECK_NOTHROW(solver.solve());
    }

    SECTION("use mass if exists") {
        using problem_type = num_prob_collect::ode::implicit_kaps_problem;
        using solver_type = num_collect::ode::runge_kutta::
            inexact_newton_update_equation_solver<problem_type>;

        solver_type solver;

        constexpr double epsilon = 0.1;
        problem_type problem{epsilon};
        constexpr double init_time = 0.0;
        constexpr double step_size = 0.01;
        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 1.0);

        constexpr double a21 = 1.0 / 3.0;
        constexpr double a22 = 1.0 / 3.0;
        constexpr double b2 = a21 + a22;
        constexpr double c1 = 1.0 / 4.0;
        constexpr double c2 = 3.0 / 4.0;

        // First stage.
        problem.evaluate_on(init_time, init_var,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const Eigen::Vector2d k1 = problem.diff_coeff();

        // Second stage.
        solver.update_jacobian(
            problem, init_time + b2 * step_size, step_size, init_var, a22);
        Eigen::Vector2d z2 = Eigen::Vector2d::Zero();
        solver.init(step_size * a21 * k1, z2);
        solver.solve();
        problem.evaluate_on(init_time + b2 * step_size, init_var + z2,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const Eigen::Vector2d k2 =
            (z2 - solver.solution_offset()) / (step_size * a22);

        const Eigen::Vector2d variable =
            init_var + step_size * (c1 * k1 + c2 * k2);
        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::exp(-2.0 * step_size), std::exp(-step_size));
        comparison_approvals::verify_with_reference(variable, reference);
    }

    SECTION("use sparse Jacobian") {
        using vector_type = Eigen::VectorXd;
        using matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
        using problem_type =
            num_collect::ode::problems::linear_first_order_ode_problem<
                vector_type, matrix_type>;
        using solver_type = num_collect::ode::runge_kutta::
            inexact_newton_update_equation_solver<problem_type>;

        static constexpr double diffusion_coeff = 1.0;
        const auto test_function = [](double time, double x) {
            return std::exp(-diffusion_coeff * num_collect::pi<double> *
                       num_collect::pi<double> * time) *
                std::sin(num_collect::pi<double> * x);
        };

        constexpr num_collect::index_type num_points = 9;
        constexpr double space_resolution = 1.0 / (num_points + 1);
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        constexpr double coeff =
            diffusion_coeff / (space_resolution * space_resolution);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            triplets.emplace_back(
                static_cast<int>(i), static_cast<int>(i), -2.0 * coeff);
            if (i > 0) {
                triplets.emplace_back(
                    static_cast<int>(i), static_cast<int>(i - 1), coeff);
            }
            if (i < num_points - 1) {
                triplets.emplace_back(
                    static_cast<int>(i), static_cast<int>(i + 1), coeff);
            }
        }
        matrix_type stiffness_matrix(num_points, num_points);
        stiffness_matrix.setFromTriplets(triplets.begin(), triplets.end());

        const vector_type load_vector = vector_type::Zero(num_points);
        problem_type problem{stiffness_matrix, load_vector};

        constexpr double init_time = 0.0;
        constexpr double step_size = 0.01;
        vector_type init_var(num_points);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            init_var(i) = test_function(
                init_time, static_cast<double>(i + 1) * space_resolution);
        }

        constexpr double a21 = 1.0 / 3.0;
        constexpr double a22 = 1.0 / 3.0;
        constexpr double b2 = a21 + a22;
        constexpr double c1 = 1.0 / 4.0;
        constexpr double c2 = 3.0 / 4.0;

        solver_type solver;

        // First stage.
        problem.evaluate_on(init_time, init_var,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const vector_type k1 = problem.diff_coeff();

        // Second stage.
        solver.update_jacobian(
            problem, init_time + b2 * step_size, step_size, init_var, a22);
        vector_type z2 = vector_type::Zero(num_points);
        solver.init(step_size * a21 * k1, z2);
        solver.solve();
        problem.evaluate_on(init_time + b2 * step_size, init_var + z2,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const vector_type k2 =
            (z2 - solver.solution_offset()) / (step_size * a22);

        const vector_type variable = init_var + step_size * (c1 * k1 + c2 * k2);
        vector_type reference(num_points);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            reference(i) = test_function(init_time + step_size,
                static_cast<double>(i + 1) * space_resolution);
        }
        comparison_approvals::verify_with_reference(variable, reference);
    }

    SECTION("use sparse Jacobian and mass") {
        using vector_type = Eigen::VectorXd;
        using matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;
        using problem_type =
            num_collect::ode::problems::linear_first_order_dae_problem<
                vector_type, matrix_type>;
        using solver_type = num_collect::ode::runge_kutta::
            inexact_newton_update_equation_solver<problem_type>;

        static constexpr double diffusion_coeff = 1.0;
        const auto test_function = [](double time, double x) {
            return std::exp(-diffusion_coeff * num_collect::pi<double> *
                       num_collect::pi<double> * time) *
                std::sin(num_collect::pi<double> * x);
        };

        constexpr double mass = 1.5;
        constexpr num_collect::index_type num_points = 9;
        constexpr double space_resolution = 1.0 / (num_points + 1);
        num_collect::util::vector<Eigen::Triplet<double>> stiffness_triplets;
        num_collect::util::vector<Eigen::Triplet<double>> mass_triplets;
        constexpr double coeff =
            mass * diffusion_coeff / (space_resolution * space_resolution);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            stiffness_triplets.emplace_back(
                static_cast<int>(i), static_cast<int>(i), -2.0 * coeff);
            if (i > 0) {
                stiffness_triplets.emplace_back(
                    static_cast<int>(i), static_cast<int>(i - 1), coeff);
            }
            if (i < num_points - 1) {
                stiffness_triplets.emplace_back(
                    static_cast<int>(i), static_cast<int>(i + 1), coeff);
            }
            mass_triplets.emplace_back(
                static_cast<int>(i), static_cast<int>(i), mass);
        }
        matrix_type stiffness_matrix(num_points, num_points);
        stiffness_matrix.setFromTriplets(
            stiffness_triplets.begin(), stiffness_triplets.end());
        matrix_type mass_matrix(num_points, num_points);
        mass_matrix.setFromTriplets(mass_triplets.begin(), mass_triplets.end());

        const vector_type load_vector = vector_type::Zero(num_points);
        problem_type problem{mass_matrix, stiffness_matrix, load_vector};

        constexpr double init_time = 0.0;
        constexpr double step_size = 0.01;
        vector_type init_var(num_points);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            init_var(i) = test_function(
                init_time, static_cast<double>(i + 1) * space_resolution);
        }

        constexpr double a21 = 1.0 / 3.0;
        constexpr double a22 = 1.0 / 3.0;
        constexpr double b2 = a21 + a22;
        constexpr double c1 = 1.0 / 4.0;
        constexpr double c2 = 3.0 / 4.0;

        solver_type solver;

        // First stage.
        problem.evaluate_on(init_time, init_var,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const vector_type k1 = problem.diff_coeff();

        // Second stage.
        solver.update_jacobian(
            problem, init_time + b2 * step_size, step_size, init_var, a22);
        vector_type z2 = vector_type::Zero(num_points);
        solver.init(step_size * a21 * k1, z2);
        solver.solve();
        problem.evaluate_on(init_time + b2 * step_size, init_var + z2,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const vector_type k2 =
            (z2 - solver.solution_offset()) / (step_size * a22);

        const vector_type variable = init_var + step_size * (c1 * k1 + c2 * k2);
        vector_type reference(num_points);
        for (num_collect::index_type i = 0; i < num_points; ++i) {
            reference(i) = test_function(init_time + step_size,
                static_cast<double>(i + 1) * space_resolution);
        }
        comparison_approvals::verify_with_reference(variable, reference);
    }

    SECTION("solve without Jacobian") {
        using problem_type = num_prob_collect::ode::no_jacobian_kaps_problem;
        using solver_type = num_collect::ode::runge_kutta::
            inexact_newton_update_equation_solver<problem_type>;

        solver_type solver;

        constexpr double epsilon = 0.1;
        problem_type problem{epsilon};
        constexpr double init_time = 0.0;
        constexpr double step_size = 0.01;
        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 1.0);

        constexpr double a21 = 1.0 / 3.0;
        constexpr double a22 = 1.0 / 3.0;
        constexpr double b2 = a21 + a22;
        constexpr double c1 = 1.0 / 4.0;
        constexpr double c2 = 3.0 / 4.0;

        // First stage.
        problem.evaluate_on(init_time, init_var,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const Eigen::Vector2d k1 = problem.diff_coeff();

        // Second stage.
        solver.update_jacobian(
            problem, init_time + b2 * step_size, step_size, init_var, a22);
        Eigen::Vector2d z2 = Eigen::Vector2d::Zero();
        solver.init(step_size * a21 * k1, z2);
        solver.solve();
        problem.evaluate_on(init_time + b2 * step_size, init_var + z2,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const Eigen::Vector2d k2 =
            (z2 - solver.solution_offset()) / (step_size * a22);

        const Eigen::Vector2d variable =
            init_var + step_size * (c1 * k1 + c2 * k2);
        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::exp(-2.0 * step_size), std::exp(-step_size));
        comparison_approvals::verify_with_reference(variable, reference);
    }

    SECTION("solve without Jacobian but with mass") {
        using problem_type =
            num_prob_collect::ode::no_jacobian_implicit_kaps_problem;
        using solver_type = num_collect::ode::runge_kutta::
            inexact_newton_update_equation_solver<problem_type>;

        solver_type solver;

        constexpr double epsilon = 0.1;
        problem_type problem{epsilon};
        constexpr double init_time = 0.0;
        constexpr double step_size = 0.01;
        const Eigen::Vector2d init_var = Eigen::Vector2d(1.0, 1.0);

        constexpr double a21 = 1.0 / 3.0;
        constexpr double a22 = 1.0 / 3.0;
        constexpr double b2 = a21 + a22;
        constexpr double c1 = 1.0 / 4.0;
        constexpr double c2 = 3.0 / 4.0;

        // First stage.
        problem.evaluate_on(init_time, init_var,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const Eigen::Vector2d k1 = problem.diff_coeff();

        // Second stage.
        solver.update_jacobian(
            problem, init_time + b2 * step_size, step_size, init_var, a22);
        Eigen::Vector2d z2 = Eigen::Vector2d::Zero();
        solver.init(step_size * a21 * k1, z2);
        solver.solve();
        problem.evaluate_on(init_time + b2 * step_size, init_var + z2,
            num_collect::ode::evaluation_type{.diff_coeff = true});
        const Eigen::Vector2d k2 =
            (z2 - solver.solution_offset()) / (step_size * a22);

        const Eigen::Vector2d variable =
            init_var + step_size * (c1 * k1 + c2 * k2);
        const Eigen::Vector2d reference =
            Eigen::Vector2d(std::exp(-2.0 * step_size), std::exp(-step_size));
        comparison_approvals::verify_with_reference(variable, reference);
    }
}
