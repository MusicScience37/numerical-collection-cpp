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
 * \brief Definition of adc_group class.
 */
#pragma once

#include <queue>
#include <utility>
#include <vector>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/opt/impl/adc_rectangle.h"
#include "num_collect/util/assert.h"

namespace num_collect::opt::impl {

/*!
 * \brief Class of groups in \cite Sergeyev2006 for \ref
 * num_collect::opt::adaptive_diagonal_curves.
 *
 * \tparam Value Type of function values.
 * \tparam TernaryVector Type of ternary vectors.
 */
template <base::concepts::real_scalar Value, typename TernaryVector>
class adc_group {
public:
    //! Type of function values.
    using value_type = Value;

    //! Type of hyper-rectangles.
    using rectangle_type = adc_rectangle<value_type, TernaryVector>;

    /*!
     * \brief Constructor.
     *
     * \param[in] dist Distance between center point and vertex.
     */
    explicit adc_group(value_type dist) : dist_(dist) {}

    /*!
     * \brief Add a hyper-rectangle to this group.
     *
     * \param[in] rect Rectangle.
     */
    void push(rectangle_type rect) { rects_.push(std::move(rect)); }

    /*!
     * \brief Access the hyper-rectangle with the smallest average of
     * function values at diagonal vertices.
     *
     * \return Reference to the rectangle.
     */
    [[nodiscard]] auto min_rect() const -> const rectangle_type& {
        NUM_COLLECT_DEBUG_ASSERT(!rects_.empty());
        return rects_.top();
    }

    /*!
     * \brief Check whether this group is empty.
     *
     * \return Whether this group is empty.
     */
    [[nodiscard]] auto empty() const -> bool { return rects_.empty(); }

    /*!
     * \brief Pick out the hyper-rectangle with the smallest average of function
     * values at diagonal vertices.
     *
     * \return Rectangle.
     */
    [[nodiscard]] auto pop() -> rectangle_type {
        NUM_COLLECT_DEBUG_ASSERT(!rects_.empty());
        auto rect = std::move(rects_.top());
        rects_.pop();
        return rect;
    }

    /*!
     * \brief Check whether the hyper-rectangle in this group can be divided.
     *
     * \retval true The hyper-rectangle can be divided.
     * \retval false The hyper-rectangle cannot be divided.
     */
    [[nodiscard]] auto is_dividable() const -> bool {
        if (rects_.empty()) {
            return false;
        }
        const auto& rect = rects_.top();
        return !rect.vertex().is_full();
    }

    /*!
     * \brief Get the distance between center point and vertex.
     *
     * \return Distance between center point and vertex.
     */
    [[nodiscard]] auto dist() const -> const value_type& { return dist_; }

private:
    /*!
     * \brief Class to compare rectangles.
     */
    struct greater {
        /*!
         * \brief Compare rectangles.
         *
         * \param[in] left Left-hand-side rectangle.
         * \param[in] right Right-hand-side rectangle.
         * \return Result of left > right.
         */
        [[nodiscard]] auto operator()(const rectangle_type& left,
            const rectangle_type& right) const -> bool {
            return left.ave_value() > right.ave_value();
        }
    };

    //! Type of queues of rectangles.
    using queue_type = std::priority_queue<rectangle_type,
        std::vector<rectangle_type>, greater>;

    //! Rectangles.
    queue_type rects_{};

    //! Distance between center point and vertex.
    value_type dist_;
};

}  // namespace num_collect::opt::impl
