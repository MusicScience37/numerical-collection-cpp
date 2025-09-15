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
 * \brief Definition of pointer_iterator class.
 */
#pragma once

#include <compare>
#include <iterator>
#include <type_traits>

namespace num_collect::util::impl {

/*!
 * \brief Class of iterators using pointers.
 *
 * \tparam Pointer Type of pointers.
 *
 * This class is a simple wrapper of pointers to be used as iterators.
 */
template <typename Pointer>
    requires std::is_pointer_v<Pointer>
class pointer_iterator {
public:
    //! Type of pointers.
    using pointer = Pointer;

    //! Type of differences of pointers.
    using difference_type =
        typename std::iterator_traits<pointer>::difference_type;

    //! Type of values.
    using value_type = typename std::iterator_traits<pointer>::value_type;

    //! Type of references.
    using reference = typename std::iterator_traits<pointer>::reference;

    //! Category of iterators.
    using iterator_category = std::random_access_iterator_tag;

    //! Concept of iterators.
    using iterator_concept = std::contiguous_iterator_tag;

    /*!
     * \brief Constructor.
     */
    pointer_iterator() noexcept : ptr_(nullptr) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] ptr Pointer.
     */
    explicit pointer_iterator(pointer ptr) noexcept : ptr_(ptr) {}

    /*!
     * \brief Access to the element.
     *
     * \return Reference to the element.
     */
    [[nodiscard]] auto operator*() const noexcept -> reference { return *ptr_; }

    /*!
     * \brief Access to member functions.
     *
     * \return Pointer.
     */
    [[nodiscard]] auto operator->() const noexcept -> pointer { return ptr_; }

    /*!
     * \brief Access to the element with an offset.
     *
     * \param[in] n Offset.
     * \return Reference to the element.
     */
    [[nodiscard]] auto operator[](difference_type n) const noexcept
        -> reference {
        return *(ptr_ + n);
    }

    /*!
     * \brief Increment this iterator.
     *
     * \return This.
     */
    auto operator++() noexcept -> pointer_iterator& {
        ++ptr_;
        return *this;
    }

    /*!
     * \brief Increment this iterator.
     *
     * \return Previous value of this.
     */
    auto operator++(int) noexcept -> pointer_iterator {
        pointer_iterator old = *this;
        ++ptr_;
        return old;
    }

    /*!
     * \brief Decrement this iterator.
     *
     * \return This.
     */
    auto operator--() noexcept -> pointer_iterator& {
        --ptr_;
        return *this;
    }

    /*!
     * \brief Decrement this iterator.
     *
     * \return Previous value of this.
     */
    auto operator--(int) noexcept -> pointer_iterator {
        pointer_iterator old = *this;
        --ptr_;
        return old;
    }

    /*!
     * \brief Advance this iterator.
     *
     * \param[in] n Amount to advance.
     * \return This.
     */
    auto operator+=(difference_type n) noexcept -> pointer_iterator& {
        ptr_ += n;
        return *this;
    }

    /*!
     * \brief Retreat this iterator.
     *
     * \param[in] n Amount to retreat.
     * \return This.
     */
    auto operator-=(difference_type n) noexcept -> pointer_iterator& {
        ptr_ -= n;
        return *this;
    }

    /*!
     * \brief Get the difference from another iterator.
     *
     * \param[in] other Other iterator.
     * \return Difference.
     */
    [[nodiscard]] auto operator-(const pointer_iterator& other) const noexcept
        -> difference_type {
        return ptr_ - other.ptr_;
    }

    /*!
     * \brief Compare with another iterator.
     *
     * \param[in] other Other iterator.
     * \retval true This iterator is equal to the other.
     * \retval false This iterator is not equal to the other.
     */
    [[nodiscard]] auto operator==(const pointer_iterator& other) const noexcept
        -> bool {
        return ptr_ == other.ptr_;
    }

    /*!
     * \brief Compare with another iterator.
     *
     * \param[in] other Other iterator.
     * \return Comparison result.
     */
    [[nodiscard]] auto operator<=>(
        const pointer_iterator& other) const noexcept {
        return ptr_ <=> other.ptr_;
    }

private:
    //! Pointer.
    pointer ptr_;
};

/*!
 * \brief Add to an iterator.
 *
 * \tparam Pointer Type of pointers.
 * \param[in] iterator Iterator.
 * \param[in] n Amount to add.
 * \return Added iterator.
 */
template <typename Pointer>
[[nodiscard]] auto operator+(const pointer_iterator<Pointer>& iterator,
    typename pointer_iterator<Pointer>::difference_type n) noexcept
    -> pointer_iterator<Pointer> {
    pointer_iterator<Pointer> advanced = iterator;
    advanced += n;
    return advanced;
}

/*!
 * \brief Add to an iterator.
 *
 * \tparam Pointer Type of pointers.
 * \param[in] n Amount to add.
 * \param[in] iterator Iterator.
 * \return Added iterator.
 */
template <typename Pointer>
[[nodiscard]] auto operator+(
    typename pointer_iterator<Pointer>::difference_type n,
    const pointer_iterator<Pointer>& iterator) noexcept
    -> pointer_iterator<Pointer> {
    return iterator + n;
}

/*!
 * \brief Subtract from an iterator.
 *
 * \tparam Pointer Type of pointers.
 * \param[in] iterator Iterator.
 * \param[in] n Amount to subtract.
 * \return Subtracted iterator.
 */
template <typename Pointer>
[[nodiscard]] auto operator-(const pointer_iterator<Pointer>& iterator,
    typename pointer_iterator<Pointer>::difference_type n) noexcept
    -> pointer_iterator<Pointer> {
    pointer_iterator<Pointer> retreated = iterator;
    retreated -= n;
    return retreated;
}

}  // namespace num_collect::util::impl
