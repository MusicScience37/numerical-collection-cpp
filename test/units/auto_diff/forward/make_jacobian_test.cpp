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
 * \brief Test of make_jacobian function.
 */
#include "num_collect/auto_diff/forward/make_jacobian.h"

#include <ostream>
#include <string>

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include "eigen_approx.h"
#include "num_collect/auto_diff/forward/create_diff_variable.h"
#include "num_collect/auto_diff/forward/variable.h"

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::forward::make_jacobian", "", float, double) {
    using value_type = TestType;
    using diff_type = Eigen::Matrix<value_type, Eigen::Dynamic, 1>;
    using variable_vector_type =
        num_collect::auto_diff::forward::variable_vector_type<diff_type>;
    using jacobian_type =
        Eigen::Matrix<value_type, Eigen::Dynamic, Eigen::Dynamic>;

    SECTION("make Jacobian") {
        const variable_vector_type vars =
            num_collect::auto_diff::forward::create_diff_variable_vector(
                (diff_type(2) << static_cast<value_type>(1.234),
                    static_cast<value_type>(2.345))
                    .finished());
        const variable_vector_type res =
            (variable_vector_type(3) << vars(0) + vars(1), vars(0) - vars(1),
                vars(0) * vars(1))
                .finished();
        const jacobian_type coeff =
            num_collect::auto_diff::forward::make_jacobian(res);

        jacobian_type true_coeff = jacobian_type::Zero(3, 2);
        true_coeff(0, 0) = static_cast<value_type>(1);
        true_coeff(0, 1) = static_cast<value_type>(1);
        true_coeff(1, 0) = static_cast<value_type>(1);
        true_coeff(1, 1) = static_cast<value_type>(-1);
        true_coeff(2, 0) = vars[1].value();
        true_coeff(2, 1) = vars[0].value();
        REQUIRE_THAT(coeff, eigen_approx(true_coeff));
    }
}
