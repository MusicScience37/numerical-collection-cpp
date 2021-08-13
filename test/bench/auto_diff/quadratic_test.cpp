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
#include <celero/Celero.h>

#include "auto_diff_fixture.h"
#include "num_collect/auto_diff/backward/differentiate.h"
#include "num_collect/auto_diff/forward/variable.h"

// NOLINTNEXTLINE: external library
CELERO_MAIN

constexpr int samples = 30;
#ifdef NDEBUG
constexpr int iterations = 10000;
#else
constexpr int iterations = 100;
#endif

// NOLINTNEXTLINE: external library
BASELINE_F(quadratic, forward, auto_diff_fixture, samples, iterations) {
    using scalar_type = double;
    using variable_type =
        num_collect::auto_diff::forward::variable<scalar_type>;
    using num_collect::auto_diff::forward::create_diff_variable;

    const auto var = create_diff_variable(static_cast<scalar_type>(1.234));
    const variable_type val = static_cast<scalar_type>(0.5) * var * var - var;
    const scalar_type coeff = val.diff();

    const scalar_type true_coeff = var.value() - static_cast<scalar_type>(1);
    check_error(coeff, true_coeff);
}

// NOLINTNEXTLINE: external library
BENCHMARK_F(quadratic, backward, auto_diff_fixture, samples, iterations) {
    using scalar_type = double;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::differentiate;
    using num_collect::auto_diff::backward::variable_tag;

    const auto var =
        variable_type(static_cast<scalar_type>(1.234), variable_tag());
    const variable_type val = static_cast<scalar_type>(0.5) * var * var - var;
    const scalar_type coeff = differentiate(val, var);

    const scalar_type true_coeff = var.value() - static_cast<scalar_type>(1);
    check_error(coeff, true_coeff);
}
