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
 * \brief Definition of trivial_vector class.
 */
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <new>
#include <type_traits>
#include <utility>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/util/assert.h"

namespace num_collect::util {

/*!
 * \brief Concept of types compatible with
 * \ref num_collect::util::trivial_vector.
 *
 * \tparam T Type to check.
 *
 * This requires that
 * - T is trivially copyable
 *   (requirement for use of `std::memcpy`, `std::realloc`), and
 * - T has alignment not greater than alignof(std::max_align_t)
 *   (requirement for use of `std::malloc`, `std::realloc`).
 */
template <typename T>
concept trivial_vector_compatible = std::is_trivially_copyable_v<T> &&
    (alignof(T) <= alignof(std::max_align_t));

/*!
 * \brief Class of vectors with trivially copyable elements.
 *
 * \tparam T Type of values.
 *
 * This class uses `std::realloc` for faster resizing.
 *
 * \note This class uses signed integers as indices for consistency with
 * Eigen library.
 */
template <trivial_vector_compatible T>
class trivial_vector {
public:
    //! Type of values.
    using value_type = T;

    //! Type of sizes.
    using size_type = index_type;

    //! Type of differences of pointers.
    using difference_type = std::ptrdiff_t;

    //! Type of references.
    using reference = value_type&;

    //! Type of const references.
    using const_reference = const value_type&;

    //! Type of pointers.
    using pointer = value_type*;

    //! Type of const pointers.
    using const_pointer = const value_type*;

    // TODO define iterators later.

    // Currently, functions are ordered similar to
    // https://en.cppreference.com/w/cpp/container/vector.html

    /*!
     * \brief Constructor.
     */
    trivial_vector()
        : data_(allocate(default_capacity)),
          size_(0),
          capacity_(default_capacity) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] size Size.
     * \param[in] value Values to fill the vector.
     */
    explicit trivial_vector(
        size_type size, const value_type& value = value_type())
        : trivial_vector(size, non_initialized_t()) {
        std::fill(data_, data_ + size_, value);
    }

    /*!
     * \brief Constructor.
     *
     * \tparam InputIterator Type of input iterators.
     * \param[in] first Iterator to the first element.
     * \param[in] last Iterator to the past-the-end element.
     */
    template <std::input_iterator InputIterator>
    trivial_vector(InputIterator first, InputIterator last)
        : trivial_vector(std::distance(first, last), non_initialized_t()) {
        std::copy(first, last, data_);
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] data Pointer to the data.
     * \param[in] size Size.
     *
     * \note This constructor may be faster than the constructor with
     * iterators because this function uses `std::memcpy` internally.
     */
    trivial_vector(const_pointer data, size_type size)
        : trivial_vector(size, non_initialized_t()) {
        std::memcpy(
            data_, data, static_cast<std::size_t>(size_) * sizeof(value_type));
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] values Initializer list of values.
     */
    trivial_vector(std::initializer_list<value_type> values)
        : trivial_vector(
              static_cast<index_type>(values.size()), non_initialized_t()) {
        std::copy(values.begin(), values.end(), data_);
    }

    /*!
     * \brief Copy constructor.
     *
     * \param[in] other Other vector.
     */
    trivial_vector(const trivial_vector& other)
        : trivial_vector(other.size_, non_initialized_t()) {
        std::memcpy(data_, other.data_,
            static_cast<std::size_t>(size_) * sizeof(value_type));
    }

    /*!
     * \brief Move constructor.
     *
     * \param[in,out] other Other vector.
     */
    trivial_vector(trivial_vector&& other) noexcept
        : data_(std::exchange(other.data_, nullptr)),
          size_(std::exchange(other.size_, 0)),
          capacity_(std::exchange(other.capacity_, 0)) {}

    /*!
     * \brief Copy assignment operator.
     *
     * \param[in] other Other vector.
     * \return This.
     */
    auto operator=(const trivial_vector& other) -> trivial_vector& {
        if (this != &other) {
            resize(other.size_, non_initialized_t());
            std::memcpy(data_, other.data_,
                static_cast<std::size_t>(size_) * sizeof(value_type));
        }
        return *this;
    }

    /*!
     * \brief Move assignment operator.
     *
     * \param[in,out] other Other vector.
     * \return This.
     */
    auto operator=(trivial_vector&& other) noexcept -> trivial_vector& {
        if (this != &other) {
            deallocate(data_);
            data_ = std::exchange(other.data_, nullptr);
            size_ = std::exchange(other.size_, 0);
            capacity_ = std::exchange(other.capacity_, 0);
        }
        return *this;
    }

    /*!
     * \brief Destructor.
     */
    ~trivial_vector() noexcept { deallocate(data_); }

