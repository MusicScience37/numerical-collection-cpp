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
 * \brief Definition of iteration_logger class.
 */
#pragma once

#include <concepts>
#include <iterator>

#include <fmt/format.h>

#include "num_collect/concepts/floating_point.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/util/exception.h"

namespace num_collect::logging {

namespace impl {

//! Default precision in .num_collect::logging::iteration_logger class.
inline constexpr index_type iteration_logger_default_precision = 4;

//! Default width in .num_collect::logging::iteration_logger class.
inline constexpr index_type iteration_logger_default_width = 11;

}  // namespace impl

/*!
 * \brief Interface of logging items in num_collect::logging::iteration_logger
 * class.
 */
class iteration_logger_item_base {
public:
    /*!
     * \brief Format the current value to a buffer.
     *
     * \param[in] buffer Buffer to write the value.
     */
    virtual void format_value_to(fmt::memory_buffer& buffer) const = 0;

    /*!
     * \brief Format the label to a buffer.
     *
     * \param[in] buffer Buffer to write the value.
     */
    virtual void format_label_to(fmt::memory_buffer& buffer) const = 0;

    iteration_logger_item_base(const iteration_logger_item_base&) = delete;
    iteration_logger_item_base(iteration_logger_item_base&&) = delete;
    auto operator=(const iteration_logger_item_base&)
        -> iteration_logger_item_base& = delete;
    auto operator=(iteration_logger_item_base&&)
        -> iteration_logger_item_base& = delete;

    /*!
     * \brief Destruct.
     */
    virtual ~iteration_logger_item_base() noexcept = default;

protected:
    /*!
     * \brief Construct.
     */
    iteration_logger_item_base() noexcept = default;
};

/*!
 * \brief Class of logging items in num_collect::logging::iteration_logger
 * class.
 *
 * \tparam Value Type of the value.
 */
template <typename Value>
class iteration_logger_item final : public iteration_logger_item_base {
public:
    /*!
     * \brief Construct.
     *
     * \param[in] label Label.
     * \param[in] value Reference to the value.
     */
    iteration_logger_item(std::string label, const Value& value)
        : label_(std::move(label)), value_(value) {}

    /*!
     * \brief Format the current value to a buffer.
     *
     * \param[in] buffer Buffer to write the value.
     */
    void format_value_to(fmt::memory_buffer& buffer) const override {
        fmt::format_to(std::back_inserter(buffer), FMT_STRING("{0: >{1}}"),
            value_, width_);
    }

    /*!
     * \brief Format the label to a buffer.
     *
     * \param[in] buffer Buffer to write the value.
     */
    void format_label_to(fmt::memory_buffer& buffer) const override {
        fmt::format_to(std::back_inserter(buffer), FMT_STRING("{0: >{1}}"),
            label_, width_);
    }

    /*!
     * \brief Set width.
     *
     * \param[in] value Value
     * \return This object.
     */
    auto width(index_type value) -> iteration_logger_item* {
        NUM_COLLECT_ASSERT(value > 0);
        width_ = value;
        return this;
    }

    /*!
     * \brief Get width.
     *
     * \return Width.
     */
    [[nodiscard]] auto width() const -> index_type { return width_; }

    iteration_logger_item(const iteration_logger_item&) = delete;
    iteration_logger_item(iteration_logger_item&&) = delete;
    auto operator=(const iteration_logger_item&)
        -> iteration_logger_item& = delete;
    auto operator=(iteration_logger_item&&) -> iteration_logger_item& = delete;

    /*!
     * \brief Destruct.
     */
    ~iteration_logger_item() noexcept override = default;

private:
    //! Label.
    std::string label_;

    //! Reference to the value.
    const Value& value_;

    //! Width.
    index_type width_{impl::iteration_logger_default_width};
};

/*!
 * \brief Class of logging items in num_collect::logging::iteration_logger
 * class specialized for floating-point values.
 *
 * \tparam Value Type of the value.
 */
template <concepts::floating_point Value>
class iteration_logger_item<Value> final : public iteration_logger_item_base {
public:
    /*!
     * \brief Construct.
     *
     * \param[in] label Label.
     * \param[in] value Reference to the value.
     */
    iteration_logger_item(std::string label, const Value& value)
        : label_(std::move(label)), value_(value) {}

    /*!
     * \brief Format the current value to a buffer.
     *
     * \param[in] buffer Buffer to write the value.
     */
    void format_value_to(fmt::memory_buffer& buffer) const override {
        fmt::format_to(std::back_inserter(buffer), FMT_STRING("{0: >{1}.{2}}"),
            value_, width_, precision_);
    }

    /*!
     * \brief Format the label to a buffer.
     *
     * \param[in] buffer Buffer to write the value.
     */
    void format_label_to(fmt::memory_buffer& buffer) const override {
        fmt::format_to(std::back_inserter(buffer), FMT_STRING("{0: >{1}}"),
            label_, width_);
    }

    /*!
     * \brief Set width.
     *
     * \param[in] value Value
     * \return This object.
     */
    auto width(index_type value) -> iteration_logger_item* {
        NUM_COLLECT_ASSERT(value > 0);
        width_ = value;
        return this;
    }

    /*!
     * \brief Get width.
     *
     * \return Width.
     */
    [[nodiscard]] auto width() const -> index_type { return width_; }

    /*!
     * \brief Set precision.
     *
     * \param[in] value Value
     * \return This object.
     */
    auto precision(index_type value) -> iteration_logger_item* {
        NUM_COLLECT_ASSERT(value > 0);
        precision_ = value;
        return this;
    }

    /*!
     * \brief Get precision.
     *
     * \return Precision.
     */
    [[nodiscard]] auto precision() const -> index_type { return precision_; }

    iteration_logger_item(const iteration_logger_item&) = delete;
    iteration_logger_item(iteration_logger_item&&) = delete;
    auto operator=(const iteration_logger_item&)
        -> iteration_logger_item& = delete;
    auto operator=(iteration_logger_item&&) -> iteration_logger_item& = delete;

    /*!
     * \brief Destruct.
     */
    ~iteration_logger_item() noexcept override = default;

private:
    //! Label.
    std::string label_;

    //! Reference to the value.
    const Value& value_;

    //! Precision.
    index_type precision_{impl::iteration_logger_default_precision};

    //! Width.
    index_type width_{impl::iteration_logger_default_width};
};

}  // namespace num_collect::logging
