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

#include "num_collect/util/index_type.h"

#if defined(NUM_COLLECT_DOCUMENTATION)

/*!
 * \brief Whether the compiler has source_location class in C++ standard
 * library. (Value is compiler-dependent.)
 */
#define NUM_COLLECT_HAS_SOURCE_LOCATION 1

#include <source_location>

namespace num_collect::impl {

/*!
 * \brief Type of source_location.
 */
using source_location_type = std::source_location;

}  // namespace num_collect::impl

#elif __has_include(<source_location>)

// NOLINTNEXTLINE
#define NUM_COLLECT_HAS_SOURCE_LOCATION 1

#include <source_location>

namespace num_collect::impl {

/*!
 * \brief Type of source_location.
 */
using source_location_type = std::source_location;

}  // namespace num_collect::impl

#elif __has_include(<experimental/source_location>)

// NOLINTNEXTLINE
#define NUM_COLLECT_HAS_SOURCE_LOCATION 1

#include <experimental/source_location>

namespace num_collect::impl {

/*!
 * \brief Type of source_location.
 */
using source_location_type = std::experimental::source_location;

}  // namespace num_collect::impl

#else

// NOLINTNEXTLINE
#define NUM_COLLECT_HAS_SOURCE_LOCATION 0

#endif

namespace num_collect {

/*!
 * \brief Class to hold information of source codes.
 *
 * \note This class won't manage memory for strings.
 * \note This class is a wrapper of `std::source_location`.
 */
class source_info_view {
public:
#if defined(NUM_COLLECT_DOCUMENTATION) || NUM_COLLECT_HAS_SOURCE_LOCATION
    /*!
     * \brief Constructor
     *
     * \param[in] location source_location object.
     */
    explicit constexpr source_info_view(
        impl::source_location_type location =
            impl::source_location_type::current())
        : source_info_view(location.file_name(),
              static_cast<index_type>(location.line()),
              static_cast<index_type>(location.column()),
              location.function_name()) {}
#else
    /*!
     * \brief Constructor
     */
    constexpr source_info_view()
        : source_info_view("unknown", 0, 0, "unknown") {}
#endif

    /*!
     * \brief Construct.
     *
     * \param[in] file_path File path.
     * \param[in] line Line number.
     * \param[in] column Column number.
     * \param[in] function_name Function name.
     */
    constexpr source_info_view(std::string_view file_path, index_type line,
        index_type column, std::string_view function_name)
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

}  // namespace num_collect
