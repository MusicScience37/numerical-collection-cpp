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
 * \brief Test of automatic differentiation of quadratic function.
 */
#include <vector>

#include <stat_bench/benchmark_macros.h>

#include "auto_diff_fixture.h"
#include "num_collect/auto_diff/backward/differentiate.h"
#include "num_collect/auto_diff/backward/variable.h"
#include "num_collect/auto_diff/forward/create_diff_variable.h"
#include "num_collect/auto_diff/forward/variable.h"

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(auto_diff_fixture, "quadratic", "forward") {
    using scalar_type = double;
    using variable_type =
        num_collect::auto_diff::forward::variable<scalar_type>;
    using num_collect::auto_diff::forward::create_diff_variable;

    STAT_BENCH_MEASURE() {
        const auto var = create_diff_variable(static_cast<scalar_type>(1.234));
        const variable_type val =
            static_cast<scalar_type>(0.5) * var * var - var;
        const scalar_type coeff = val.diff();

        const scalar_type true_coeff =
            var.value() - static_cast<scalar_type>(1);
        check_error(coeff, true_coeff);
    };
}

// NOLINTNEXTLINE
STAT_BENCH_CASE_F(auto_diff_fixture, "quadratic", "backward") {
    using scalar_type = double;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::differentiate;
    using num_collect::auto_diff::backward::variable_tag;

    STAT_BENCH_MEASURE() {
        const auto var =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const variable_type val =
            static_cast<scalar_type>(0.5) * var * var - var;
        const scalar_type coeff = differentiate(val, var);

        const scalar_type true_coeff =
            var.value() - static_cast<scalar_type>(1);
        check_error(coeff, true_coeff);
    };
}

STAT_BENCH_MAIN
