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
 * \brief Definition of exceptions.
 */
#pragma once

#include <stdexcept>

#include <fmt/format.h>

#include "num_collect/util/source_info_view.h"

namespace num_collect {
inline namespace base {

namespace impl {

/*!
 * \brief Format error.
 *
 * \param[in] message Error message.
 * \param[in] source_info Information of source codes.
 * \return Formatted string.
 */
[[nodiscard]] inline auto format_error(std::string_view message,
    util::source_info_view source_info) -> std::string {
    return fmt::format(FMT_STRING("{} ({}:{}:{})"), message,
        source_info.file_path(), source_info.line(), source_info.column());
}

}  // namespace impl

/*!
 * \brief Class of exception in this project.
 */
class num_collect_exception : public std::runtime_error {
public:
    /*!
     * \brief Construct.
     *
     * \param[in] message Error message.
     * \param[in] source_info Information of source codes.
     */
    explicit num_collect_exception(std::string_view message,
        util::source_info_view source_info = util::source_info_view())
        : std::runtime_error(impl::format_error(message, source_info)) {}
};

/*!
 * \brief Class of exception on assertion failure.
 */
class assertion_failure : public num_collect_exception {
public:
    using num_collect_exception::num_collect_exception;
};

/*!
 * \brief Class of exception on failure in algorithm.
 */
class algorithm_failure : public num_collect_exception {
public:
    using num_collect_exception::num_collect_exception;
};

/*!
 * \brief Class of exception on errors in files.
 */
class file_error : public num_collect_exception {
public:
    using num_collect_exception::num_collect_exception;
};

}  // namespace base
}  // namespace num_collect
