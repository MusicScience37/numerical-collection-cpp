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
 * \brief Definition of log_and_throw function.
 */
#pragma once

#include <string_view>

#include "num_collect/logging/logger.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging {

/*!
 * \brief Write an error log and throw an exception.
 *
 * \tparam Exception Type of the exception.
 * \param[in] message Error message.
 * \param[in] source Information of the source code.
 */
template <typename Exception>
[[noreturn]] void log_and_throw(std::string_view message,
    util::source_info_view source = util::source_info_view()) {
    logger().error(source)(message);
    throw Exception(message);
}

}  // namespace num_collect::logging
