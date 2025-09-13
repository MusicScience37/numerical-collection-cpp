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
 * \brief Declaration of the function to create asynchronous log sinks.
 */
#pragma once

#include "num_collect/impl/num_collect_export.h"
#include "num_collect/logging/sinks/log_sink.h"

namespace num_collect::logging::sinks {

/*!
 * \brief Create a log sink to write logs asynchronously.
 *
 * \param[in] sink Log sink to write logs.
 * \return Log sink to write logs asynchronously.
 */
[[nodiscard]] NUM_COLLECT_EXPORT auto create_async_log_sink(log_sink sink)
    -> log_sink;

}  // namespace num_collect::logging::sinks
