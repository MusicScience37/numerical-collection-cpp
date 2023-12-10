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

#include "num_collect/impl/num_collect_export.h"
#include "num_collect/logging/sinks/async_logging_worker_config.h"

namespace num_collect::logging::sinks {

/*!
 * \brief Initialize the worker to perform asynchronous logging.
 *
 * \param[in] config Configuration.
 *
 * \warning Initialization will be done only in the first call to this function
 * in a process. Use of an asynchronous log sink will automatically perform
 * initialization instead of this function.
 */
NUM_COLLECT_EXPORT void init_async_logging_worker(
    const async_logging_worker_config& config);

/*!
 * \brief Stop the worker to perform asynchronous logging.
 *
 * \warning Once stopped, asynchronous logs won't be processed even when
 * init_async_logging_worker is called.
 */
NUM_COLLECT_EXPORT void stop_async_logging_worker();

}  // namespace num_collect::logging::sinks
