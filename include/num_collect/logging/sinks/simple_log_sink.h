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
 * \brief Declaration of functions for simple log sinks.
 */
#pragma once

#include <memory>
#include <string_view>

#include "num_collect/impl/num_collect_export.h"
#include "num_collect/logging/sinks/log_sink_base.h"

namespace num_collect::logging::sinks {

/*!
 * \brief Create a log sink to write to a single file.
 *
 * \param[in] filepath Filepath.
 * \return Log sink.
 */
[[nodiscard]] NUM_COLLECT_EXPORT auto create_single_file_sink(
    std::string_view filepath) -> std::shared_ptr<log_sink_base>;

/*!
 * \brief Create a log sink to write to console with color.
 *
 * \return Log sink.
 */
[[nodiscard]] NUM_COLLECT_EXPORT auto create_colored_console_sink()
    -> std::shared_ptr<log_sink_base>;

/*!
 * \brief Create a log sink to write to console without color.
 *
 * \return Log sink.
 */
[[nodiscard]] NUM_COLLECT_EXPORT auto create_non_colored_console_sink()
    -> std::shared_ptr<log_sink_base>;

}  // namespace num_collect::logging::sinks
