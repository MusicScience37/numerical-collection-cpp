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
 * \brief Implementation of log_tag_config_node class.
 */
#include "num_collect/logging/log_tag_config_node.h"

#include <memory>
#include <mutex>

#include <hash_tables/maps/open_address_map_st.h>

#include "num_collect/logging/impl/log_tag_element.h"
#include "num_collect/logging/impl/separate_top_log_tag_element.h"
#include "num_collect/util/assert.h"

namespace num_collect::logging {

namespace impl {

/*!
 * \brief Struct of internal data of \ref log_tag_config_node.
 */
struct log_tag_config_node_impl {
    //! Configuration of this node.
    log_tag_config config{};

    //! Child nodes.
    hash_tables::maps::open_address_map_st<log_tag_element,
        std::shared_ptr<log_tag_config_node_impl>>
        children{};

    //! Mutex.
    std::mutex mutex{};

    /*!
     * \brief Constructor.
     *
     * \param[in] input_config Configuration of this node.
     */
    explicit log_tag_config_node_impl(log_tag_config input_config)
        : config(std::move(input_config)) {}
};

/*!
 * \brief Get the root logging configuration node.
 *
 * \return Root logging configuration node.
 */
[[nodiscard]] auto get_root_log_tag_config_node_impl()
    -> log_tag_config_node_impl* {
    static log_tag_config_node_impl root{log_tag_config{}};
    return &root;
}

}  // namespace impl

log_tag_config_node::log_tag_config_node(
    impl::log_tag_config_node_impl* impl) noexcept
    : impl_(impl) {
    NUM_COLLECT_ASSERT(impl_ != nullptr);
}

auto log_tag_config_node::sink() const noexcept -> const sinks::log_sink& {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    return impl_->config.sink();
}

auto log_tag_config_node::sink(const sinks::log_sink& val)
    -> log_tag_config_node& {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    impl_->config.sink(val);
    impl_->children.for_all(
        [&val](const impl::log_tag_element& /*element*/,
            const std::shared_ptr<impl::log_tag_config_node_impl>& child) {
            child->config.sink(val);
        });
    return *this;
}

auto log_tag_config_node::output_log_level() const noexcept -> log_level {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    return impl_->config.output_log_level();
}

auto log_tag_config_node::output_log_level(log_level val)
    -> log_tag_config_node& {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    impl_->config.output_log_level(val);
    impl_->children.for_all(
        [&val](const impl::log_tag_element& /*element*/,
            const std::shared_ptr<impl::log_tag_config_node_impl>& child) {
            child->config.output_log_level(val);
        });
    return *this;
}

auto log_tag_config_node::output_log_level_in_child_iterations() const noexcept
    -> log_level {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    return impl_->config.output_log_level_in_child_iterations();
}

auto log_tag_config_node::output_log_level_in_child_iterations(log_level val)
    -> log_tag_config_node& {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    impl_->config.output_log_level_in_child_iterations(val);
    impl_->children.for_all(
        [&val](const impl::log_tag_element& /*element*/,
            const std::shared_ptr<impl::log_tag_config_node_impl>& child) {
            child->config.output_log_level_in_child_iterations(val);
        });
    return *this;
}

auto log_tag_config_node::iteration_output_period() const noexcept
    -> index_type {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    return impl_->config.iteration_output_period();
}

auto log_tag_config_node::iteration_output_period(index_type val)
    -> log_tag_config_node& {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    impl_->config.iteration_output_period(val);
    impl_->children.for_all(
        [&val](const impl::log_tag_element& /*element*/,
            const std::shared_ptr<impl::log_tag_config_node_impl>& child) {
            child->config.iteration_output_period(val);
        });
    return *this;
}

auto log_tag_config_node::iteration_label_period() const noexcept
    -> index_type {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    return impl_->config.iteration_label_period();
}

auto log_tag_config_node::iteration_label_period(index_type val)
    -> log_tag_config_node& {
    std::unique_lock<std::mutex> lock(impl_->mutex);
    impl_->config.iteration_label_period(val);
    impl_->children.for_all(
        [&val](const impl::log_tag_element& /*element*/,
            const std::shared_ptr<impl::log_tag_config_node_impl>& child) {
            child->config.iteration_label_period(val);
        });
    return *this;
}

auto get_log_tag_config_node(log_tag_view log_tag) -> log_tag_config_node {
    auto* current = impl::get_root_log_tag_config_node_impl();
    std::string_view remaining_tag = log_tag.name();
    while (!remaining_tag.empty()) {
        const auto [next_element, next_remaining] =
            impl::separate_top_log_tag_element(remaining_tag);

        std::unique_lock<std::mutex> lock(current->mutex);
        auto& child = current->children.get_or_create_with_factory(
            next_element, [current] {
                auto child = std::make_shared<impl::log_tag_config_node_impl>(
                    current->config);
                return child;
            });
        current = child.get();
        lock.unlock();

        remaining_tag = next_remaining;
    }
    return log_tag_config_node(current);
}

auto get_default_log_tag_config_node() -> log_tag_config_node {
    return get_log_tag_config_node(log_tag_view{""});
}

auto get_log_tag_config(log_tag_view log_tag) -> log_tag_config {
    auto* current = impl::get_root_log_tag_config_node_impl();
    std::string_view remaining_tag = log_tag.name();
    while (!remaining_tag.empty()) {
        const auto [next_element, next_remaining] =
            impl::separate_top_log_tag_element(remaining_tag);

        std::unique_lock<std::mutex> lock(current->mutex);
        auto* child = current->children.try_get(next_element);
        if (child == nullptr) {
            break;
        }
        current = child->get();
        lock.unlock();

        remaining_tag = next_remaining;
    }
    return current->config;
}

auto get_default_log_tag_config() -> log_tag_config {
    return get_log_tag_config(log_tag_view{""});
}

}  // namespace num_collect::logging
