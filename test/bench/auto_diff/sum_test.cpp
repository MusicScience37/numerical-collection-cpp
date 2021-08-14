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
 * \brief Test of automatic differentiation of sum.
 */
#include <celero/Celero.h>

#include "auto_diff_fixture.h"
#include "num_collect/auto_diff/backward/differentiate.h"
#include "num_collect/auto_diff/forward/create_diff_variable.h"

constexpr int samples = 30;
#ifdef NDEBUG
constexpr int iterations = 10000;
#else
constexpr int iterations = 100;
#endif

class sum_fixture : public auto_diff_fixture {
public:
    sum_fixture() = default;

    [[nodiscard]] auto getExperimentValues() const
        -> std::vector<celero::TestFixture::ExperimentValue> override {
        std::vector<celero::TestFixture::ExperimentValue> problem_space;
        problem_space.emplace_back(2);   // NOLINT
        problem_space.emplace_back(5);   // NOLINT
        problem_space.emplace_back(10);  // NOLINT
        return problem_space;
    }

    void setUp(
        const celero::TestFixture::ExperimentValue& experiment_value) override {
        size_ = static_cast<num_collect::index_type>(experiment_value.Value);
    }

    [[nodiscard]] auto get_size() const -> num_collect::index_type {
        return size_;
    }

private:
    num_collect::index_type size_{};
};

// NOLINTNEXTLINE: external library
BASELINE_F(sum, forward, sum_fixture, samples, iterations) {
    using scalar_type = double;
    using diff_type = Eigen::VectorXd;
    using vector_type =
        num_collect::auto_diff::forward::variable_vector_type<Eigen::VectorXd>;
    using num_collect::auto_diff::forward::create_diff_variable_vector;

    const num_collect::index_type size = get_size();
    const vector_type vec =
        create_diff_variable_vector(Eigen::VectorXd::Ones(size));
    const auto val = vec.sum();
    const diff_type& coeff = val.diff();

    const diff_type true_coeff = diff_type::Ones(size);
    check_error(coeff, true_coeff);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(sum, backward, sum_fixture, samples, iterations) {
    using scalar_type = double;
    using diff_type = Eigen::VectorXd;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using vector_type = Eigen::Matrix<variable_type, Eigen::Dynamic, 1>;
    using num_collect::auto_diff::backward::differentiate;
    using num_collect::auto_diff::backward::variable_tag;

    vector_type vec;
    const num_collect::index_type size = get_size();
    vec.resize(size);
    for (num_collect::index_type i = 0; i < size; ++i) {
        vec(i) = variable_type(1.0, variable_tag());
    }
    const auto val = vec.sum();
    const diff_type coeff = differentiate(val, vec);

    const diff_type true_coeff = diff_type::Ones(size);
    check_error(coeff, true_coeff);
}
