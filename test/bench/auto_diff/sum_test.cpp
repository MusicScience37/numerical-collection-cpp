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
#include <stat_bench/bench/invocation_context.h>
#include <stat_bench/benchmark_macros.h>

#include "auto_diff_fixture.h"
#include "num_collect/auto_diff/backward/create_diff_variable.h"
#include "num_collect/auto_diff/backward/differentiate.h"
#include "num_collect/auto_diff/forward/create_diff_variable.h"
#include "num_collect/base/index_type.h"

class sum_fixture : public auto_diff_fixture {
public:
    sum_fixture() {
        // NOLINTNEXTLINE
        add_param<num_collect::index_type>("size")->add(2)->add(5)->add(10);
    }

    void setup(stat_bench::bench::InvocationContext& context) override {
        size_ = context.get_param<num_collect::index_type>("size");
    }

    [[nodiscard]] auto get_size() const -> num_collect::index_type {
        return size_;
    }

private:
    num_collect::index_type size_{};
};

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(sum_fixture, "sum", "forward") {
    using scalar_type = double;
    using diff_type = Eigen::VectorXd;
    using vector_type =
        num_collect::auto_diff::forward::variable_vector_type<Eigen::VectorXd>;
    using num_collect::auto_diff::forward::create_diff_variable_vector;

    STAT_BENCH_MEASURE() {
        const num_collect::index_type size = get_size();
        const vector_type vec =
            create_diff_variable_vector(Eigen::VectorXd::Ones(size));
        const auto val = vec.sum();
        const diff_type& coeff = val.diff();

        const diff_type true_coeff = diff_type::Ones(size);
        check_error(coeff, true_coeff);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(sum_fixture, "sum", "backward") {
    using scalar_type = double;
    using diff_type = Eigen::VectorXd;
    using vector_type =
        num_collect::auto_diff::backward::variable_vector_type<Eigen::VectorXd>;
    using num_collect::auto_diff::backward::create_diff_variable_vector;
    using num_collect::auto_diff::backward::differentiate;

    STAT_BENCH_MEASURE() {
        const num_collect::index_type size = get_size();
        const vector_type vec =
            create_diff_variable_vector(Eigen::VectorXd::Ones(size));
        const auto val = vec.sum();
        const diff_type coeff = differentiate(val, vec);

        const diff_type true_coeff = diff_type::Ones(size);
        check_error(coeff, true_coeff);
    };
}
