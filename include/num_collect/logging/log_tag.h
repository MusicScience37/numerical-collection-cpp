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
 * \brief Definition of log_tag class.
 */
#pragma once

#include <compare>
#include <string>
#include <string_view>

namespace num_collect::logging {

/*!
 * \brief Class of tags of logs.
 *
 * \thread_safety Not thread-safe.
 */
class log_tag {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] name Name of this tag.
     */
    explicit log_tag(std::string_view name) : name_(name) {}

    /*!
     * \brief Get the name of this tag.
     *
     * \return Name.
     */
    [[nodiscard]] auto name() const noexcept -> const std::string& {
        return name_;
    }

    /*!
     * \brief Compare two tags.
     *
     * \param[in] right Right-hand-side object.
     * \return Result.
     */
    auto operator<=>(const log_tag& right) const noexcept
        -> std::strong_ordering {
        return this->name().compare(right.name()) <=> 0;
    }

    /*!
     * \brief Compare two tags.
     *
     * \param[in] right Right-hand-side object.
     * \return Result.
     */
    auto operator==(const log_tag& right) const noexcept -> bool {
        return this->name() == right.name();
    }

    /*!
     * \brief Compare two tags.
     *
     * \param[in] right Right-hand-side object.
     * \return Result.
     */
    auto operator!=(const log_tag& right) const noexcept -> bool = default;

private:
    //! Name.
    std::string name_;
};

}  // namespace num_collect::logging
