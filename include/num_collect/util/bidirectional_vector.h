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
 * \brief Definition of bidirectional_vector class.
 */
#pragma once

#include <cstddef>
#include <deque>
#include <limits>
#include <utility>

#include <fmt/format.h>  // IWYU pragma: keep

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/util/assert.h"

namespace num_collect::util {

/*!
 * \brief Class to save data in a sequence which can be extended even toward
 * negative direction.
 *
 * This class generalizes `std::vector` class.
 * Indices can be positive integers, zero, or negative integers.
 *
 * \tparam Value Type of values.
 * \tparam Container Type of the internal container. (Assumed to be
 * `std::deque`.)
 */
template <typename Value, typename Container = std::deque<Value>>
class bidirectional_vector {
public:
    //! Type of values.
    using value_type = Value;

    //! Type of the internal container.
    using container_type = Container;

    /*!
     * \brief Constructor.
     */
    bidirectional_vector() = default;

    /*!
     * \brief Constructor.
     *
     * \param[in] container Internal container.
     * \param[in] origin_index Index of origin.
     */
    bidirectional_vector(container_type container, index_type origin_index)
        : container_(std::move(container)), origin_index_(origin_index) {}

    /*!
     * \brief Get the internal container.
     *
     * \return Internal container.
     */
    [[nodiscard]] auto container() const noexcept -> const container_type& {
        return container_;
    }

    /*!
     * \brief Check whether this object is empty.
     *
     * \return Result.
     */
    [[nodiscard]] auto empty() const noexcept -> bool {
        return container_.empty();
    }

    /*!
     * \brief Get the minimum index. (Equal to the index of the origin.)
     *
     * \return Minimum index.
     */
    [[nodiscard]] auto min_index() const noexcept -> index_type {
        return origin_index_;
    }

    /*!
     * \brief Get the maximum index.
     *
     * \return Maximum index.
     */
    [[nodiscard]] auto max_index() const noexcept -> index_type {
        return origin_index_ + static_cast<index_type>(container_.size()) - 1;
    }

    /*!
     * \brief Access a value with checks.
     *
     * This version throws for indices out of range.
     *
     * \param[in] index Index.
     * \return Value.
     */
    [[nodiscard]] auto at(index_type index) const -> const value_type& {
        return container_[container_index(index)];
    }

    /*!
     * \brief Access a value with checks.
     *
     * This version throws for indices out of range.
     *
     * \param[in] index Index.
     * \return Value.
     */
    [[nodiscard]] auto at(index_type index) -> value_type& {
        return container_[container_index(index)];
    }

    /*!
     * \brief Access a value without checks.
     *
     * Access to indices out of range causes undefined behaviour.
     *
     * \param[in] index Index.
     * \return Value.
     */
    [[nodiscard]] auto operator[](index_type index) const -> const value_type& {
        return container_[unsafe_container_index(index)];
    }

    /*!
     * \brief Access a value without checks.
     *
     * Access to indices out of range causes undefined behaviour.
     *
     * \param[in] index Index.
     * \return Value.
     */
    [[nodiscard]] auto operator[](index_type index) -> value_type& {
        return container_[unsafe_container_index(index)];
    }

    /*!
     * \brief Access a value preparing it if needed.
     *
     * \param[in] index Index.
     * \return Value.
     */
    [[nodiscard]] auto get_or_prepare(index_type index) -> value_type& {
        prepare_for(index);
        return container_[unsafe_container_index(index)];
    }

