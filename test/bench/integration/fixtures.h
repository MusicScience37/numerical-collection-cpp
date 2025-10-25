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
#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(5)
            ->add(10)
            ->add(50)
#else
            ->add(3)
            ->add(5)
            ->add(7)
            ->add(10)
            ->add(20)
            ->add(30)
            ->add(50)
#endif
            ;
    }
};

class gauss_legendre_kronrod_fixture : public stat_bench::FixtureBase {
public:
    gauss_legendre_kronrod_fixture() {
        this->add_param<num_collect::index_type>("degree")
#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(5)
            ->add(10)
#else
            ->add(2)
            ->add(3)
            ->add(4)
            ->add(5)
            ->add(6)
            ->add(7)
            ->add(8)
            ->add(9)
            ->add(10)
#endif
            ;
    }
};

class de_finite_fixture : public stat_bench::FixtureBase {
public:
    de_finite_fixture() {
        this->add_param<num_collect::index_type>("points")
#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(5)
            ->add(10)
            ->add(50)
#else
            ->add(3)
            ->add(5)
            ->add(7)
            ->add(10)
            ->add(15)
            ->add(20)
            ->add(30)
            ->add(50)
#endif
            ;
    }
};

class tanh_finite_fixture : public stat_bench::FixtureBase {
public:
    tanh_finite_fixture() {
        this->add_param<num_collect::index_type>("points")
#ifndef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(10)
            ->add(50)
            ->add(100)
#else
            ->add(10)
            ->add(20)
            ->add(30)
            ->add(50)
            ->add(70)
            ->add(100)
            ->add(150)
            ->add(200)
#endif
            ;
    }
};
