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
 * \brief Definition of the function to get the default log sink.
 */
#include "num_collect/logging/sinks/default_log_sink.h"

#include "num_collect/logging/sinks/console_log_sink.h"

namespace num_collect::logging::sinks {

auto get_default_log_sink() -> log_sink {
    return create_colored_console_sink();
}

}  // namespace num_collect::logging::sinks
