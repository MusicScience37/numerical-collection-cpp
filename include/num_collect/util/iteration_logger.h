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
 * \brief Definition of iteration_logger class.
 */
#pragma once

#include <iostream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#include <fmt/core.h>
#include <fmt/format.h>

#include "num_collect/util/index_type.h"

namespace num_collect {

/*!
 * \brief Class to write log of iterations in algorithms.
 */
class iteration_logger {
public:
    /*!
     * \brief Class of items in log of iteration_logger class.
     */
    class log_item {
    public:
        /*!
         * \brief Construct.
         */
        log_item() = default;

        /*!
         * \brief Insert a value.
         *
         * \param[in] value Value.
         * \return This object.
         */
        auto operator=(index_type value) -> log_item& {
            value_ = value;
            return *this;
        }

        /*!
         * \brief Insert a value.
         *
         * \param[in] value Value.
         * \return This object.
         */
        auto operator=(double value) -> log_item& {
            value_ = value;
            return *this;
        }

        /*!
         * \brief Insert a value.
         *
         * \param[in] value Value.
         * \return This object.
         */
        auto operator=(std::string value) -> log_item& {
            value_ = value;
            return *this;
        }

        /*!
         * \brief Remove the value.
         *
         * \return This object.
         */
        auto operator=(std::nullptr_t /*unused*/) -> log_item& {
            value_ = std::monostate();
            return *this;
        }

        /*!
         * \brief Set precision.
         *
         * \param[in] value Value.
         * \return This object.
         */
        auto precision(index_type value) -> log_item& {
            if (value <= 0) {
                throw std::invalid_argument(
                    "precision in iteration_logger must be a positive integer");
            }
            precision_ = value;
            return *this;
        }

        /*!
         * \brief Set width.
         *
         * \param[in] value Value
         * \return This object.
         */
        auto width(index_type value) -> log_item& {
            if (value <= 0) {
                throw std::invalid_argument(
                    "width in iteration_logger must be a positive integer");
            }
            width_ = value;
            return *this;
        }

        /*!
         * \brief Format value and write to stream.
         *
         * \param[in] stream Stream.
         */
        void format_to(std::ostream& stream) const {
            std::visit(
                [&stream, this](const auto& value) {
                    if constexpr (std::is_same_v<std::decay_t<decltype(value)>,
                                      std::monostate>) {
                        stream << std::string(
                            static_cast<std::size_t>(width_), ' ');
                    } else if constexpr (std::is_same_v<
                                             std::decay_t<decltype(value)>,
                                             double>) {
                        stream << fmt::format(
                            "{:>{}.{}}", value, width_, precision_);
                    } else {
                        stream << fmt::format("{:>{}}", value, width_);
                    }
                },
                value_);
        }

        /*!
         * \brief Get width.
         *
         * \return Width.
         */
        [[nodiscard]] auto width() const -> index_type { return width_; }

    private:
        //! Value.
        std::variant<std::monostate, index_type, double, std::string> value_{};

        //! Default precision.
        static constexpr index_type default_precision = 4;

        //! Precision.
        index_type precision_{default_precision};

        //! Default width.
        static constexpr index_type default_width = 12;

        //! Width.
        index_type width_{default_width};
    };

    /*!
     * \brief Construct.
     */
    iteration_logger() = default;

    /*!
     * \brief Access a log item.
     *
     * \param[in] title Title.
     * \return Log item.
     */
    auto operator[](const std::string& title) -> log_item& {
        auto [iter, inserted] = items_.try_emplace(title);
        if (inserted) {
            item_order_.push_back(title);
        }
        return iter->second;
    }

    /*!
     * \brief Access a log item.
     *
     * \param[in] title Title.
     * \return Log item.
     */
    auto operator[](const std::string& title) const -> const log_item& {
        return items_.at(title);
    }

    /*!
     * \brief Write latest values to a stream.
     *
     * \param[in] stream stream
     */
    void write_to(std::ostream& stream) {
        if ((iterations_ % log_output_period_) != 0) {
            ++iterations_;
            return;
        }
        if ((iterations_ % (log_output_period_ * title_output_period_)) == 0) {
            for (const auto& title : item_order_) {
                stream << fmt::format(
                    "{:>{}}", title, items_.at(title).width());
            }
            stream << "\n";
        }
        for (const auto& title : item_order_) {
            items_.at(title).format_to(stream);
        }
        // flush each line
        stream << std::endl;
        ++iterations_;
    }

    /*!
     * \brief Set log output period.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto log_output_period(index_type value) -> iteration_logger& {
        if (value <= 0) {
            throw std::invalid_argument(
                "log output period in iteration_logger must be a positive "
                "integer");
        }
        log_output_period_ = value;
        return *this;
    }

    /*!
     * \brief Set title output period.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto title_output_period(index_type value) -> iteration_logger& {
        if (value <= 0) {
            throw std::invalid_argument(
                "title output period in iteration_logger must be a positive "
                "integer");
        }
        title_output_period_ = value;
        return *this;
    }

private:
    //! Log items.
    std::unordered_map<std::string, log_item> items_{};

    //! Order of log items.
    std::vector<std::string> item_order_{};

    //! Number of iterations.
    index_type iterations_{0};

    //! Log output period.
    index_type log_output_period_{1};

    //! Default title output period.
    static constexpr index_type default_title_output_period = 20;

    //! Title output period.
    index_type title_output_period_{default_title_output_period};
};

}  // namespace num_collect
