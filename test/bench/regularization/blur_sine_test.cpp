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

#include <Eigen/Core>
#include <cmath>
#include <random>

#include <celero/Celero.h>

#include "log_error_udm.h"
#include "num_collect/regularization/explicit_gcv.h"
#include "num_collect/regularization/explicit_l_curve.h"
#include "num_collect/regularization/tikhonov.h"

// NOLINTNEXTLINE: external library
CELERO_MAIN

class blur_sine_fixture : public celero::TestFixture {
public:
    blur_sine_fixture() = default;

    [[nodiscard]] auto getExperimentValues() const
        -> std::vector<celero::TestFixture::ExperimentValue> override {
        std::vector<celero::TestFixture::ExperimentValue> problem_space;
        problem_space.emplace_back(-100);  // NOLINT
        problem_space.emplace_back(-4);    // NOLINT
        problem_space.emplace_back(-2);    // NOLINT
        problem_space.emplace_back(0);     // NOLINT
        return problem_space;
    }

    void setUp(
        const celero::TestFixture::ExperimentValue& experiment_value) override {
        error_rate_ = std::pow(10.0,  // NOLINT
            static_cast<int>(experiment_value.Value));
        std::mt19937 engine;  // NOLINT
        std::normal_distribution<double> dist{0.0,
            std::sqrt(prob_.data().squaredNorm() /
                static_cast<double>(prob_.data().size()) * error_rate_)};
        data_with_error_ = prob_.data();
        for (num_collect::index_type i = 0; i < data_with_error_.size(); ++i) {
            data_with_error_(i) += dist(engine);
        }
    }

    void set_error(const Eigen::VectorXd& solution) {
        log_error_->addValue(
            std::log10((solution - prob_.solution()).squaredNorm() /
                prob_.solution().squaredNorm()));
    }

    [[nodiscard]] auto getUserDefinedMeasurements() const -> std::vector<
        std::shared_ptr<celero::UserDefinedMeasurement>> override {
        return {log_error_};
    }

    [[nodiscard]] auto prob() const
        -> const num_prob_collect::regularization::blur_sine& {
        return prob_;
    }

    [[nodiscard]] auto data_with_error() const -> const Eigen::VectorXd& {
        return data_with_error_;
    }

private:
    double error_rate_{};

#ifndef NDEBUG
    static constexpr num_collect::index_type data_size = 20;
    static constexpr num_collect::index_type solution_size = 30;
#else
    static constexpr num_collect::index_type data_size = 40;
    static constexpr num_collect::index_type solution_size = 60;
#endif
    num_prob_collect::regularization::blur_sine prob_{data_size, solution_size};

    Eigen::VectorXd data_with_error_{};

    std::shared_ptr<log_error_udm> log_error_{
        std::make_shared<log_error_udm>()};
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

// NOLINTNEXTLINE: external library
BASELINE_F(
    reg_blur_sine, tikhonov_l_curve, blur_sine_fixture, samples, iterations) {
    using solver_type =
        num_collect::regularization::tikhonov<coeff_type, data_type>;
    using searcher_type =
        num_collect::regularization::explicit_l_curve<solver_type>;

    solver_type solver;
    solver.compute(prob().coeff(), data_with_error());

    searcher_type searcher{solver};
    searcher.search();
    Eigen::VectorXd solution;
    searcher.solve(solution);

    set_error(solution);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(
    reg_blur_sine, tikhonov_gcv, blur_sine_fixture, samples, iterations) {
    using solver_type =
        num_collect::regularization::tikhonov<coeff_type, data_type>;
    using searcher_type =
        num_collect::regularization::explicit_gcv<solver_type>;

    solver_type solver;
    solver.compute(prob().coeff(), data_with_error());

    searcher_type searcher{solver};
    searcher.search();
    Eigen::VectorXd solution;
    searcher.solve(solution);

    set_error(solution);
}
