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
 * \brief Definition of producer_consumer_circular_queue class.
 */
#pragma once

#include <atomic>
#include <cstddef>
#include <limits>
#include <string>
#include <type_traits>  // IWYU pragma: keep
#include <utility>

#include <fmt/format.h>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/util/cache_line.h"
#include "num_collect/util/object_storage.h"
#include "num_collect/util/safe_cast.h"

namespace num_collect::util {

/*!
 * \brief Class of a queue using a circular buffer and thread-safe for a single
 * producer thread and a single consumer thread.
 *
 * \tparam T Type of elements in the queue.
 *
 * \thread_safety Operations for the same object is thread safe only if a single
 * producer thread and a single consumer thread exists.
 */
template <typename T>
class alignas(cache_line) producer_consumer_circular_queue {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] size Size.
     */
    explicit producer_consumer_circular_queue(index_type size)
        : begin_(new storage_type[get_buffer_size(size)]),
          // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
          end_(begin_ + (size + 1)),
          producer_pos_(begin_),
          consumer_pos_(begin_) {}

    /*!
     * \brief Destructor.
     */
    ~producer_consumer_circular_queue() noexcept {
        while (try_ignore()) {
        }
        delete[] begin_;
    }

    producer_consumer_circular_queue(
        const producer_consumer_circular_queue&) = delete;
    producer_consumer_circular_queue(
        producer_consumer_circular_queue&&) = delete;
    auto operator=(const producer_consumer_circular_queue&) = delete;
    auto operator=(producer_consumer_circular_queue&&) = delete;

    /*!
     * \brief Try to push an element constructing in-place.
     *
     * \tparam Args Types of arguments of the constructor.
     * \param[in] args Arguments of the constructor.
     * \return Whether an element could be pushed.
     */
    template <typename... Args>
    [[nodiscard]] auto try_emplace(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>) -> bool {
        storage_type* const pushed_pos =
            producer_pos_.load(std::memory_order::relaxed);
        storage_type* const next_producer_pos = increment(pushed_pos);
        if (next_producer_pos ==
            consumer_pos_.load(std::memory_order::acquire)) {
            return false;
        }

        pushed_pos->emplace(std::forward<Args>(args)...);
        producer_pos_.store(next_producer_pos, std::memory_order::release);
        return true;
    }

    /*!
     * \brief Try to pop an element.
     *
     * \tparam Output Type of the variable to pop to.
     * \param[in] output Variable to pop to.
     * \return Whether an element could be popped.
     */
    template <typename Output>
    [[nodiscard]] auto try_pop(Output& output) noexcept(
        noexcept(output = std::move(std::declval<T&>()))) -> bool {
        storage_type* const popped_pos =
            consumer_pos_.load(std::memory_order::relaxed);
        if (popped_pos == producer_pos_.load(std::memory_order::acquire)) {
            return false;
        }

        output = std::move(popped_pos->get_ref());
        popped_pos->reset();
        consumer_pos_.store(increment(popped_pos), std::memory_order::release);
        return true;
    }

    /*!
     * \brief Try to pop an element without getting the element.
     *
     * \return Whether an element could be popped.
     */
    [[nodiscard]] auto try_ignore() noexcept -> bool {
        storage_type* const popped_pos =
            consumer_pos_.load(std::memory_order::relaxed);
        if (popped_pos == producer_pos_.load(std::memory_order::acquire)) {
            return false;
        }

        popped_pos->reset();
        consumer_pos_.store(increment(popped_pos), std::memory_order::release);
        return true;
    }

private:
    //! Type of storage.
    using storage_type = object_storage<T, cache_line>;

    /*!
     * \brief Validate a size and get the size of the buffer.
     *
     * \param[in] val Size of the queue.
     * \return Size of the buffer.
     */
    [[nodiscard]] static auto get_buffer_size(index_type val) -> std::size_t {
        if (val <= 0 || val == std::numeric_limits<index_type>::max()) {
            NUM_COLLECT_LOG_AND_THROW(
                invalid_argument, "Invalid queue size {}.", val);
        }
        return safe_cast<std::size_t>(val + 1);
    }

    /*!
     * \brief Increment a pointer with consideration of the loop of the buffer.
     *
     * \param[in] ptr Pointer.
     * \return Incremented pointer.
     */
    [[nodiscard]] auto increment(storage_type* ptr) noexcept -> storage_type* {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        ++ptr;
        if (ptr == end_) {
            ptr = begin_;
        }
        return ptr;
    }

    //! Beginning of the buffer.
    storage_type* begin_;

    //! Past-the-end pointer of the buffer.
    storage_type* end_;

    /*!
     * \brief Position of the producer.
     *
     * Producer will write the next object here.
     */
    std::atomic<storage_type*> producer_pos_;

    /*!
     * \brief Position of the consumer.
     *
     * Consumer will read the next object here.
     */
    std::atomic<storage_type*> consumer_pos_;
};

}  // namespace num_collect::util
