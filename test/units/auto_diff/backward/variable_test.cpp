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
 * \brief Test of variable class.
 */
#include "num_collect/auto_diff/backward/variable.h"

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::backward::variable", "", float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::constant_tag;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("construct with a node") {
        constexpr auto value = static_cast<scalar_type>(1.234);
        const auto node =
            num_collect::auto_diff::backward::graph::create_node<scalar_type>();

        const auto var = variable_type(value, node);

        REQUIRE(var.value() == value);
        REQUIRE(var.node() == node);
    }

    SECTION("construct a constant") {
        constexpr auto value = static_cast<scalar_type>(1.234);

        const auto var = variable_type(value, constant_tag());

        REQUIRE(var.value() == value);
        REQUIRE(var.node() == nullptr);
    }

    SECTION("construct a variable") {
        constexpr auto value = static_cast<scalar_type>(1.234);

        const auto var = variable_type(value, variable_tag());

        REQUIRE(var.value() == value);
        REQUIRE(var.node() != nullptr);
    }

    SECTION("construct a constant without tags") {
        constexpr auto value = static_cast<scalar_type>(1.234);

        const auto var = variable_type(value);

        REQUIRE(var.value() == value);
        REQUIRE(var.node() == nullptr);
    }

    SECTION("construct without arguments") {
        const auto var = variable_type();

        REQUIRE(var.value() == static_cast<scalar_type>(0));
        REQUIRE(var.node() == nullptr);
    }

    SECTION("construct with a node") {
        const auto orig =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());

        const variable_type res = -orig;
        REQUIRE_THAT(res.value(), Catch::Matchers::WithinRel(-orig.value()));
        REQUIRE(res.node());
        REQUIRE(res.node()->children().size() == 1);
        REQUIRE(res.node()->children()[0].node() == orig.node());
        REQUIRE_THAT(res.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(static_cast<scalar_type>(-1)));
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::backward::operator+", "", float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::constant_tag;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("variable + variable") {
        const auto left =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const auto right =
            variable_type(static_cast<scalar_type>(2.345), variable_tag());

        const variable_type res = left + right;

        REQUIRE_THAT(res.value(),
            Catch::Matchers::WithinRel(left.value() + right.value()));
        REQUIRE(res.node());
        REQUIRE(res.node()->children().size() == 2);
        REQUIRE(res.node()->children()[0].node() == left.node());
        REQUIRE(res.node()->children()[1].node() == right.node());
        REQUIRE_THAT(res.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(static_cast<scalar_type>(1)));
        REQUIRE_THAT(res.node()->children()[1].sensitivity(),
            Catch::Matchers::WithinRel(static_cast<scalar_type>(1)));
    }

    SECTION("scalar + variable") {
        const auto left = static_cast<scalar_type>(1.234);
        const auto right =
            variable_type(static_cast<scalar_type>(2.345), variable_tag());

        const variable_type res = left + right;

        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(left + right.value()));
        REQUIRE(res.node() == right.node());
    }

    SECTION("variable + scalar") {
        const auto left =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const auto right = static_cast<scalar_type>(2.345);

        const variable_type res = left + right;

        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(left.value() + right));
        REQUIRE(res.node() == left.node());
    }

    SECTION("scalar + scalar") {
        const auto left =
            variable_type(static_cast<scalar_type>(1.234), constant_tag());
        const auto right = static_cast<scalar_type>(2.345);

        const variable_type res = left + right;

        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(left.value() + right));
        REQUIRE(res.node() == nullptr);
    }

    SECTION("self assignment") {
        const auto orig =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        auto var = orig;

        var += var;

        REQUIRE_THAT(var.value(),
            Catch::Matchers::WithinRel(orig.value() + orig.value()));
        REQUIRE(var.node());
        REQUIRE(var.node()->children().size() == 1);
        REQUIRE(var.node()->children()[0].node() == orig.node());
        REQUIRE_THAT(var.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(static_cast<scalar_type>(2)));
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::backward::operator-", "", float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::constant_tag;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("variable - variable") {
        const auto left =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const auto right =
            variable_type(static_cast<scalar_type>(2.345), variable_tag());

        const variable_type res = left - right;

        REQUIRE_THAT(res.value(),
            Catch::Matchers::WithinRel(left.value() - right.value()));
        REQUIRE(res.node());
        REQUIRE(res.node()->children().size() == 2);
        REQUIRE(res.node()->children()[0].node() == left.node());
        REQUIRE(res.node()->children()[1].node() == right.node());
        REQUIRE_THAT(res.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(static_cast<scalar_type>(1)));
        REQUIRE_THAT(res.node()->children()[1].sensitivity(),
            Catch::Matchers::WithinRel(static_cast<scalar_type>(-1)));
    }

    SECTION("scalar - variable") {
        const auto left = static_cast<scalar_type>(1.234);
        const auto right =
            variable_type(static_cast<scalar_type>(2.345), variable_tag());

        const variable_type res = left - right;

        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(left - right.value()));
        REQUIRE(res.node());
        REQUIRE(res.node()->children().size() == 1);
        REQUIRE(res.node()->children()[0].node() == right.node());
        REQUIRE_THAT(res.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(static_cast<scalar_type>(-1)));
    }

    SECTION("variable - scalar") {
        const auto left =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const auto right = static_cast<scalar_type>(2.345);

        const variable_type res = left - right;

        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(left.value() - right));
        REQUIRE(res.node() == left.node());
    }

    SECTION("scalar - scalar") {
        const auto left =
            variable_type(static_cast<scalar_type>(1.234), constant_tag());
        const auto right = static_cast<scalar_type>(2.345);

        const variable_type res = left - right;

        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(left.value() - right));
        REQUIRE(res.node() == nullptr);
    }

    SECTION("self assignment") {
        const auto orig =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        auto var = orig;

        var -= var;  // NOLINT

        REQUIRE(var.value() == static_cast<scalar_type>(0));
        REQUIRE(var.node() == nullptr);
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::backward::operator*", "", float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::constant_tag;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("variable * variable") {
        const auto left =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const auto right =
            variable_type(static_cast<scalar_type>(2.345), variable_tag());

        const variable_type res = left * right;

        REQUIRE_THAT(res.value(),
            Catch::Matchers::WithinRel(left.value() * right.value()));
        REQUIRE(res.node());
        REQUIRE(res.node()->children().size() == 2);
        REQUIRE(res.node()->children()[0].node() == left.node());
        REQUIRE(res.node()->children()[1].node() == right.node());
        REQUIRE_THAT(res.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(right.value()));
        REQUIRE_THAT(res.node()->children()[1].sensitivity(),
            Catch::Matchers::WithinRel(left.value()));
    }

    SECTION("scalar * variable") {
        const auto left = static_cast<scalar_type>(1.234);
        const auto right =
            variable_type(static_cast<scalar_type>(2.345), variable_tag());

        const variable_type res = left * right;

        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(left * right.value()));
        REQUIRE(res.node());
        REQUIRE(res.node()->children().size() == 1);
        REQUIRE(res.node()->children()[0].node() == right.node());
        REQUIRE_THAT(res.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(left));
    }

    SECTION("variable * scalar") {
        const auto left =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const auto right = static_cast<scalar_type>(2.345);

        const variable_type res = left * right;

        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(left.value() * right));
        REQUIRE(res.node());
        REQUIRE(res.node()->children().size() == 1);
        REQUIRE(res.node()->children()[0].node() == left.node());
        REQUIRE_THAT(res.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(right));
    }

    SECTION("scalar * scalar") {
        const auto left =
            variable_type(static_cast<scalar_type>(1.234), constant_tag());
        const auto right = static_cast<scalar_type>(2.345);

        const variable_type res = left * right;

        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(left.value() * right));
        REQUIRE(res.node() == nullptr);
    }

    SECTION("self assignment") {
        const auto orig =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        auto var = orig;

        var *= var;

        REQUIRE_THAT(var.value(),
            Catch::Matchers::WithinRel(orig.value() * orig.value()));
        REQUIRE(var.node());
        REQUIRE(var.node()->children().size() == 1);
        REQUIRE(var.node()->children()[0].node() == orig.node());
        REQUIRE_THAT(var.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(
                static_cast<scalar_type>(2) * orig.value()));
    }
}

