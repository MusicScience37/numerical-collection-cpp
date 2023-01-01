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
 * \brief Definition of log_tag_element class.
 */
#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <utility>

#include "num_collect/util/hash_string.h"

namespace num_collect::logging::impl {

/*!
 * \brief Class of elements in log tags.
 *
 * \thread_safety Not thread-safe.
 */
class log_tag_element {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] name Name.
     */
    explicit log_tag_element(std::string name)
        : name_(std::move(name)), hash_(util::hash_string(name_)) {}

    /*!
     * \brief Get the name.
     *
     * \return Name.
     */
    [[nodiscard]] auto name() const noexcept -> const std::string& {
        return name_;
    }

    /*!
     * \brief Get the hash number.
     *
     * \return Hash number.
     */
    [[nodiscard]] auto hash() const noexcept -> std::size_t { return hash_; }

    /*!
     * \brief Compare with another object.
     *
     * \param[in] right Right-hand-side object.
     * \retval true Two objects are equal.
     * \retval false Two objects are not equal.
     */
    [[nodiscard]] auto operator==(const log_tag_element& right) const noexcept
        -> bool {
        return (hash_ == right.hash_) && (name_ == right.name_);
    }

    /*!
     * \brief Compare with another object.
     *
     * \param[in] right Right-hand-side object.
     * \retval true Two objects are not equal.
     * \retval false Two objects are equal.
     */
    [[nodiscard]] auto operator!=(const log_tag_element& right) const noexcept
        -> bool {
        return !operator==(right);
    }

private:
    //! Name.
    std::string name_;

    //! Hash number.
    std::size_t hash_;
};

}  // namespace num_collect::logging::impl

namespace std {

/*!
 * \brief Specialization of std::hash for
 * num_collect::logging::impl::log_tag_element.
 */
template <>
struct hash<num_collect::logging::impl::log_tag_element> {
public:
    //! Type of keys.
    using argument_type = num_collect::logging::impl::log_tag_element;

    //! Type of hash numbers.
    using result_type = std::size_t;

    /*!
     * \brief Get the hash number.
     *
     * \param[in] key Key.
     * \return Hash number.
     */
    [[nodiscard]] auto operator()(const argument_type& key) const noexcept
        -> result_type {
        return key.hash();
    }
};

}  // namespace std
