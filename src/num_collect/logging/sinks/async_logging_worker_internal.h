/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Declaration of functions to control workers to perform asynchronous
 * logging.
 */
#pragma once

#include <chrono>
#include <memory>
#include <string_view>

#include "num_collect/logging/log_level.h"
#include "num_collect/logging/sinks/log_sink_base.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::sinks {

/*!
 * \brief Write a log asynchronously.
 *
 * \param[in] sink Log sink.
 * \param[in] time Time.
 * \param[in] tag Tag.
 * \param[in] level Log level.
 * \param[in] source Information of the source code.
 * \param[in] body Log body.
 */
void async_write_log(const std::shared_ptr<log_sink_base>& sink,
    std::chrono::system_clock::time_point time, std::string_view tag,
    log_level level, util::source_info_view source, std::string_view body);

}  // namespace num_collect::logging::sinks
