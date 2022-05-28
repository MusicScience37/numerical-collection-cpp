/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of iteration_layer_handler class.
 */
#include "num_collect/logging/impl/iteration_layer_handler.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("num_collect::logging::impl::iteration_layer_handler") {
    using num_collect::logging::impl::iteration_layer_handler;

    SECTION("create an object") {
        iteration_layer_handler handler;
        CHECK_FALSE(handler.is_upper_layer_iterative());
    }

    SECTION("handle iterations of ancestor nodes") {
        iteration_layer_handler handler1;
        iteration_layer_handler handler2;
        iteration_layer_handler handler3;

        handler1.initialize_lower_layer(handler2);
        handler2.initialize_lower_layer(handler3);
        CHECK_FALSE(handler1.is_upper_layer_iterative());
        CHECK_FALSE(handler2.is_upper_layer_iterative());
        CHECK_FALSE(handler3.is_upper_layer_iterative());

        handler1.set_iterative();
        CHECK_FALSE(handler1.is_upper_layer_iterative());
        CHECK(handler2.is_upper_layer_iterative());
        CHECK(handler3.is_upper_layer_iterative());
    }

    SECTION("handle iterations of parent node in multiple child nodes") {
        iteration_layer_handler handler1;
        iteration_layer_handler handler21;
        iteration_layer_handler handler22;

        handler1.initialize_lower_layer(handler21);
        handler1.initialize_lower_layer(handler22);
        CHECK_FALSE(handler1.is_upper_layer_iterative());
        CHECK_FALSE(handler21.is_upper_layer_iterative());
        CHECK_FALSE(handler22.is_upper_layer_iterative());

        handler1.set_iterative();
        CHECK_FALSE(handler1.is_upper_layer_iterative());
        CHECK(handler21.is_upper_layer_iterative());
        CHECK(handler22.is_upper_layer_iterative());
    }
}
