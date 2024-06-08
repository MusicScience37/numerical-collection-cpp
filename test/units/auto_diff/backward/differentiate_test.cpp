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
 * \brief Test of differentiate function.
 */
#include "num_collect/auto_diff/backward/differentiate.h"

#include <ostream>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::backward::differentiate(variable, variable)", "",
    float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::differentiate;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("same variable") {
        const auto var =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());

        const scalar_type coeff = differentiate(var, var);

        REQUIRE_THAT(
            coeff, Catch::Matchers::WithinRel(static_cast<scalar_type>(1)));
    }

    SECTION("quadratic function") {
        const auto var =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const variable_type val =
            static_cast<scalar_type>(0.5) * var * var - var;

        const scalar_type coeff = differentiate(val, var);

        const scalar_type true_coeff =
            var.value() - static_cast<scalar_type>(1);
        REQUIRE_THAT(coeff, Catch::Matchers::WithinRel(true_coeff));
    }

    SECTION("unrelated variable") {
        const auto var1 =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const auto var2 =
            variable_type(static_cast<scalar_type>(2.345), variable_tag());

        const scalar_type coeff = differentiate(var1, var2);

        REQUIRE(coeff == static_cast<scalar_type>(0));
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::backward::differentiate(variable, matrix)", "",
    float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::differentiate;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("product of all elements in a vector") {
        using vector_type = Eigen::Matrix<variable_type, 2, 1>;
        using diff_type = Eigen::Matrix<scalar_type, 2, 1>;
        const auto vec = vector_type(
            variable_type(static_cast<scalar_type>(1.234), variable_tag()),
            variable_type(static_cast<scalar_type>(2.345), variable_tag()));
        const auto val = vec.prod();
        CHECK_THAT(val.value(),
            Catch::Matchers::WithinRel(vec[0].value() * vec[1].value()));

        const diff_type coeff = differentiate(val, vec);

        const auto true_coeff = diff_type(vec[1].value(), vec[0].value());
        REQUIRE_THAT(coeff, eigen_approx(true_coeff));
    }

    SECTION("sum of all elements in a matrix") {
        using matrix_type =
            Eigen::Matrix<variable_type, Eigen::Dynamic, Eigen::Dynamic>;
        using diff_type =
            Eigen::Matrix<scalar_type, Eigen::Dynamic, Eigen::Dynamic>;

        matrix_type mat;
        mat.resize(2, 2);
        mat(0, 0) = variable_type(static_cast<scalar_type>(1), variable_tag());
        mat(0, 1) = variable_type(static_cast<scalar_type>(2), variable_tag());
        mat(1, 0) = mat(0, 1);
        const auto val = mat.sum();
        CHECK_THAT(val.value(),
            Catch::Matchers::WithinRel(static_cast<scalar_type>(5)));  // NOLINT

        const diff_type coeff = differentiate(val, mat);

        diff_type true_coeff;
        true_coeff.resize(2, 2);
        true_coeff(0, 0) = static_cast<scalar_type>(1);
        true_coeff(0, 1) = static_cast<scalar_type>(2);
        true_coeff(1, 0) = static_cast<scalar_type>(2);
        true_coeff(1, 1) = static_cast<scalar_type>(0);
        REQUIRE_THAT(coeff, eigen_approx(true_coeff));
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::backward::differentiate(vector, vector)", "",
    float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::differentiate;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("Jacobian of a function") {
        using arg_type = Eigen::Matrix<variable_type, 2, 1>;
        using val_type = Eigen::Matrix<variable_type, 3, 1>;
        using diff_type = Eigen::Matrix<scalar_type, 3, 2>;
        const auto vec = arg_type(
            variable_type(static_cast<scalar_type>(1.234), variable_tag()),
            variable_type(static_cast<scalar_type>(2.345), variable_tag()));
        const auto val =
            val_type(vec[0] + vec[1], vec[0] - vec[1], vec[0] * vec[1]);

        const diff_type coeff = differentiate(val, vec);

        diff_type true_coeff;
        true_coeff(0, 0) = static_cast<scalar_type>(1);
        true_coeff(0, 1) = static_cast<scalar_type>(1);
        true_coeff(1, 0) = static_cast<scalar_type>(1);
        true_coeff(1, 1) = static_cast<scalar_type>(-1);
        true_coeff(2, 0) = vec[1].value();
        true_coeff(2, 1) = vec[0].value();
        REQUIRE_THAT(coeff, eigen_approx(true_coeff));
    }
}