    /*!
     * \brief Assign values from an initializer list.
     *
     * \param[in] values Initializer list of values.
     * \return This.
     */
    auto operator=(std::initializer_list<value_type> values)
        -> trivial_vector& {
        resize(static_cast<index_type>(values.size()), non_initialized_t());
        std::copy(values.begin(), values.end(), data_);
        return *this;
    }

    /*!
     * \brief Access an element with bounds checking.
     *
     * \param[in] index Index.
     * \return Reference to the element.
     * \throw num_collect::base::out_of_range If the index is out of range.
     */
    [[nodiscard]] auto at(size_type index) -> reference {
        if (index < 0 || size_ <= index) {
            throw out_of_range("Index is out of range in trivial_vector::at.");
        }
        return data_[index];
    }

    /*!
     * \brief Access an element with bounds checking.
     *
     * \param[in] index Index.
     * \return Reference to the element.
     * \throw num_collect::base::out_of_range If the index is out of range.
     */
    [[nodiscard]] auto at(size_type index) const -> const_reference {
        if (index < 0 || size_ <= index) {
            throw out_of_range("Index is out of range in trivial_vector::at.");
        }
        return data_[index];
    }

    /*!
     * \brief Access an element without bounds checking.
     *
     * \param[in] index Index.
     * \return Reference to the element.
     *
     * \note This function checks the bounds in debug mode.
     */
    [[nodiscard]] auto operator[](size_type index) noexcept -> reference {
        NUM_COLLECT_DEBUG_ASSERT(0 <= index && index < size_);
        return data_[index];
    }

    /*!
     * \brief Access an element without bounds checking.
     *
     * \param[in] index Index.
     * \return Reference to the element.
     *
     * \note This function checks the bounds in debug mode.
     */
    [[nodiscard]] auto operator[](size_type index) const noexcept
        -> const_reference {
        NUM_COLLECT_DEBUG_ASSERT(0 <= index && index < size_);
        return data_[index];
    }

    /*!
     * \brief Access the first element.
     *
     * \return Reference to the first element.
     *
     * \note This function checks that the vector is not empty in debug mode.
     * The behavior is undefined if the vector is empty in release mode.
     */
    [[nodiscard]] auto front() noexcept -> reference {
        NUM_COLLECT_DEBUG_ASSERT(size_ > 0);
        return data_[0];
    }

    /*!
     * \brief Access the first element.
     *
     * \return Reference to the first element.
     *
     * \note This function checks that the vector is not empty in debug mode.
     * The behavior is undefined if the vector is empty in release mode.
     */
    [[nodiscard]] auto front() const noexcept -> const_reference {
        NUM_COLLECT_DEBUG_ASSERT(size_ > 0);
        return data_[0];
    }

    /*!
     * \brief Access the last element.
     *
     * \return Reference to the last element.
     *
     * \note This function checks that the vector is not empty in debug mode.
     * The behavior is undefined if the vector is empty in release mode.
     */
    [[nodiscard]] auto back() noexcept -> reference {
        NUM_COLLECT_DEBUG_ASSERT(size_ > 0);
        return data_[size_ - 1];
    }

    /*!
     * \brief Access the last element.
     *
     * \return Reference to the last element.
     *
     * \note This function checks that the vector is not empty in debug mode.
     * The behavior is undefined if the vector is empty in release mode.
     */
    [[nodiscard]] auto back() const noexcept -> const_reference {
        NUM_COLLECT_DEBUG_ASSERT(size_ > 0);
        return data_[size_ - 1];
    }

    /*!
     * \brief Get the pointer to the data.
     *
     * \return Pointer to the data.
     */
    [[nodiscard]] auto data() noexcept -> pointer { return data_; }

    /*!
     * \brief Get the pointer to the data.
     *
     * \return Pointer to the data.
     */
    [[nodiscard]] auto data() const noexcept -> const_pointer { return data_; }

    // TODO define iterators later.

    /*!
     * \brief Check whether this vector is empty.
     *
     * \retval true This vector is empty.
     * \retval false This vector is not empty.
     */
    [[nodiscard]] auto empty() const noexcept -> bool { return size_ == 0; }

    /*!
     * \brief Get the size of this vector.
     *
     * \return Size.
     */
    [[nodiscard]] auto size() const noexcept -> size_type { return size_; }

    /*!
     * \brief Get the maximum size of this vector.
     *
     * \return Maximum size.
     */
    [[nodiscard]] static constexpr auto max_size() noexcept -> size_type {
        constexpr std::size_t max_allocatable_size =
            std::numeric_limits<std::size_t>::max() / sizeof(value_type);
        constexpr index_type max_index = std::numeric_limits<index_type>::max();
        if constexpr (max_allocatable_size <=
            static_cast<std::make_unsigned_t<index_type>>(max_index)) {
            return static_cast<index_type>(max_allocatable_size);
        } else {
            return max_index;
        }
    }

