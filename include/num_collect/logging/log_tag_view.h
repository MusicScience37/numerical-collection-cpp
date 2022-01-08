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

#include <string_view>

#include "num_collect/util/hash_string.h"

namespace num_collect::logging {

/*!
 * \brief Class of tags of logs without memory management.
 */
class log_tag_view {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] name Name of this tag.
     */
    constexpr explicit log_tag_view(std::string_view name) noexcept
        : name_(name), hash_(hash_string(name)) {}

    /*!
     * \brief Get the name of this tag.
     *
     * \return Name.
     */
    [[nodiscard]] constexpr auto name() const noexcept -> std::string_view {
        return name_;
    }

    /*!
     * \brief Get the hash number of this tag.
     *
     * \return Hash number.
     */
    [[nodiscard]] constexpr auto hash() const noexcept -> std::size_t {
        return hash_;
    }

private:
    //! Name.
    std::string_view name_;

    //! Hash number.
    std::size_t hash_;
};

}  // namespace num_collect::logging