    /*!
     * \brief Change the position of this object.
     *
     * \param[in] min_index Minimum index.
     * \param[in] max_index Maximum index.
     * \param[in] value Additional values.
     */
    void resize(index_type min_index, index_type max_index,
        const value_type& value = value_type()) {
        const index_type current_min_index = this->min_index();
        const index_type current_max_index = this->max_index();

        if (max_index < current_min_index || current_max_index < min_index) {
            origin_index_ = min_index;
            const auto next_size =
                static_cast<std::size_t>(max_index - min_index + 1);
            container_.resize(next_size);
            for (auto& elem : container_) {
                elem = value;
            }
            return;
        }

        if (min_index < current_min_index) {
            const auto num_added =
                static_cast<std::size_t>(current_min_index - min_index);
            container_.insert(container_.begin(), num_added, value);
        } else if (min_index > current_min_index) {
            const index_type num_erased = min_index - current_min_index;
            container_.erase(
                container_.begin(), container_.begin() + num_erased);
        }
        origin_index_ = min_index;

        if (max_index > current_max_index) {
            const auto num_added =
                static_cast<std::size_t>(max_index - current_max_index);
            container_.insert(container_.end(), num_added, value);
        } else if (max_index < current_max_index) {
            const index_type num_erased = current_max_index - max_index;
            container_.erase(container_.end() - num_erased, container_.end());
        }
    }

    /*!
     * \brief Add a value to the beginning.
     *
     * \param[in] value Value.
     */
    void push_front(const value_type& value) {
        NUM_COLLECT_ASSERT(
            origin_index_ > std::numeric_limits<index_type>::min());
        container_.push_front(value);
        --origin_index_;
    }

    /*!
     * \brief Add a value to the end.
     *
     * \param[in] value Value.
     */
    void push_back(const value_type& value) { container_.push_back(value); }

    /*!
     * \brief Move the position of this object.
     *
     * \param[in] offset Offset from the current position.
     */
    void move_position(index_type offset) {
        NUM_COLLECT_ASSERT(is_safe_offset(offset));
        origin_index_ += offset;
    }

private:
    //! Internal container.
    container_type container_{};

    //! Index of origin.
    index_type origin_index_{0};

    /*!
     * \brief Calculate the index in the container with checks.
     *
     * \param[in] index Index for this object.
     * \return Index in the container.
     */
    [[nodiscard]] auto container_index(index_type index) const -> std::size_t {
        if (index < origin_index_) {
            throw_out_of_range(index);
        }
        const auto result = static_cast<std::size_t>(index - origin_index_);
        if (result >= container_.size()) {
            throw_out_of_range(index);
        }
        return result;
    }

    /*!
     * \brief Calculate the index in the container without checks.
     *
     * \param[in] index Index for this object.
     * \return Index in the container.
     */
    [[nodiscard]] auto unsafe_container_index(index_type index) const noexcept
        -> std::size_t {
        return static_cast<std::size_t>(index - origin_index_);
    }

    /*!
     * \brief Throw exception for indices out of range.
     *
     * \param[in] index Index.
     */
    [[noreturn]] void throw_out_of_range(index_type index) const {
        throw invalid_argument(
            fmt::format("Index out of range (index: {}, range: [{}, {}])",
                index, min_index(), max_index()));
    }

    /*!
     * \brief Prepare space for an index.
     *
     * \param[in] index Index.
     */
    void prepare_for(index_type index) {
        if (container_.empty()) {
            origin_index_ = index;
            container_.push_back(value_type());
            return;
        }

        index_type next_min = min_index();
        index_type next_max = max_index();
        bool should_resize = false;
        if (next_min > index) {
            next_min = index;
            should_resize = true;
        }
        if (next_max < index) {
            next_max = index;
            should_resize = true;
        }
        if (should_resize) {
            resize(next_min, next_max);
        }
    }

    /*!
     * \brief Check whether the given offset is safe to move.
     *
     * \param[in] offset Offset.
     * \return Result.
     */
    [[nodiscard]] auto is_safe_offset(index_type offset) const noexcept
        -> bool {
        if (offset < 0) {
            return origin_index_ >=
                std::numeric_limits<index_type>::min() - offset;
        }
        return max_index() <= std::numeric_limits<index_type>::max() - offset;
    }
};

}  // namespace num_collect::util
