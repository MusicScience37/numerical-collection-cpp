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

#include <functional>
#include <iterator>
#include <memory>
#include <string_view>
#include <type_traits>

#include <fmt/format.h>

#include "num_collect/base/concepts/formattable.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/exception.h"
#include "num_collect/logging/concepts/formattable_real_scalar.h"
#include "num_collect/logging/concepts/getter_of.h"
#include "num_collect/logging/log_tag.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/util/source_info_view.h"

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
    virtual void format_value_to(fmt::memory_buffer& buffer) = 0;

    /*!
     * \brief Format the label to a buffer.
     *
     * \param[in] buffer Buffer to write the label.
     */
    virtual void format_label_to(fmt::memory_buffer& buffer) = 0;

    /*!
     * \brief Format the summary to a buffer.
     *
     * \param[in] buffer Buffer to write the summary.
     */
    virtual void format_summary_to(fmt::memory_buffer& buffer) = 0;

    /*!
     * \brief Get the label.
     *
     * \return Label.
     */
    [[nodiscard]] virtual auto label() const noexcept -> const std::string& = 0;

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
 * \tparam Func Type of the function to get the value.
 */
template <base::concepts::formattable Value,
    concepts::getter_of<Value> Function>
class iteration_logger_item final : public iteration_logger_item_base {
public:
    /*!
     * \brief Construct.
     *
     * \tparam InputFunction Type of the input function.
     * \param[in] label Label.
     * \param[in] function Function to to get the value.
     */
    template <concepts::getter_of<Value> InputFunction>
    iteration_logger_item(std::string label, InputFunction&& function)
        : label_(std::move(label)),
          function_(std::forward<InputFunction>(function)) {}

    /*!
     * \brief Format the current value to a buffer.
     *
     * \param[in] buffer Buffer to write the value.
     */
    void format_value_to(fmt::memory_buffer& buffer) override {
        fmt::format_to(std::back_inserter(buffer), FMT_STRING("{0: >{1}}"),
            function_(), width_);
    }

    /*!
     * \brief Format the label to a buffer.
     *
     * \param[in] buffer Buffer to write the label.
     */
    void format_label_to(fmt::memory_buffer& buffer) override {
        fmt::format_to(std::back_inserter(buffer), FMT_STRING("{0: >{1}}"),
            label_, width_);
    }

    /*!
     * \brief Format the summary to a buffer.
     *
     * \param[in] buffer Buffer to write the summary.
     */
    void format_summary_to(fmt::memory_buffer& buffer) override {
        fmt::format_to(std::back_inserter(buffer), FMT_STRING("{0}={1}"),
            label_, function_());
    }

    /*!
     * \brief Get the label.
     *
     * \return Label.
     */
    [[nodiscard]] auto label() const noexcept -> const std::string& override {
        return label_;
    }

