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
 * \brief Definition of log_config class.
 */
#pragma once

#include "num_collect/logging/impl/log_tag_config_tree.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"

namespace num_collect::logging {

/*!
 * \brief Class of configurations of logs.
 *
 * \thread_safety Thread-safe for all operations.
 */
class log_config {
public:
    /*!
     * \brief Access the global instance.
     *
     * \return Reference to the global instance.
     */
    [[nodiscard]] static auto instance() -> log_config& {
        static log_config config{};
        return config;
    }

    /*!
     * \brief Get the default configuration of log tags.
     *
     * \return Default configuration of log tags.
     */
    [[nodiscard]] auto get_default_tag_config() -> log_tag_config {
        return get_config_of(log_tag_view(""));
    }

    /*!
     * \brief Set the default configuration of log tags.
     *
     * \param[in] config Configuration.
     */
    void set_default_tag_config(const log_tag_config& config) {
        set_config_of(log_tag_view(""), config);
    }

    /*!
     * \brief Get the configuration of a tag.
     *
     * \param[in] tag Tag.
     * \return Configuration.
     */
    [[nodiscard]] auto get_config_of(log_tag_view tag) -> log_tag_config {
        return tree_.get_config_of(tag);
    }

    /*!
     * \brief Set the configuration of a tag.
     *
     * \param[in] tag Tag.
     * \param[in] config Configuration.
     */
    void set_config_of(log_tag_view tag, const log_tag_config& config) {
        tree_.set_config_of(tag, config);
    }

    log_config(const log_config&) = delete;
    log_config(log_config&&) = delete;
    auto operator=(const log_config&) -> log_config& = delete;
    auto operator=(log_config&&) -> log_config& = delete;

    /*!
     * \brief Destructor.
     */
    ~log_config() = default;

private:
    /*!
     * \brief Constructor.
     */
    log_config() = default;

    //! Tree of configurations.
    impl::log_tag_config_tree tree_{};
};

}  // namespace num_collect::logging
