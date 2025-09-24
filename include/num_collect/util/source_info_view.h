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
 * \brief Definition of source_info_view class.
 */
#pragma once

#include <string_view>

#include "num_collect/base/index_type.h"
#include "num_collect/util/impl/compiler_builtins.h"

namespace num_collect::util {

/*!
 * \brief Class to hold information of source codes.
 *
 * This class is similar to `std::source_location` in C++20,
 * but different in a point that data can be specified in constructor
 * for testing.
 *
 * \note This class won't manage memory for strings.
 * So, strings must be valid while an instance of this class is used.
 */
class source_info_view {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] file_path File path.
     * \param[in] line Line number.
     * \param[in] column Column number.
     * \param[in] function_name Function name.
     */
    constexpr explicit source_info_view(
        std::string_view file_path = NUM_COLLECT_BUILTIN_FILE(),
        index_type line = NUM_COLLECT_BUILTIN_LINE(),
        index_type column = NUM_COLLECT_BUILTIN_COLUMN(),
        std::string_view function_name = NUM_COLLECT_BUILTIN_FUNCTION())
        : file_path_(file_path),
          line_(line),
          column_(column),
          function_name_(function_name) {}

    /*!
     * \brief Get the file path.
     *
     * \return File path.
     */
    [[nodiscard]] constexpr auto file_path() const -> std::string_view {
        return file_path_;
    }

    /*!
     * \brief Get the line number.
     *
     * \return Line number.
     */
    [[nodiscard]] constexpr auto line() const -> index_type { return line_; }

    /*!
     * \brief Get the column number.
     *
     * \return Column number.
     */
    [[nodiscard]] constexpr auto column() const -> index_type {
        return column_;
    }

    /*!
     * \brief Get the function name.
     *
     * \return Function name.
     */
    [[nodiscard]] constexpr auto function_name() const -> std::string_view {
        return function_name_;
    }

private:
    //! File path.
    std::string_view file_path_;

    //! Line number.
    index_type line_;

    //! Column number.
    index_type column_;

    //! Function name.
    std::string_view function_name_;
};

}  // namespace num_collect::util
