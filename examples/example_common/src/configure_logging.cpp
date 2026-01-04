/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Implementation of configure_logging function.
 */
#include "configure_logging.h"

void configure_logging(num_collect::index_type iteration_output_period) {
    num_collect::logging::edit_default_log_tag_config()
        .output_log_level(num_collect::logging::log_level::iteration)
        .iteration_output_period(iteration_output_period);
}