    /*!
     * \brief Reserve memory.
     *
     * \param[in] capacity Capacity.
     */
    void reserve(size_type capacity) {
        if (capacity < 0) {
            throw invalid_argument("Negative capacity was given to reserve.");
        }
        if (capacity > max_size()) {
            throw invalid_argument("Too large capacity was given to reserve.");
        }
        if (capacity > capacity_) {
            data_ = reallocate(data_, capacity);
            capacity_ = capacity;
        }
    }

    /*!
     * \brief Get the capacity of this vector.
     *
     * \return Capacity.
     */
    [[nodiscard]] auto capacity() const noexcept -> size_type {
        return capacity_;
    }

    /*!
     * \brief Clear all elements in this vector.
     */
    void clear() noexcept { size_ = 0; }

    /*!
     * \brief Change the size of this vector.
     *
     * \param[in] new_size New size.
     * \param[in] value Value to fill new elements.
     */
    void resize(size_type new_size, const value_type& value = value_type()) {
        if (new_size < 0) {
            throw invalid_argument("Negative size was given to resize.");
        }
        if (new_size > max_size()) {
            throw invalid_argument("Too large size was given to resize.");
        }
        if (new_size > capacity_) {
            data_ = reallocate(data_, new_size);
            capacity_ = new_size;
        }
        if (new_size > size_) {
            std::fill(data_ + size_, data_ + new_size, value);
        }
        size_ = new_size;
    }

private:
    //! Tag to specify non-initialized range of elements.
    struct non_initialized_t {};

    /*!
     * \brief Constructor.
     *
     * \param[in] size Size.
     *
     * \warning This constructor does not initialize the elements.
     * So, the values of the elements may be arbitrary.
     */
    trivial_vector(size_type size, non_initialized_t /*unused*/)
        : data_(nullptr),
          size_(size),
          capacity_(std::max(size, default_capacity)) {
        if (size_ < 0) {
            throw invalid_argument(
                "Negative size was given to trivial_vector.");
        }
        if (size_ > max_size()) {
            throw invalid_argument(
                "Too large size was given to trivial_vector.");
        }

        data_ = allocate(capacity_);
    }

    /*!
     * \brief Change the size of this vector.
     *
     * \param[in] new_size New size.
     */
    void resize(size_type new_size, non_initialized_t /*unused*/) {
        if (new_size < 0) {
            throw invalid_argument("Negative size was given to resize.");
        }
        if (new_size > max_size()) {
            throw invalid_argument("Too large size was given to resize.");
        }
        if (new_size > capacity_) {
            data_ = reallocate(data_, new_size);
            capacity_ = new_size;
        }
        size_ = new_size;
    }

    /*!
     * \brief Allocate memory.
     *
     * \param[in] capacity Capacity.
     * \return Pointer to the allocated memory.
     */
    static auto allocate(size_type capacity) -> pointer {
        NUM_COLLECT_ASSERT(capacity > 0);

        // NOLINTNEXTLINE(*-no-malloc)
        void* ptr = std::malloc(
            static_cast<std::size_t>(capacity) * sizeof(value_type));
        if (ptr == nullptr) {
            throw std::bad_alloc();
        }
        return static_cast<pointer>(ptr);
    }

    /*!
     * \brief Reallocate memory.
     *
     * \param[in] ptr Pointer to the memory.
     * \param[in] new_capacity New capacity.
     * \return Pointer to the reallocated memory.
     */
    static auto reallocate(pointer ptr, size_type new_capacity) -> pointer {
        NUM_COLLECT_ASSERT(new_capacity > 0);

        // NOLINTNEXTLINE(*-no-malloc)
        void* new_ptr = std::realloc(static_cast<void*>(ptr),
            static_cast<std::size_t>(new_capacity) * sizeof(value_type));
        if (new_ptr == nullptr) {
            throw std::bad_alloc();
        }
        return static_cast<pointer>(new_ptr);
    }

    /*!
     * \brief Deallocate memory.
     *
     * \param[in] ptr Pointer to the memory.
     *
     * \note This function can accept nullptr.
     */
    static void deallocate(pointer ptr) noexcept {
        // NOLINTNEXTLINE(*-no-malloc)
        std::free(static_cast<void*>(ptr));
    }

    //! Default capacity.
    static constexpr size_type default_capacity = 8;

    //! Pointer to the data.
    pointer data_;

    //! Size.
    size_type size_;

    //! Capacity.
    size_type capacity_;
};

}  // namespace num_collect::util
