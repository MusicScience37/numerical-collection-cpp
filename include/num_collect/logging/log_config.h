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

#include <array>
#include <mutex>
#include <vector>

#include "num_collect/logging/log_tag.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"

namespace num_collect::logging {

/*!
 * \brief Class of configurations of logs.
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
        std::unique_lock<std::mutex> lock(mutex_);
        return default_tag_config_;
    }

    /*!
     * \brief Set the default configuration of log tags.
     *
     * \param[in] config Configuration.
     */
    void set_default_tag_config(const log_tag_config& config) {
        std::unique_lock<std::mutex> lock(mutex_);
        default_tag_config_ = config;
    }

    /*!
     * \brief Get the configuration of a tag.
     *
     * \param[in] tag Tag.
     * \return Configuration.
     */
    [[nodiscard]] auto get_config_of(log_tag_view tag) -> log_tag_config {
        auto& bucket = bucket_for(tag);
        std::unique_lock<std::mutex> lock(bucket.mutex);
        auto iter = find_place_of(tag, bucket);
        if (iter == bucket.configs.end() || iter->first != tag) {
            iter = bucket.configs.emplace(
                iter, log_tag(tag), get_default_tag_config());
        }
        return iter->second;
    }

    /*!
     * \brief Set the configuration of a tag.
     *
     * \param[in] tag Tag.
     * \param[in] config Configuration.
     */
    void set_config_of(log_tag_view tag, const log_tag_config& config) {
        auto& bucket = bucket_for(tag);
        std::unique_lock<std::mutex> lock(bucket.mutex);
        auto iter = find_place_of(tag, bucket);
        if (iter == bucket.configs.end() || iter->first != tag) {
            bucket.configs.emplace(iter, log_tag(tag), config);
        } else {
            iter->second = config;
        }
    }

    log_config(const log_config&) = delete;
    log_config(log_config&&) = delete;
    auto operator=(const log_config&) -> log_config& = delete;
    auto operator=(log_config&&) -> log_config& = delete;

    /*!
     * \brief Destruct.
     */
    ~log_config() = default;

private:
    /*!
     * \brief Struct to hold buckets in the dictionary of configurations of log
     * tags.
     */
    struct log_tag_config_dict_bucket {
    public:
        //! Configurations.
        std::vector<std::pair<log_tag, log_tag_config>> configs{};

        //! Mutex.
        std::mutex mutex{};
    };

    //! Number of buckets.
    static constexpr std::size_t num_buckets = 37;

    /*!
     * \brief Construct.
     */
    log_config() = default;

    /*!
     * \brief Access the bucket for a tag.
     *
     * \param[in] tag Tag.
     * \return Reference to the bucket.
     */
    [[nodiscard]] auto bucket_for(log_tag_view tag)
        -> log_tag_config_dict_bucket& {
        return log_tag_configs_[tag.hash() % num_buckets];
    }

    /*!
     * \brief Find place of a tag.
     *
     * \param[in] tag Tag.
     * \param[in] bucket Bucket.
     * \return Iterator for the element.
     */
    [[nodiscard]] static auto find_place_of(
        log_tag_view tag, log_tag_config_dict_bucket& bucket)
        -> std::vector<std::pair<log_tag, log_tag_config>>::iterator {
        return std::lower_bound(bucket.configs.begin(), bucket.configs.end(),
            tag,
            [](const std::pair<log_tag, log_tag_config>& elem,
                const log_tag_view& target) {
                // NOLINTNEXTLINE(modernize-use-nullptr,hicpp-use-nullptr)
                return elem.first < target;  // Bug of clang-tidy-12?
            });
    }

    //! Dictionary of configurations of log tags.
    std::array<log_tag_config_dict_bucket, num_buckets> log_tag_configs_{};

    //! Default configuration of log tags.
    log_tag_config default_tag_config_{};

    //! Mutex for members except for log_tag_configs_.
    std::mutex mutex_{};
};

}  // namespace num_collect::logging
