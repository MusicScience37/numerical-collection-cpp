/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of fixtures for benchmarks.
 */
#pragma once

#include <stat_bench/fixture_base.h>

#include "num_collect/base/index_type.h"

class gauss_legendre_fixture : public stat_bench::FixtureBase {
public:
    gauss_legendre_fixture() {
        this->add_param<num_collect::index_type>("degree")
#ifndef NDEBUG
            ->add(5)   // NOLINT
            ->add(10)  // NOLINT
            ->add(50)  // NOLINT
#else
            ->add(3)   // NOLINT
            ->add(5)   // NOLINT
            ->add(7)   // NOLINT
            ->add(10)  // NOLINT
            ->add(20)  // NOLINT
            ->add(30)  // NOLINT
            ->add(50)  // NOLINT
#endif
            ;
    }
};

class gauss_legendre_kronrod_fixture : public stat_bench::FixtureBase {
public:
    gauss_legendre_kronrod_fixture() {
        this->add_param<num_collect::index_type>("degree")
#ifndef NDEBUG
            ->add(5)   // NOLINT
            ->add(10)  // NOLINT
#else
            ->add(2)   // NOLINT
            ->add(3)   // NOLINT
            ->add(4)   // NOLINT
            ->add(5)   // NOLINT
            ->add(6)   // NOLINT
            ->add(7)   // NOLINT
            ->add(8)   // NOLINT
            ->add(9)   // NOLINT
            ->add(10)  // NOLINT
#endif
            ;
    }
};

class de_finite_fixture : public stat_bench::FixtureBase {
public:
    de_finite_fixture() {
        this->add_param<num_collect::index_type>("points")
#ifndef NDEBUG
            ->add(5)   // NOLINT
            ->add(10)  // NOLINT
            ->add(50)  // NOLINT
#else
            ->add(3)   // NOLINT
            ->add(5)   // NOLINT
            ->add(7)   // NOLINT
            ->add(10)  // NOLINT
            ->add(20)  // NOLINT
            ->add(30)  // NOLINT
            ->add(50)  // NOLINT
#endif
            ;
    }
};
