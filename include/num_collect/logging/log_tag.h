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

#include <string>
#include <string_view>

#include "num_collect/util/hash_string.h"

namespace num_collect::logging {

/*!
 * \brief Class of tags of logs.
 */
class log_tag {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] name Name of this tag.
     */
    explicit log_tag(std::string_view name)
        : name_(name), hash_(hash_string(name)) {}

    /*!
     * \brief Get the name of this tag.
     *
     * \return Name.
     */
    [[nodiscard]] auto name() const noexcept -> const std::string& {
        return name_;
    }

    /*!
     * \brief Get the hash number of this tag.
     *
     * \return Hash number.
     */
    [[nodiscard]] auto hash() const noexcept -> std::size_t { return hash_; }

private:
    //! Name.
    std::string name_;

    //! Hash number.
    std::size_t hash_;
};

}  // namespace num_collect::logging
