/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of vector class.
 */
#pragma once

#include <cstddef>
#include <initializer_list>
#include <utility>
#include <vector>

#include "num_collect/base/index_type.h"
#include "num_collect/util/trivial_vector.h"

namespace num_collect::util {

/*!
 * \brief Class of vectors wrapping std::vector class to use singed integers as
 * indices.
 *
 * \tparam T Type of values.
 */
template <typename T>
class vector {
public:
    //! Type of actual vectors.
    using internal_vector_type = std::vector<T>;

    //! Type of values.
    using value_type = typename internal_vector_type::value_type;

    //! Type of sizes.
    using size_type = index_type;

    //! Type of differences of pointers.
    using difference_type = typename internal_vector_type::difference_type;

    //! Type of references.
    using reference = typename internal_vector_type::reference;

    //! Type of const references.
    using const_reference = typename internal_vector_type::const_reference;

    //! Type of pointers.
    using pointer = typename internal_vector_type::pointer;

    //! Type of const pointers.
    using const_pointer = typename internal_vector_type::const_pointer;

    //! Type of iterators.
    using iterator = typename internal_vector_type::iterator;

    //! Type of const iterators.
    using const_iterator = typename internal_vector_type::const_iterator;

    //! Type of reverse iterators.
    using reverse_iterator = typename internal_vector_type::reverse_iterator;

    //! Type of const reverse iterators.
    using const_reverse_iterator =
        typename internal_vector_type::const_reverse_iterator;

    /*!
     * \brief Constructor.
     *
     * This create an empty vector.
     */
    vector() = default;

    /*!
     * \brief Constructor.
     *
     * \param[in] size Size.
     * \param[in] value Values to fill the vector.
     */
    explicit vector(index_type size, const value_type& value = value_type())
        : vector_(static_cast<std::size_t>(size), value) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] values Initializer list of values.
     */
    vector(std::initializer_list<T> values) : vector_(values) {}

    /*!
     * \brief Access to an element.
     *
     * \param[in] index Index.
     * \return Reference to the element.
     */
    [[nodiscard]] auto operator[](index_type index) -> reference {
        return vector_[static_cast<std::size_t>(index)];
    }

    /*!
     * \brief Access to an element.
     *
     * \param[in] index Index.
     * \return Reference to the element.
     */
    [[nodiscard]] auto operator[](index_type index) const -> const_reference {
        return vector_[static_cast<std::size_t>(index)];
    }

    /*!
     * \brief Access to the front element.
     *
     * \return Reference.
     */
    [[nodiscard]] auto front() -> reference { return vector_.front(); }

    /*!
     * \brief Access to the front element.
     *
     * \return Reference.
     */
    [[nodiscard]] auto front() const -> const_reference {
        return vector_.front();
    }

    /*!
     * \brief Access to the final element.
     *
     * \return Reference.
     */
    [[nodiscard]] auto back() -> reference { return vector_.back(); }

    /*!
     * \brief Access to the final element.
     *
     * \return Reference.
     */
    [[nodiscard]] auto back() const -> const_reference {
        return vector_.back();
    }

    /*!
     * \brief Get the iterator to the first element.
     *
     * \return Iterator.
     */
    [[nodiscard]] auto begin() -> iterator { return vector_.begin(); }

    /*!
     * \brief Get the iterator to the first element.
     *
     * \return Iterator.
     */
    [[nodiscard]] auto begin() const -> const_iterator {
        return vector_.begin();
    }

    /*!
     * \brief Get the iterator to the first element.
     *
     * \return Iterator.
     */
    [[nodiscard]] auto cbegin() const -> const_iterator {
        return vector_.begin();
    }

    /*!
     * \brief Get the past-the-end iterator.
     *
     * \return Iterator.
     */
    [[nodiscard]] auto end() -> iterator { return vector_.end(); }

    /*!
     * \brief Get the past-the-end iterator.
     *
     * \return Iterator.
     */
    [[nodiscard]] auto end() const -> const_iterator { return vector_.end(); }

    /*!
     * \brief Get the past-the-end iterator.
     *
     * \return Iterator.
     */
    [[nodiscard]] auto cend() const -> const_iterator { return vector_.end(); }

    /*!
     * \brief Get the pointer to the first element.
     *
     * \return Pointer.
     */
    [[nodiscard]] auto data() noexcept -> pointer { return vector_.data(); }

    /*!
     * \brief Get the pointer to the first element.
     *
     * \return Pointer.
     */
    [[nodiscard]] auto data() const noexcept -> const_pointer {
        return vector_.data();
    }

    /*!
     * \brief Check whether this vector is empty.
     *
     * \retval true This vector is empty.
     * \retval false This vector is not empty.
     */
    [[nodiscard]] auto empty() const -> bool { return vector_.empty(); }

    /*!
     * \brief Get the size of this vector.
     *
     * \return Size.
     */
    [[nodiscard]] auto size() const -> index_type {
        return static_cast<index_type>(vector_.size());
    }

    /*!
     * \brief Reserve memory.
     *
     * \param[in] size Size.
     */
    void reserve(index_type size) {
        vector_.reserve(static_cast<std::size_t>(size));
    }

    /*!
     * \brief Change the size.
     *
     * \param[in] size Size.
     */
    void resize(index_type size) {
        vector_.resize(static_cast<std::size_t>(size));
    }

    /*!
     * \brief Remove the all elements in this vector.
     */
    void clear() { vector_.clear(); }

    /*!
     * \brief Add an element.
     *
     * \param[in] value Value of the element.
     */
    void push_back(const value_type& value) { vector_.push_back(value); }

    /*!
     * \brief Add an element.
     *
     * \param[in] value Value of the element.
     */
    void push_back(value_type&& value) { vector_.push_back(std::move(value)); }

    /*!
     * \brief Add an element.
     *
     * \tparam Args Type of the arguments of the constructor.
     * \param[in] args Arguments of the constructor.
     * \return Reference to the added element.
     */
    template <typename... Args>
    auto emplace_back(Args&&... args) -> reference {
        return vector_.emplace_back(std::forward<Args>(args)...);
    }

    /*!
     * \brief Insert an element.
     *
     * \param[in] pos Position to insert.
     * \param[in] value Inserted value.
     * \return Iterator to the inserted element.
     */
    auto insert(const_iterator pos, const value_type& value) -> iterator {
        return vector_.insert(pos, value);
    }

    /*!
     * \brief Insert an element.
     *
     * \param[in] pos Position to insert.
     * \param[in] value Inserted value.
     * \return Iterator to the inserted element.
     */
    auto insert(const_iterator pos, value_type&& value) -> iterator {
        return vector_.insert(pos, std::move(value));
    }

    /*!
     * \brief Erase an element.
     *
     * \param[in] pos Position to erase.
     * \return Iterator to the next element of the erased one.
     */
    auto erase(const_iterator pos) -> iterator { return vector_.erase(pos); }

private:
    //! Actual vector.
    internal_vector_type vector_;
};

/*!
 * \brief Class of vectors wrapping std::vector class to use singed integers as
 * indices.
 *
 * \tparam T Type of values.
 *
 * This specialization uses faster implementation for trivially copyable types.
 */
template <trivial_vector_compatible T>
class vector<T> : public trivial_vector<T> {
public:
    using trivial_vector<T>::trivial_vector;
};

}  // namespace num_collect::util
