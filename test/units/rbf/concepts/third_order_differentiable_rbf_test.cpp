/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of third_order_differentiable_rbf concept.
 */
#include "num_collect/rbf/concepts/third_order_differentiable_rbf.h"

#include <catch2/catch_test_macros.hpp>

#include "num_collect/rbf/rbfs/differentiated.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/rbf/rbfs/wendland_csrbf.h"

TEST_CASE("num_collect::rbf::concepts::third_order_differentiable_rbf") {
    using num_collect::rbf::concepts::third_order_differentiable_rbf;
    using num_collect::rbf::rbfs::differentiated_t;

    SECTION("check RBFs") {
        STATIC_REQUIRE(third_order_differentiable_rbf<
            num_collect::rbf::rbfs::gaussian_rbf<double>>);
        STATIC_REQUIRE(third_order_differentiable_rbf<
            num_collect::rbf::rbfs::wendland_csrbf<double, 2, 2>>);
        STATIC_REQUIRE_FALSE(third_order_differentiable_rbf<
            num_collect::rbf::rbfs::wendland_csrbf<double, 2, 1>>);
        STATIC_REQUIRE_FALSE(third_order_differentiable_rbf<
            num_collect::rbf::rbfs::wendland_csrbf<double, 2, 0>>);
        STATIC_REQUIRE_FALSE(third_order_differentiable_rbf<differentiated_t<
                num_collect::rbf::rbfs::wendland_csrbf<double, 2, 0>>>);
    }
}
