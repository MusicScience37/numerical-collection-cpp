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
 * \brief Definition of log_tag_view class.
 */
#pragma once

#include <compare>
#include <string>
#include <string_view>

#include "num_collect/logging/log_tag.h"

namespace num_collect::logging {

/*!
 * \brief Class of tags of logs without memory management.
 *
 * \thread_safety Not thread-safe.
 */
class log_tag_view {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] name Name of this tag.
     */
    constexpr explicit log_tag_view(std::string_view name) noexcept
        : name_(name) {}

    /*!
     * \brief Construct (implicit conversion).
     *
     * \param[in] tag Tag.
     */
    log_tag_view(const log_tag& tag) noexcept  // NOLINT
        : name_(tag.name()) {}

    /*!
     * \brief Convert to log_tag object.
     *
     * \return Converted tag.
     */
    explicit operator log_tag() const { return log_tag(name_); }

    /*!
     * \brief Get the name of this tag.
     *
     * \return Name.
     */
    [[nodiscard]] constexpr auto name() const noexcept -> std::string_view {
        return name_;
    }

    /*!
     * \brief Compare two tags.
     *
     * \param[in] right Right-hand-side object.
     * \return Result.
     */
    constexpr auto operator<=>(
        const log_tag_view& right) const noexcept -> std::strong_ordering {
        return this->name().compare(right.name()) <=> 0;
    }

    /*!
     * \brief Compare two tags.
     *
     * \param[in] right Right-hand-side object.
     * \return Result.
     */
    constexpr auto operator==(
        const log_tag_view& right) const noexcept -> bool {
        return this->name() == right.name();
    }

    /*!
     * \brief Compare two tags.
     *
     * \param[in] right Right-hand-side object.
     * \return Result.
     */
    constexpr auto operator!=(
        const log_tag_view& right) const noexcept -> bool = default;

private:
    //! Name.
    std::string_view name_;
};

}  // namespace num_collect::logging
