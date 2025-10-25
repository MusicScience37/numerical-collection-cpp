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
 * \brief Benchmark of push_back function.
 */
#include <vector>

#include <stat_bench/benchmark_macros.h>
#include <stat_bench/fixture_base.h>
#include <stat_bench/plot_options.h>

#include "num_collect/base/index_type.h"
#include "num_collect/util/trivial_vector.h"

class push_back_fixture : public stat_bench::FixtureBase {
public:
    push_back_fixture() {
        this->add_param<bool>("reserved")->add(true)->add(false);
        this->add_param<num_collect::index_type>("size")
            ->add(100)
#ifdef NUM_COLLECT_ENABLE_HEAVY_BENCH
            ->add(1000)
            ->add(10000)
            ->add(100000)
#endif
            ;
    }

    void setup(stat_bench::InvocationContext& context) override {
        size_ = context.get_param<num_collect::index_type>("size");
        reserved_ = context.get_param<bool>("reserved");
    }

    [[nodiscard]] auto size() const -> num_collect::index_type { return size_; }

    [[nodiscard]] auto reserved() const -> bool { return reserved_; }

private:
    num_collect::index_type size_{};
    bool reserved_{false};
};

STAT_BENCH_CASE_F(push_back_fixture, "push_back", "std::vector") {
    STAT_BENCH_MEASURE() {
        std::vector<int> vec;
        const num_collect::index_type size = this->size();
        if (this->reserved()) {
            vec.reserve(static_cast<std::size_t>(size));
        }
        for (num_collect::index_type i = 0; i < size; ++i) {
            vec.push_back(0);
        }
        stat_bench::do_not_optimize(vec);
    };
}

STAT_BENCH_CASE_F(push_back_fixture, "push_back", "trivial_vector") {
    STAT_BENCH_MEASURE() {
        num_collect::util::trivial_vector<int> vec;
        const num_collect::index_type size = this->size();
        if (this->reserved()) {
            vec.reserve(size);
        }
        for (num_collect::index_type i = 0; i < size; ++i) {
            vec.push_back(0);
        }
        stat_bench::do_not_optimize(vec);
    };
}

STAT_BENCH_GROUP("push_back")
    .add_parameter_to_time_line_plot("size",
        stat_bench::PlotOptions()
            .subplot_column_parameter_name("reserved")
            .log_parameter(true));

STAT_BENCH_MAIN
