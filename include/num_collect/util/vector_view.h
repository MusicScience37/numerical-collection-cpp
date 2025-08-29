/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of vector_view class.
 */
#pragma once

#include <concepts>
#include <iterator>
#include <type_traits>

#include "num_collect/base/concepts/implicitly_convertible_to.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"

namespace num_collect::util {

namespace impl {

/*!
 * \brief Concept of types which can be convertible to
 * \ref num_collect::util::vector_view.
 *
 * \tparam T Type to check.
 * \tparam Value Type of values of the view.
 */
template <typename T, typename Value>
concept convertible_to_vector_view = requires(T& obj) {
    { obj.data() } -> base::concepts::implicitly_convertible_to<Value*>;
    { obj.size() } -> std::convertible_to<index_type>;
};

}  // namespace impl

/*!
 * \brief Class of views of vectors.
 *
 * \tparam T Type of values.
 *
 * \note T can be const-qualified.
 * \note This class holds a pointer to the data of a vector as in `std::span`,
 * but uses signed integers for indices for compatibility with Eigen library.
 */
template <typename T>
class vector_view {
public:
    //! Type of values.
    using value_type = T;

    //! Type of sizes.
    using size_type = index_type;

    //! Type of differences of pointers.
    using difference_type = index_type;

    //! Type of references.
    using reference = T&;

    //! Type of const references.
    using const_reference = const T&;

    //! Type of pointers.
    using pointer = T*;

    //! Type of const pointers.
    using const_pointer = const T*;

    //! Type of iterators.
    using iterator = T*;

    //! Type of const iterators.
    using const_iterator = const T*;

    //! Type of reverse iterators.
    using reverse_iterator = std::reverse_iterator<iterator>;

    //! Type of const reverse iterators.
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    /*!
     * \brief Constructor.
     *
     * This default constructor initializes the internal data to null pointer.
     */
    constexpr vector_view() noexcept : data_(nullptr), size_(0) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] data Pointer to the data.
     * \param[in] size Number of elements.
     */
    constexpr vector_view(T* data, index_type size) noexcept
        : data_(data), size_(size) {}

    /*!
     * \brief Constructor.
     *
     * \tparam InputVector Type of the input vector.
     * \param[in] vector Input vector.
     */
    template <typename InputVector>
        requires impl::convertible_to_vector_view<InputVector, T> &&
                     (!std::is_same_v<std::decay_t<InputVector>,
                         vector_view<T>>)
    // NOLINTNEXTLINE(*-explicit-*)
    constexpr vector_view(InputVector&& vector)
        : data_(vector.data()), size_(static_cast<size_type>(vector.size())) {}

    /*!
     * \brief Assign from a vector.
     *
     * \tparam InputVector Type of the input vector.
     * \param[in] vector Input vector.
     * \return This object.
     */
    template <typename InputVector>
        requires impl::convertible_to_vector_view<InputVector, T> &&
        (!std::is_same_v<std::decay_t<InputVector>, vector_view<T>>)
    // NOLINTNEXTLINE(*-explicit-*)
    constexpr auto operator=(InputVector&& vector) -> vector_view& {
        data_ = vector.data();
        size_ = static_cast<size_type>(vector.size());
        return *this;
    }

    /*!
     * \brief Copy constructor.
     */
    constexpr vector_view(const vector_view&) noexcept = default;

    /*!
     * \brief Move constructor.
     */
    constexpr vector_view(vector_view&&) noexcept = default;

    /*!
     * \brief Copy assignment operator.
     *
     * \return This object.
     */
    constexpr auto operator=(const vector_view&) noexcept
        -> vector_view& = default;

    /*!
     * \brief Move assignment operator.
     *
     * \return This object.
     */
    constexpr auto operator=(vector_view&&) noexcept -> vector_view& = default;

    /*!
     * \brief Destructor.
     */
    constexpr ~vector_view() = default;

    /*!
     * \brief Access to an element.
     *
     * \param[in] index Index.
     * \return Reference to the element.
     */
    [[nodiscard]] constexpr auto operator[](index_type index) const noexcept
        -> reference {
        return data_[index];
    }

    /*!
     * \brief Access to an element with check of boundary.
     *
     * \param[in] index Index.
     * \return Reference to the element.
     */
    [[nodiscard]] constexpr auto at(index_type index) const -> reference {
        if (index < 0 || size_ <= index) {
            throw out_of_range("Index in vector_view is out of range.");
        }
        return data_[index];
    }

    /*!
     * \brief Access to the front element.
     *
     * \return Reference.
     */
    [[nodiscard]] constexpr auto front() const noexcept -> reference {
        return data_[0];
    }

    /*!
     * \brief Access to the final element.
     *
     * \return Reference.
     */
    [[nodiscard]] constexpr auto back() const noexcept -> reference {
        return data_[size_ - 1];
    }

    /*!
     * \brief Get the iterator to the first element.
     *
     * \return Iterator.
     */
    [[nodiscard]] constexpr auto begin() const noexcept -> iterator {
        return data_;
    }

    /*!
     * \brief Get the iterator to the first element.
     *
     * \return Iterator.
     */
    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
        return data_;
    }

    /*!
     * \brief Get the past-the-end iterator.
     *
     * \return Iterator.
     */
    [[nodiscard]] constexpr auto end() const noexcept -> iterator {
        return data_ + size_;
    }

    /*!
     * \brief Get the past-the-end iterator.
     *
     * \return Iterator.
     */
    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
        return data_ + size_;
    }

    /*!
     * \brief Get the pointer to the data.
     *
     * \return Pointer to the data.
     */
    [[nodiscard]] constexpr auto data() const noexcept -> pointer {
        return data_;
    }

    /*!
     * \brief Get the number of elements.
     *
     * \return Number of elements.
     */
    [[nodiscard]] constexpr auto size() const noexcept -> size_type {
        return size_;
    }

    /*!
     * \brief Check whether this vector is empty.
     *
     * \retval true This vector is empty.
     * \retval false This vector is not empty.
     */
    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return size_ == 0;
    }

private:
    //! Pointer to the data.
    pointer data_;

    //! Number of elements.
    size_type size_;
};

}  // namespace num_collect::util