    /*!
     * \brief Set width.
     *
     * \param[in] value Value
     * \return This object.
     */
    auto width(index_type value) -> iteration_logger_item* {
        if (value <= 0) {
            throw assertion_failure("Width must be a positive number.");
        }
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

    //! Function to get the value.
    Function function_;

    //! Width.
    index_type width_{impl::iteration_logger_default_width};
};

/*!
 * \brief Class of logging items in num_collect::logging::iteration_logger
 * class specialized for floating-point values.
 *
 * \tparam Value Type of the value.
 * \tparam Func Type of the function to get the value.
 */
template <concepts::formattable_real_scalar Value,
    concepts::getter_of<Value> Function>
class iteration_logger_item<Value, Function> final
    : public iteration_logger_item_base {
public:
    /*!
     * \brief Construct.
     *
     * \tparam InputFunction Type of the input function.
     * \param[in] label Label.
     * \param[in] function Function to to get the value.
     */
    template <concepts::getter_of<Value> InputFunction>
    iteration_logger_item(std::string label, InputFunction&& function)
        : label_(std::move(label)),
          function_(std::forward<InputFunction>(function)) {}

    /*!
     * \brief Format the current value to a buffer.
     *
     * \param[in] buffer Buffer to write the value.
     */
    void format_value_to(fmt::memory_buffer& buffer) override {
        fmt::format_to(std::back_inserter(buffer), FMT_STRING("{0: >{1}.{2}}"),
            function_(), width_, precision_);
    }

    /*!
     * \brief Format the label to a buffer.
     *
     * \param[in] buffer Buffer to write the label.
     */
    void format_label_to(fmt::memory_buffer& buffer) override {
        fmt::format_to(std::back_inserter(buffer), FMT_STRING("{0: >{1}}"),
            label_, width_);
    }

    /*!
     * \brief Format the summary to a buffer.
     *
     * \param[in] buffer Buffer to write the summary.
     */
    void format_summary_to(fmt::memory_buffer& buffer) override {
        fmt::format_to(std::back_inserter(buffer), FMT_STRING("{0}={1:.{2}}"),
            label_, function_(), precision_);
    }

    /*!
     * \brief Get the label.
     *
     * \return Label.
     */
    [[nodiscard]] auto label() const noexcept -> const std::string& override {
        return label_;
    }

    /*!
     * \brief Set width.
     *
     * \param[in] value Value
     * \return This object.
     */
    auto width(index_type value) -> iteration_logger_item* {
        if (value <= 0) {
            throw assertion_failure("Width must be a positive number.");
        }
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
        if (value <= 0) {
            throw assertion_failure("Precision must be a positive number.");
        }
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

    //! Function to get the value.
    Function function_;

    //! Precision.
    index_type precision_{impl::iteration_logger_default_precision};

    //! Width.
    index_type width_{impl::iteration_logger_default_width};
};

/*!
 * \brief Class to write logs of iterations.
 *
 * \note This class is not thread safe because this class is meant for use only
 * from single thread at the end of each iteration.
 */
class iteration_logger {
public:
    /*!
     * \brief Construct.
     *
     * \param[in] logger Logger.
     */
    explicit iteration_logger(num_collect::logging::logger& logger)
        : logger_(&logger),
          tag_(logger.tag()),
          write_iterations_(logger.should_log(log_level::iteration)),
          write_summaries_(logger.should_log(log_level::summary)),
          sink_(logger.config().sink()),
          iteration_output_period_(logger.config().iteration_output_period()),
          iteration_label_period_(logger.config().iteration_label_period()) {
        logger.start_iteration();
    }

    iteration_logger(const iteration_logger&) = delete;
    auto operator=(const iteration_logger&) = delete;

    /*!
     * \brief Move constructor.
     */
    iteration_logger(iteration_logger&&) noexcept = default;

    /*!
     * \brief Move assignment operator.
     *
     * \return This.
     */
    auto operator=(iteration_logger&&) -> iteration_logger& = default;

    /*!
     * \brief Destructor.
     */
    ~iteration_logger() noexcept {
        if (logger_ != nullptr) {
            logger_->finish_iteration();
        }
    }

    /*!
     * \brief Reset the iteration count.
     */
    void reset_count() { iterations_ = 0; }

    /*!
     * \brief Append an item.
     *
     * \param[in] item Item.
     */
    void append(std::shared_ptr<iteration_logger_item_base> item) {
        items_.push_back(std::move(item));
    }

    /*!
     * \brief Append an item.
     *
     * \tparam Value Type of the value.
     * \tparam InputFunction Type of the input function.
     * \param[in] label Label.
     * \param[in] function Function to to get the value.
     * \return Item.
     */
    template <base::concepts::formattable Value,
        concepts::getter_of<Value> InputFunction>
    auto append(std::string label, InputFunction&& function) -> std::shared_ptr<
        iteration_logger_item<Value, std::decay_t<InputFunction>>> {
        auto item = std::make_shared<
            iteration_logger_item<Value, std::decay_t<InputFunction>>>(
            std::move(label), std::forward<InputFunction>(function));
        append(item);
        return item;
    }

    /*!
     * \brief Append an item.
     *
     * \tparam Value Type of the value.
     * \param[in] label Label.
     * \param[in] value Reference to the value.
     * \return Item.
     *
     * \note This will hold the reference to the value inside this object.
     */
    template <base::concepts::formattable Value>
    auto append(std::string label, const Value& value) -> std::shared_ptr<
        iteration_logger_item<Value, std::function<const Value&()>>> {
        auto item = std::make_shared<
            iteration_logger_item<Value, std::function<const Value&()>>>(
            std::move(label), [&value]() -> const Value& { return value; });
        append(item);
        return item;
    }

    /*!
     * \brief Format a line of labels.
     *
     * \param[in] buffer Buffer to format to.
     */
    void format_labels_to(fmt::memory_buffer& buffer) const {
        for (const auto& item : items_) {
            buffer.push_back(' ');
            item->format_label_to(buffer);
        }
    }

    /*!
     * \brief Format a line of values.
     *
     * \param[in] buffer Buffer to format to.
     */
    void format_values_to(fmt::memory_buffer& buffer) const {
        for (const auto& item : items_) {
            buffer.push_back(' ');
            item->format_value_to(buffer);
        }
    }

    /*!
     * \brief Format a line of summary.
     *
     * \param[in] buffer Buffer to format to.
     */
    void format_summary_to(fmt::memory_buffer& buffer) const {
        buffer.append(std::string_view("Finished iterations: "));
        bool is_first = true;
        for (const auto& item : items_) {
            if (is_first) {
                is_first = false;
            } else {
                buffer.push_back(',');
                buffer.push_back(' ');
            }
            item->format_summary_to(buffer);
        }
    }

    /*!
     * \brief Write an iteration to a logger.
     *
     * \param[in] source Information of the source code.
     *
     * \note This will write logs taking period configurations into account.
     */
    void write_iteration_to(
        util::source_info_view source = util::source_info_view()) {
        if (!write_iterations_) {
            return;
        }

        if ((iterations_ % iteration_output_period_) != 0) {
            ++iterations_;
            return;
        }

        if ((iterations_ %
                (iteration_label_period_ * iteration_output_period_)) == 0) {
            buffer_.clear();
            format_labels_to(buffer_);
            sink_->write(std::chrono::system_clock::now(), tag_.name(),
                log_level::iteration_label, source,
                std::string_view(buffer_.data(), buffer_.size()));
        }

        buffer_.clear();
        format_values_to(buffer_);
        sink_->write(std::chrono::system_clock::now(), tag_.name(),
            log_level::iteration, source,
            std::string_view(buffer_.data(), buffer_.size()));

        ++iterations_;
    }

    /*!
     * \brief Write a summary to a logger.
     *
     * \param[in] source Information of the source code.
     */
    void write_summary_to(
        util::source_info_view source = util::source_info_view()) {
        if (!write_summaries_) {
            return;
        }

        buffer_.clear();
        format_summary_to(buffer_);
        sink_->write(std::chrono::system_clock::now(), tag_.name(),
            log_level::summary, source,
            std::string_view(buffer_.data(), buffer_.size()));
    }

private:
    //! Logger.
    logger* logger_;

    //! Log tag.
    log_tag_view tag_;

    //! Whether to write iteration logs.
    bool write_iterations_;

    //! Whether to write summary logs.
    bool write_summaries_;

    //! Log sink.
    std::shared_ptr<log_sink_base> sink_;

    //! Period to write iteration logs.
    index_type iteration_output_period_;

    //! Period to write labels of iteration logs.
    index_type iteration_label_period_;

    //! Log items.
    std::vector<std::shared_ptr<iteration_logger_item_base>> items_{};

    //! Current number of iterations.
    index_type iterations_{0};

    //! Buffer of logging data.
    fmt::memory_buffer buffer_{};
};

}  // namespace num_collect::logging
