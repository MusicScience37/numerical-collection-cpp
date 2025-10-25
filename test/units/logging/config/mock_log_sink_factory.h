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
 * \brief Definition of mock_log_sink_factory class.
 */
#pragma once

#include <memory>

#include "num_collect/logging/config/log_sink_factory_base.h"
#include "num_collect/logging/sinks/log_sink.h"
#include "trompeloeil_catch2.h"

namespace num_collect_test::logging::config {

class mock_log_sink_factory final
    : public num_collect::logging::config::log_sink_factory_base {
public:
    // NOLINTNEXTLINE(modernize-use-trailing-return-type)
    MAKE_MOCK1(create,
        num_collect::logging::sinks::log_sink(
            num_collect::logging::config::log_sink_factory_table&),
        override);

    /*!
     * \brief Constructor.
     */
    mock_log_sink_factory() noexcept = default;

    /*!
     * \brief Destructor.
     */
    ~mock_log_sink_factory() noexcept override = default;

    mock_log_sink_factory(const mock_log_sink_factory&) = delete;
    mock_log_sink_factory(mock_log_sink_factory&&) = delete;
    auto operator=(const mock_log_sink_factory&)
        -> mock_log_sink_factory& = delete;
    auto operator=(mock_log_sink_factory&&) -> mock_log_sink_factory& = delete;
};

}  // namespace num_collect_test::logging::config
