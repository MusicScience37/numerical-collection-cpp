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
 * \brief Definition of log_tag_config_tree class.
 */
#pragma once

#include <memory>
#include <mutex>
#include <string_view>
#include <type_traits>
#include <utility>

#include <hash_tables/hashes/std_hash.h>
#include <hash_tables/maps/separate_shared_chain_map_mt.h>
#include <hash_tables/tables/separate_shared_chain_table_mt.h>

#include "num_collect/logging/impl/log_tag_element.h"
#include "num_collect/logging/impl/separate_top_log_tag_element.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"

namespace num_collect::logging::impl {

/*!
 * \brief Class of nodes in log_tag_config_tree objects.
 */
class log_tag_config_tree_node {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] config Configuration for this node.
     */
    explicit log_tag_config_tree_node(log_tag_config config)
        : config_(std::move(config)) {}

    /*!
     * \brief Get the configuration of a tag.
     *
     * \param[in] tag Log tag without upper levels.
     * \return Configuration.
     */
    [[nodiscard]] auto get_config_of(std::string_view tag) -> log_tag_config {
        if (tag.empty()) {
            std::unique_lock<std::mutex> lock(mutex_);
            return config_;
        }

        const auto [next_element, remaining] =
            separate_top_log_tag_element(tag);
        return get_or_create_child_node(next_element)->get_config_of(remaining);
    }

    /*!
     * \brief Set the configuration of a tag.
     *
     * \param[in] tag Log tag without upper levels.
     * \param[in] config Configuration.
     */
    void set_config_of(std::string_view tag, const log_tag_config& config) {
        if (tag.empty()) {
            std::unique_lock<std::mutex> lock(mutex_);
            config_ = config;
            return;
        }

        const auto [next_element, remaining] =
            separate_top_log_tag_element(tag);
        get_or_create_child_node(next_element)
            ->set_config_of(remaining, config);
    }

private:
    /*!
     * \brief Get or create a child node.
     *
     * \param[in] element Element in the log tag.
     * \return Childe node.
     */
    [[nodiscard]] auto get_or_create_child_node(const log_tag_element& element)
        -> std::shared_ptr<log_tag_config_tree_node> {
        return child_nodes_.get_or_create_with_factory(element, [this] {
            std::unique_lock<std::mutex> lock(mutex_);
            return std::make_shared<log_tag_config_tree_node>(config_);
        });
    }

    //! Configuration for this node.
    log_tag_config config_;

    //! Child nodes.
    hash_tables::maps::separate_shared_chain_map_mt<log_tag_element,
        std::shared_ptr<log_tag_config_tree_node>>
        child_nodes_{};

    //! Mutex of this node.
    std::mutex mutex_{};
};

/*!
 * \brief Class of trees of configurations of log tags.
 */
class log_tag_config_tree {
public:
    /*!
     * \brief Constructor.
     */
    log_tag_config_tree() = default;

    /*!
     * \brief Get the configuration of a tag.
     *
     * \param[in] tag Tag.
     * \return Configuration.
     */
    [[nodiscard]] auto get_config_of(log_tag_view tag) -> log_tag_config {
        return top_node_.get_config_of(tag.name());
    }

    /*!
     * \brief Set the configuration of a tag.
     *
     * \param[in] tag Tag.
     * \param[in] config Configuration.
     */
    void set_config_of(log_tag_view tag, const log_tag_config& config) {
        top_node_.set_config_of(tag.name(), config);
    }

private:
    //! Top level node.
    log_tag_config_tree_node top_node_{log_tag_config{}};
};

}  // namespace num_collect::logging::impl