// NOLINTNEXTLINE
TEMPLATE_TEST_CASE(
    "num_collect::auto_diff::backward::operator/", "", float, double) {
    using scalar_type = TestType;
    using variable_type =
        num_collect::auto_diff::backward::variable<scalar_type>;
    using num_collect::auto_diff::backward::constant_tag;
    using num_collect::auto_diff::backward::variable_tag;

    SECTION("variable / variable") {
        const auto left =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const auto right =
            variable_type(static_cast<scalar_type>(2.345), variable_tag());

        const variable_type res = left / right;

        REQUIRE_THAT(res.value(),
            Catch::Matchers::WithinRel(left.value() / right.value()));
        REQUIRE(res.node());
        REQUIRE(res.node()->children().size() == 2);
        REQUIRE(res.node()->children()[0].node() == left.node());
        REQUIRE(res.node()->children()[1].node() == right.node());
        REQUIRE_THAT(res.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(
                static_cast<scalar_type>(1) / right.value()));
        REQUIRE_THAT(res.node()->children()[1].sensitivity(),
            Catch::Matchers::WithinRel(
                -left.value() / (right.value() * right.value())));
    }

    SECTION("scalar / variable") {
        const auto left = static_cast<scalar_type>(1.234);
        const auto right =
            variable_type(static_cast<scalar_type>(2.345), variable_tag());

        const variable_type res = left / right;

        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(left / right.value()));
        REQUIRE(res.node());
        REQUIRE(res.node()->children().size() == 1);
        REQUIRE(res.node()->children()[0].node() == right.node());
        REQUIRE_THAT(res.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(
                -left / (right.value() * right.value())));
    }

    SECTION("variable / scalar") {
        const auto left =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        const auto right = static_cast<scalar_type>(2.345);

        const variable_type res = left / right;

        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(left.value() / right));
        REQUIRE(res.node());
        REQUIRE(res.node()->children().size() == 1);
        REQUIRE(res.node()->children()[0].node() == left.node());
        REQUIRE_THAT(res.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(static_cast<scalar_type>(1) / right));
    }

    SECTION("scalar / scalar") {
        const auto left =
            variable_type(static_cast<scalar_type>(1.234), constant_tag());
        const auto right = static_cast<scalar_type>(2.345);

        const variable_type res = left / right;

        REQUIRE_THAT(
            res.value(), Catch::Matchers::WithinRel(left.value() / right));
        REQUIRE(res.node() == nullptr);
    }

    SECTION("self assignment") {
        const auto orig =
            variable_type(static_cast<scalar_type>(1.234), variable_tag());
        auto var = orig;

        var /= var;  // NOLINT

        REQUIRE(var.value() == static_cast<scalar_type>(1));
        REQUIRE(var.node() == nullptr);
    }
}

TEST_CASE("Eigen::Matrix<num_collect::auto_diff::backward::variable>") {
    using variable_type = num_collect::auto_diff::backward::variable<double>;
    using vector_type = Eigen::Matrix<variable_type, 2, 1>;
    using num_collect::auto_diff::backward::variable_tag;

    const auto vec = vector_type(variable_type(1.234, variable_tag()),
        variable_type(2.345, variable_tag()));

    SECTION("prod") {
        const variable_type res = vec.prod();
        REQUIRE_THAT(res.value(),
            Catch::Matchers::WithinRel(vec(0).value() * vec(1).value()));
        REQUIRE(res.node());
        REQUIRE(res.node()->children().size() == 2);
        REQUIRE(res.node()->children()[0].node() == vec[0].node());
        REQUIRE(res.node()->children()[1].node() == vec[1].node());
        REQUIRE_THAT(res.node()->children()[0].sensitivity(),
            Catch::Matchers::WithinRel(vec[1].value()));
        REQUIRE_THAT(res.node()->children()[1].sensitivity(),
            Catch::Matchers::WithinRel(vec[0].value()));
    }
}
