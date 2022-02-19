/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of configure_logging function.
 */
#pragma once

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_config.h"
#include "num_collect/logging/log_tag_config.h"

/*!
 * \brief Configure logging for examples.
 */
inline void configure_logging(
    num_collect::index_type iteration_output_period = 1) {
    const auto config = num_collect::logging::log_tag_config()
                            .write_iterations(true)
                            .write_summary(true)
                            .iteration_output_period(iteration_output_period);
    num_collect::logging::log_config::instance().set_default_tag_config(config);
}
