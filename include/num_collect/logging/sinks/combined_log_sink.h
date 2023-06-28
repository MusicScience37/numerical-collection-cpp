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
 * \brief Definition of combined_log_sink class.
 */
#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "num_collect/impl/num_collect_export.h"
#include "num_collect/logging/sinks/log_sink_base.h"

namespace num_collect::logging::sinks {

/*!
 * \brief Create a log sink to write logs to multiple log sinks.
 *
 * \param[in] sinks Log sinks with log levels.
 * \return Log sink to write logs to all of the given log sinks.
 */
[[nodiscard]] NUM_COLLECT_EXPORT auto create_combined_log_sink(
    std::vector<std::pair<std::shared_ptr<log_sink_base>, log_level>> sinks)
    -> std::shared_ptr<log_sink_base>;

}  // namespace num_collect::logging::sinks
