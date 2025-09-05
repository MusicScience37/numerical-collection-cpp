/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of static_stack class.
 */
#pragma once

#include <cstddef>
#include <new>

#include "num_collect/base/exception.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/logging_macros.h"

namespace num_collect::util {

// NOLINTBEGIN(*-avoid-c-arrays): This class is a container.

/*!
 * \brief Class of stacks using static arrays.
 *
 * \tparam T Type of values in the stack.
 * \tparam StaticArraySize Size of the static array for the stack.
 */
template <typename T, std::size_t StaticArraySize>
class static_stack {
public:
    /*!
     * \brief Constructor.
     */
    static_stack()
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        : begin_(std::launder(reinterpret_cast<T*>(buffer_))),
          end_(begin_),
          storage_end_(begin_ + StaticArraySize) {}

    static_stack(const static_stack&) = delete;
    static_stack(static_stack&&) = delete;
    auto operator=(const static_stack&) -> static_stack& = delete;
    auto operator=(static_stack&&) -> static_stack& = delete;

    /*!
     * \brief Destructor.
     */
    ~static_stack() {
        while (!empty()) {
            pop();
        }
    }

    /*!
     * \brief Push a value.
     *
     * \param[in] value Value.
     */
    void push(const T& value) {
        NUM_COLLECT_PRECONDITION(end_ != storage_end_, "Stack is full.");
        new (end_) T(value);
        ++end_;
    }

    /*!
     * \brief Pop a value.
     */
    void pop() noexcept {
        --end_;
        end_->~T();
    }

    /*!
     * \brief Get the top value.
     *
     * \return Value.
     */
    [[nodiscard]] auto top() const noexcept -> const T& { return *(end_ - 1); }

    /*!
     * \brief Get the top value.
     *
     * \return Value.
     */
    [[nodiscard]] auto top() noexcept -> T& { return *(end_ - 1); }

    /*!
     * \brief Check whether this stack is empty.
     *
     * \retval true This stack is empty.
     * \retval false This stack is not empty.
     */
    [[nodiscard]] auto empty() const noexcept -> bool { return begin_ == end_; }

    /*!
     * \brief Get the current number of values.
     *
     * \return Current number of values.
     */
    [[nodiscard]] auto size() const noexcept -> std::size_t {
        return static_cast<std::size_t>(end_ - begin_);
    }

private:
    //! Buffer for stack.
    alignas(alignof(T)) char buffer_[sizeof(T[StaticArraySize])]{};

    //! Pointer to the first element in the buffer.
    T* begin_;

    //! Pointer to the past-the-end element in the buffer.
    T* end_;

    //! Pointer to the past-the-end storage in the buffer.
    T* storage_end_;
};

// NOLINTEND(*-avoid-c-arrays)

}  // namespace num_collect::util
