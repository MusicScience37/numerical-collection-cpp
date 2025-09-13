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
 * \brief Declaration of reset_logging function.
 */
#pragma once

#include "num_collect/impl/num_collect_export.h"

namespace num_collect::logging {

/*!
 * \brief Reset logging.
 *
 * \note This function may be needed to write logs before the end of the
 * program.
 * \note This function resets the logging configuration to the default one.
 */
NUM_COLLECT_EXPORT void reset_logging();

}  // namespace num_collect::logging
