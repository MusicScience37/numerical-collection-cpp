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
 * \brief Benchmark of stacks.
 */
#include <cstddef>
#include <stack>
#include <string>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/do_not_optimize.h>

#include "num_collect/util/static_stack.h"

constexpr std::size_t num_entries = 100;

STAT_BENCH_CASE("push_pop", "std::stack") {
    STAT_BENCH_MEASURE() {
        std::stack<int> stack;
        for (std::size_t i = 0; i < num_entries; ++i) {
            stack.push(0);
        }
        stat_bench::do_not_optimize(stack.top());
        for (std::size_t i = 0; i < num_entries; ++i) {
            stack.pop();
        }
    };
}

STAT_BENCH_CASE("push_pop", "static_stack") {
    STAT_BENCH_MEASURE() {
        num_collect::util::static_stack<int, num_entries> stack;
        for (std::size_t i = 0; i < num_entries; ++i) {
            stack.push(0);
        }
        stat_bench::do_not_optimize(stack.top());
        for (std::size_t i = 0; i < num_entries; ++i) {
            stack.pop();
        }
    };
}

STAT_BENCH_MAIN
