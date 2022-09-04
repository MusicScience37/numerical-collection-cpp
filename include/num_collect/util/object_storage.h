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
 * \brief Definition of object_storage class.
 */
#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <new>
#include <utility>

#include "num_collect/util/cache_line.h"

#ifndef NDEBUG
#include <cassert>
#endif

namespace num_collect::util {

/*!
 * \brief Class of storage of objects.
 *
 * \warning This class is similar to `std::optional`, but this class won't check
 * the existence of objects for any operation. Use this class only when you're
 * writing a custom container by yourself and checking the lifetime of objects
 * by your classes.
 *
 * \tparam T Type of objects.
 * \tparam MinimumAlignment Minimum alignment.
 */
template <typename T, std::size_t MinimumAlignment = cache_line>
class alignas(std::max(MinimumAlignment, alignof(T))) object_storage {
public:
    /*!
     * \brief Constructor.
     */
    object_storage() noexcept = default;

    object_storage(const object_storage&) = delete;
    object_storage(object_storage&&) = delete;
    auto operator=(const object_storage&) = delete;
    auto operator=(object_storage&&) = delete;

    /*!
     * \brief Destructor.
     */
    ~object_storage() noexcept
#ifndef NDEBUG
    {
        assert(!has_object_);
    }
#else
        = default;
#endif

    /*!
     * \brief Construct an object.
     *
     * \tparam Args Types of arguments of the constructor.
     * \param[in] args Arguments of the constructor.
     *
     * \warning This won't call the destructor if an object exists.
     */
    template <typename... Args>
    void emplace(Args&&... args) {
#ifndef NDEBUG
        assert(!has_object_);
#endif
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
        ::new (storage_.data()) T(std::forward<Args>(args)...);
#ifndef NDEBUG
        has_object_ = true;
#endif
    }

    /*!
     * \brief Destruct the object.
     *
     * \warning This calls the destructor always.
     */
    void reset() noexcept {
        get_pointer()->~T();
#ifndef NDEBUG
        has_object_ = false;
#endif
    }

    /*!
     * \brief Get the reference of the value.
     *
     * \return Reference.
     *
     * \warning This won't check the existence of an object.
     */
    [[nodiscard]] auto get_ref() noexcept -> T& { return *get_pointer(); }

    /*!
     * \brief Get the reference of the value.
     *
     * \return Reference.
     *
     * \warning This won't check the existence of an object.
     */
    [[nodiscard]] auto get_ref() const noexcept -> const T& {
        return *get_pointer();
    }

    /*!
     * \brief Get the pointer of the value.
     *
     * \return Pointer.
     *
     * \warning This won't check the existence of an object.
     */
    [[nodiscard]] auto get_pointer() noexcept -> T* {
#ifndef NDEBUG
        assert(has_object_);
#endif
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return std::launder(reinterpret_cast<T*>(storage_.data()));
    }

    /*!
     * \brief Get the pointer of the value.
     *
     * \return Pointer.
     *
     * \warning This won't check the existence of an object.
     */
    [[nodiscard]] auto get_pointer() const noexcept -> const T* {
#ifndef NDEBUG
        assert(has_object_);
#endif
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        return std::launder(reinterpret_cast<const T*>(storage_.data()));
    }

private:
    //! Storage.
    std::array<char, sizeof(T)> storage_{};

#ifndef NDEBUG
    /*!
     * \brief Whether this storage has an object. (Defined only in debug build
     * for debugging.)
     */
    bool has_object_{false};
#endif
};

}  // namespace num_collect::util
