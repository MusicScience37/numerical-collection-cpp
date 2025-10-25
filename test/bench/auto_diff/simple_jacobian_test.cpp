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
 * \brief Test of automatic differentiation to calculate a simple Jacobian.
 */
#include <string>

#include <Eigen/Core>
#include <stat_bench/benchmark_macros.h>

#include "auto_diff_fixture.h"
#include "num_collect/auto_diff/backward/create_diff_variable.h"
#include "num_collect/auto_diff/backward/differentiate.h"
#include "num_collect/auto_diff/backward/variable.h"
#include "num_collect/auto_diff/forward/create_diff_variable.h"
#include "num_collect/auto_diff/forward/make_jacobian.h"
#include "num_collect/auto_diff/forward/variable.h"

STAT_BENCH_CASE_F(auto_diff_fixture, "simple_jacobian", "forward") {
    using arg_type =
        num_collect::auto_diff::forward::variable_vector_type<Eigen::Vector2d>;
    using value_type = Eigen::Matrix<typename arg_type::Scalar, 3, 1>;
    using jacobian_type = Eigen::Matrix<double, 3, 2>;
    using num_collect::auto_diff::forward::create_diff_variable_vector;
    using num_collect::auto_diff::forward::make_jacobian;

    STAT_BENCH_MEASURE() {
        const arg_type vec =
            create_diff_variable_vector(Eigen::Vector2d(1.234, 2.345));
        const auto val =
            value_type(vec(0) * 2.0, vec(0) + vec(1), vec(0) * vec(1));
        const jacobian_type coeff = make_jacobian(val);

        jacobian_type true_coeff = jacobian_type::Zero();
        true_coeff(0, 0) = 2.0;
        true_coeff(1, 0) = 1.0;
        true_coeff(1, 1) = 1.0;
        true_coeff(2, 0) = vec(1).value();
        true_coeff(2, 1) = vec(0).value();
        check_error(coeff, true_coeff);
    };
}

STAT_BENCH_CASE_F(auto_diff_fixture, "simple_jacobian", "backward") {
    using arg_type =
        num_collect::auto_diff::backward::variable_vector_type<Eigen::Vector2d>;
    using value_type = Eigen::Matrix<typename arg_type::Scalar, 3, 1>;
    using jacobian_type = Eigen::Matrix<double, 3, 2>;
    using num_collect::auto_diff::backward::create_diff_variable_vector;
    using num_collect::auto_diff::backward::differentiate;

    STAT_BENCH_MEASURE() {
        const arg_type vec =
            create_diff_variable_vector(Eigen::Vector2d(1.234, 2.345));
        const auto val =
            value_type(vec(0) * 2.0, vec(0) + vec(1), vec(0) * vec(1));
        const jacobian_type coeff = differentiate(val, vec);

        jacobian_type true_coeff = jacobian_type::Zero();
        true_coeff(0, 0) = 2.0;
        true_coeff(1, 0) = 1.0;
        true_coeff(1, 1) = 1.0;
        true_coeff(2, 0) = vec(1).value();
        true_coeff(2, 1) = vec(0).value();
        check_error(coeff, true_coeff);
    };
}
