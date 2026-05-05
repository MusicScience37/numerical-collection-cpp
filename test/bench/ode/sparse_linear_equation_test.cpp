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
#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <Eigen/IterativeLinearSolvers>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <stat_bench/benchmark_macros.h>
#include <stat_bench/invocation_context.h>
#include <stat_bench/plot_options.h>

#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/ode/error_tolerances.h"
#include "num_collect/ode/impl/bicgstab.h"
#include "num_collect/ode/impl/gmres.h"
#include "num_collect/rbf/generate_halton_nodes.h"
#include "num_collect/rbf/operators/laplacian_operator.h"
#include "num_collect/rbf/rbf_fd_polynomial_assembler.h"
#include "num_collect/util/vector.h"

STAT_BENCH_MAIN

using sparse_matrix_type = Eigen::SparseMatrix<double, Eigen::RowMajor>;

class sparse_linear_equation_test_fixture : public stat_bench::FixtureBase {
public:
    sparse_linear_equation_test_fixture() {
        this->add_param<int>("dim")
            ->add(100)
            ->add(200)
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(500)
            ->add(1000)
            ->add(2000)
        // ->add(5000)
        // ->add(10000)
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        size_ = context.get_param<int>("dim");

        create_coeff_matrix();

        true_sol_.resize(size_);
        for (int i = 0; i < size_; ++i) {
            const double position = nodes_[i];
            true_sol_(i) = std::sin(position * num_collect::pi<double>);
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

    //! Nodes.
    num_collect::util::vector<double> nodes_{};

    //! Coefficient matrix.
    sparse_matrix_type coeff_{};

    //! True solution.
    Eigen::VectorXd true_sol_{};

    //! Right-hand-side vector.
    Eigen::VectorXd rhs_{};

    //! Solution.
    Eigen::VectorXd sol_{};

    //! Relative tolerance.
    static constexpr double rel_tol{1e-10};

    //! Number of iterations.
    int iterations_{};

    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

private:
    void create_coeff_matrix() {
        using position_type = double;

        const num_collect::index_type num_interior_nodes = size_;
        nodes_ = num_collect::rbf::generate_1d_halton_nodes<position_type>(
            num_interior_nodes);
        nodes_.push_back(0.0);
        nodes_.push_back(1.0);
        const auto interior_nodes =
            num_collect::util::vector_view<const position_type>(nodes_).first(
                num_interior_nodes);

        using operator_type =
            num_collect::rbf::operators::laplacian_operator<position_type>;
        using assembler_type =
            num_collect::rbf::phs_rbf_fd_polynomial_assembler<position_type>;
        constexpr int polynomial_degree = 4;
        assembler_type assembler(polynomial_degree);
        assembler.num_neighbors(20);
        num_collect::util::vector<Eigen::Triplet<double>> triplets;
        const num_collect::util::nearest_neighbor_searcher<position_type>
            column_variables_nearest_neighbor_searcher(nodes_);
        constexpr num_collect::index_type row_offset = 0;
        constexpr num_collect::index_type column_offset = 0;
        assembler.compute_rows(
            [](const position_type& position) {
                return 0.1 * operator_type(position);
            },
            interior_nodes, nodes_, column_variables_nearest_neighbor_searcher,
            triplets, row_offset, column_offset);
        sparse_matrix_type whole_coefficients(
            num_interior_nodes, nodes_.size());
        whole_coefficients.setFromTriplets(triplets.begin(), triplets.end());
        coeff_.resize(size_, size_);
        coeff_.setIdentity();
        coeff_ -= 0.1 * whole_coefficients.leftCols(num_interior_nodes);
    }
};

class gmres_fixture : public sparse_linear_equation_test_fixture {
public:
    gmres_fixture() {
        this->add_param<int>("sub_dim")
            ->add(10)
            ->add(20)
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(50)
            ->add(100)
            ->add(200)
            ->add(500)
#endif
            ;
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
    solver.max_subspace_dim(std::min(subspace_size_, size_));
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

STAT_BENCH_CASE_F(sparse_linear_equation_test_fixture, "sparse_linear_equation",
    "BiCGSTAB_functional") {
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
    "BiCGSTAB_identity") {
    Eigen::BiCGSTAB<sparse_matrix_type, Eigen::IdentityPreconditioner> solver;
    solver.setTolerance(rel_tol);
    STAT_BENCH_MEASURE() {
        sol_.setZero();
        solver.compute(coeff_);
        sol_ = solver.solve(rhs_);
        iterations_ = static_cast<int>(solver.iterations());
    };
}

STAT_BENCH_CASE_F(sparse_linear_equation_test_fixture, "sparse_linear_equation",
    "BiCGSTAB_diagonal") {
    Eigen::BiCGSTAB<sparse_matrix_type, Eigen::DiagonalPreconditioner<double>>
        solver;
    solver.setTolerance(rel_tol);
    STAT_BENCH_MEASURE() {
        sol_.setZero();
        solver.compute(coeff_);
        sol_ = solver.solve(rhs_);
        iterations_ = static_cast<int>(solver.iterations());
    };
}

STAT_BENCH_CASE_F(
    sparse_linear_equation_test_fixture, "sparse_linear_equation", "SparseLU") {
    Eigen::SparseLU<sparse_matrix_type> solver;
    STAT_BENCH_MEASURE() {
        solver.compute(coeff_);
        sol_ = solver.solve(rhs_);
        iterations_ = 1;
    };
}

STAT_BENCH_GROUP("sparse_linear_equation")
    .add_parameter_to_time_line_plot(
        "dim", stat_bench::PlotOptions().log_parameter(true))
    .clear_measurement_configs()
    .add_measurement_config(stat_bench::MeasurementConfig()
            .type("Processing Time")
            .iterations(1)
            .samples(10)
            .warming_up_samples(1));
