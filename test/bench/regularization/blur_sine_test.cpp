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
 * \brief Test of blurred sine function.
 */
#include "num_prob_collect/regularization/blur_sine.h"

#include <cmath>
#include <random>

#include <Eigen/Core>
#include <stat_bench/bench/invocation_context.h>
#include <stat_bench/benchmark_macros.h>

#include "num_collect/regularization/explicit_gcv.h"
#include "num_collect/regularization/explicit_l_curve.h"
#include "num_collect/regularization/full_gen_tikhonov.h"
#include "num_collect/regularization/tikhonov.h"
#include "num_prob_collect/regularization/dense_diff_matrix.h"

STAT_BENCH_MAIN

class blur_sine_fixture : public stat_bench::FixtureBase {
public:
    blur_sine_fixture() {
        add_param<double>("noise_rate")
            ->add(1e-100)  // NOLINT
            ->add(1e-4)    // NOLINT
            ->add(1e-2)    // NOLINT
            ->add(1.0)     // NOLINT
            ;
    }

    void setup(stat_bench::bench::InvocationContext& context) override {
        noise_rate_ = context.get_param<double>("noise_rate");
        std::mt19937 engine;  // NOLINT
        const double sigma = std::sqrt(prob_.data().squaredNorm() /
            static_cast<double>(prob_.data().size()) * noise_rate_);
        std::normal_distribution<double> dist{0.0, sigma};
        data_with_noise_ = prob_.data();
        for (num_collect::index_type i = 0; i < data_with_noise_.size(); ++i) {
            data_with_noise_(i) += dist(engine);
        }
    }

    void set_error(const Eigen::VectorXd& solution) {
        error_rate_ = (solution - prob_.solution()).squaredNorm() /
            prob_.solution().squaredNorm();
    }

    void set_param(double val) { reg_param_ = val; }

    void tear_down(stat_bench::bench::InvocationContext& context) override {
        context.add_custom_output("error_rate", error_rate_);
        context.add_custom_output("reg_param", reg_param_);
    }

    [[nodiscard]] auto prob() const
        -> const num_prob_collect::regularization::blur_sine& {
        return prob_;
    }

    [[nodiscard]] auto data_with_noise() const -> const Eigen::VectorXd& {
        return data_with_noise_;
    }

    [[nodiscard]] auto dense_diff_matrix() const -> const Eigen::MatrixXd& {
        return dense_diff_matrix_;
    }

private:
    double noise_rate_{};
    double error_rate_{};
    double reg_param_{};

#ifndef NDEBUG
    static constexpr num_collect::index_type solution_size = 30;
    static constexpr num_collect::index_type data_size = solution_size;
#else
    static constexpr num_collect::index_type solution_size = 60;
    static constexpr num_collect::index_type data_size = solution_size;
#endif
    num_prob_collect::regularization::blur_sine prob_{data_size, solution_size};

    Eigen::VectorXd data_with_noise_{};

    Eigen::MatrixXd dense_diff_matrix_{
        num_prob_collect::regularization::dense_diff_matrix<Eigen::MatrixXd>(
            solution_size)};
};

constexpr std::int64_t samples = 30;
#ifndef NDEBUG
constexpr std::int64_t iterations = 1;
#else
constexpr std::int64_t iterations = 10;
#endif

using coeff_type =
    typename num_prob_collect::regularization::blur_sine::coeff_type;
using data_type =
    typename num_prob_collect::regularization::blur_sine::data_type;

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(blur_sine_fixture, "reg_blur_sine", "tikhonov_l_curve") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::regularization::tikhonov<coeff_type, data_type>;
        using searcher_type =
            num_collect::regularization::explicit_l_curve<solver_type>;

        solver_type solver;
        solver.compute(prob().coeff(), data_with_noise());

        searcher_type searcher{solver};
        searcher.search();
        Eigen::VectorXd solution;
        searcher.solve(solution);

        set_error(solution);
        set_param(searcher.opt_param());
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(blur_sine_fixture, "reg_blur_sine", "tikhonov_gcv") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::regularization::tikhonov<coeff_type, data_type>;
        using searcher_type =
            num_collect::regularization::explicit_gcv<solver_type>;

        solver_type solver;
        solver.compute(prob().coeff(), data_with_noise());

        searcher_type searcher{solver};
        searcher.search();
        Eigen::VectorXd solution;
        searcher.solve(solution);

        set_error(solution);
        set_param(searcher.opt_param());
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(blur_sine_fixture, "reg_blur_sine", "full_gen_tik_l_curve") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::regularization::full_gen_tikhonov<coeff_type,
                data_type>;
        using searcher_type =
            num_collect::regularization::explicit_l_curve<solver_type>;

        solver_type solver;
        solver.compute(prob().coeff(), data_with_noise(), dense_diff_matrix());

        searcher_type searcher{solver};
        searcher.search();
        Eigen::VectorXd solution;
        searcher.solve(solution);

        set_error(solution);
        set_param(searcher.opt_param());
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(blur_sine_fixture, "reg_blur_sine", "full_gen_tik_gcv") {
    STAT_BENCH_MEASURE() {
        using solver_type =
            num_collect::regularization::full_gen_tikhonov<coeff_type,
                data_type>;
        using searcher_type =
            num_collect::regularization::explicit_gcv<solver_type>;

        solver_type solver;
        solver.compute(prob().coeff(), data_with_noise(), dense_diff_matrix());

        searcher_type searcher{solver};
        searcher.search();
        Eigen::VectorXd solution;
        searcher.solve(solution);

        set_error(solution);
        set_param(searcher.opt_param());
    };
}
