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
 * \brief Test of sparse linear equations.
 */
#include <cmath>
#include <stdexcept>

#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCore>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/invocation_context.h>
#include <stat_bench/plot_options.h>

#include "num_collect/base/index_type.h"
#include "num_collect/constants/pi.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/impl/bicgstab.h"
#include "num_collect/ode/impl/gmres.h"
#include "num_collect/util/vector.h"

STAT_BENCH_MAIN

class sparse_linear_equation_test_fixture : public stat_bench::FixtureBase {
public:
    sparse_linear_equation_test_fixture() {
        this->add_param<int>("dim")
            ->add(100)
            ->add(200)
            ->add(500)
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(1000)
            ->add(2000)
            ->add(5000)
            ->add(10000)
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        size_ = context.get_param<int>("dim");
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        triplets.emplace_back(0, 0, 1.0);
        triplets.emplace_back(size_ - 1, size_ - 1, 1.0);
        for (int i = 1; i < size_ - 1; ++i) {
            triplets.emplace_back(i, i - 1, 1e-2);
            triplets.emplace_back(i, i, 1.0);
            triplets.emplace_back(i, i + 1, 1e-2);
        }
        coeff_.resize(size_, size_);
        coeff_.setFromTriplets(triplets.begin(), triplets.end());

        true_sol_.resize(size_);
        for (int i = 0; i < size_; ++i) {
            const double rate =
                static_cast<double>(i) / static_cast<double>(size_);
            true_sol_(i) = std::sin(rate * num_collect::constants::pi<double>);
        }

        rhs_ = coeff_ * true_sol_;

        sol_ = Eigen::VectorXd::Zero(size_);
    }

    void tear_down(stat_bench::InvocationContext& context) override {
        context.add_custom_output(
            "error", (coeff_ * sol_ - rhs_).norm() / rhs_.norm());
        context.add_custom_output("iterations", iterations_);
    }

protected:
    // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)

    //! Size.
    int size_{};

    //! Coefficient matrix.
    Eigen::SparseMatrix<double, Eigen::RowMajor> coeff_{};

    //! True solution.
    Eigen::VectorXd true_sol_{};

    //! Right-hand-side vector.
    Eigen::VectorXd rhs_{};

    //! Solution.
    Eigen::VectorXd sol_{};

    //! Relative tolerance.
    static constexpr double rel_tol{1e-8};

    //! Number of iterations.
    int iterations_{};

    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)
};

class gmres_fixture : public sparse_linear_equation_test_fixture {
public:
    gmres_fixture() {
        this->add_param<int>("sub_dim")
            ->add(1)
            ->add(2)
            ->add(3)
            ->add(4)
            ->add(5)
            ->add(10);
    }

    void setup(stat_bench::InvocationContext& context) override {
        sparse_linear_equation_test_fixture::setup(context);
        subspace_size_ = context.get_param<int>("sub_dim");
    }

protected:
    // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)

    //! Size of the subspace.
    int subspace_size_{};

    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)
};

STAT_BENCH_CASE_F(gmres_fixture, "sparse_linear_equation", "repeated_gmres") {
    num_collect::ode::impl::gmres<Eigen::VectorXd> solver;
    solver.max_subspace_dim(subspace_size_);
    const auto coeff_function = [coeff_ptr = &coeff_](
                                    const auto& target, auto& result) {
        result = (*coeff_ptr) * target;
    };
    const double tol = rel_tol * rhs_.norm();
    STAT_BENCH_MEASURE() {
        sol_.setZero();
        constexpr int max_iterations = 1000;
        for (int i = 1; i <= max_iterations; ++i) {
            solver.solve(coeff_function, rhs_, sol_);
            if ((coeff_ * sol_ - rhs_).norm() < tol) {
                iterations_ = i;
                return;
            }
        }
        throw std::runtime_error("Failed to converge.");
    };
}

STAT_BENCH_CASE_F(
    sparse_linear_equation_test_fixture, "sparse_linear_equation", "BiCGSTAB") {
    num_collect::ode::impl::bicgstab<Eigen::VectorXd> solver;
    const auto tol_abs_error_per_elem =
        1e+2 * rel_tol * rhs_.norm() / std::sqrt(rhs_.size());
    solver.tolerances(num_collect::ode::error_tolerances<Eigen::VectorXd>()
            .tol_rel_error(0.0)
            .tol_abs_error(tol_abs_error_per_elem));
    const auto coeff_function = [coeff_ptr = &coeff_](
                                    const auto& target, auto& result) {
        result = (*coeff_ptr) * target;
    };
    STAT_BENCH_MEASURE() {
        sol_.setZero();
        solver.solve(coeff_function, rhs_, sol_);
        iterations_ = static_cast<int>(solver.iterations());
    };
}

STAT_BENCH_CASE_F(sparse_linear_equation_test_fixture, "sparse_linear_equation",
    "BiCGSTAB_eigen") {
    Eigen::BiCGSTAB<Eigen::SparseMatrix<double>> solver;
    solver.setTolerance(rel_tol);
    STAT_BENCH_MEASURE() {
        sol_.setZero();
        solver.compute(coeff_);
        sol_ = solver.solve(rhs_);
        iterations_ = static_cast<int>(solver.iterations());
    };
}

STAT_BENCH_GROUP("sparse_linear_equation")
    .add_parameter_to_time_line_plot(
        "dim", stat_bench::PlotOptions().log_parameter(true));
