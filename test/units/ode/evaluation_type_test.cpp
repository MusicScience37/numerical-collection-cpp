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
 * \brief Test of evaluation_type class.
 */
#include "num_collect/ode/evaluation_type.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::ode::evaluation_type") {
    using num_collect::ode::evaluation_type;
    SECTION("allows function") {
        SECTION("diff_coeff") {
            SECTION("allowed and requested") {
                const auto allowed = evaluation_type{.diff_coeff = true};
                const auto request = evaluation_type{.diff_coeff = true};
                CHECK(allowed.allows(request));
            }

            SECTION("allowed and not requested") {
                const auto allowed = evaluation_type{.diff_coeff = true};
                const auto request = evaluation_type{.diff_coeff = false};
                CHECK(allowed.allows(request));
            }

            SECTION("not allowed and requested") {
                const auto allowed = evaluation_type{.diff_coeff = false};
                const auto request = evaluation_type{.diff_coeff = true};
                CHECK_FALSE(allowed.allows(request));
            }

            SECTION("not allowed and not requested") {
                const auto allowed = evaluation_type{.diff_coeff = false};
                const auto request = evaluation_type{.diff_coeff = false};
                CHECK(allowed.allows(request));
            }
        }

        SECTION("jacobian") {
            SECTION("allowed and requested") {
                const auto allowed = evaluation_type{.jacobian = true};
                const auto request = evaluation_type{.jacobian = true};
                CHECK(allowed.allows(request));
            }

            SECTION("allowed and not requested") {
                const auto allowed = evaluation_type{.jacobian = true};
                const auto request = evaluation_type{.jacobian = false};
                CHECK(allowed.allows(request));
            }

            SECTION("not allowed and requested") {
                const auto allowed = evaluation_type{.jacobian = false};
                const auto request = evaluation_type{.jacobian = true};
                CHECK_FALSE(allowed.allows(request));
            }

            SECTION("not allowed and not requested") {
                const auto allowed = evaluation_type{.jacobian = false};
                const auto request = evaluation_type{.jacobian = false};
                CHECK(allowed.allows(request));
            }
        }

        SECTION("mass") {
            SECTION("allowed and requested") {
                const auto allowed = evaluation_type{.mass = true};
                const auto request = evaluation_type{.mass = true};
                CHECK(allowed.allows(request));
            }

            SECTION("allowed and not requested") {
                const auto allowed = evaluation_type{.mass = true};
                const auto request = evaluation_type{.mass = false};
                CHECK(allowed.allows(request));
            }

            SECTION("not allowed and requested") {
                const auto allowed = evaluation_type{.mass = false};
                const auto request = evaluation_type{.mass = true};
                CHECK_FALSE(allowed.allows(request));
            }

            SECTION("not allowed and not requested") {
                const auto allowed = evaluation_type{.mass = false};
                const auto request = evaluation_type{.mass = false};
                CHECK(allowed.allows(request));
            }
        }
    }
}
