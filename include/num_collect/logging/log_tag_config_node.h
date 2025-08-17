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
 * \brief Definition of log_tag_config_node class.
 */
#pragma once

#include "num_collect/base/index_type.h"
#include "num_collect/impl/num_collect_export.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/sinks/log_sink.h"

namespace num_collect::logging {

namespace impl {

/*!
 * \brief Struct of internal data of \ref log_tag_config_node.
 */
struct log_tag_config_node_impl;

}  // namespace impl

/*!
 * \brief Class of nodes in the logging configuration tree.
 *
 * \note This class hold pointer to the globally allocated data.
 * So copy and move of instances have no effect.
 * \note Change of parameters are reflected to the global data including child
 * nodes.
 *
 * \thread_safety Thread safe.
 */
class NUM_COLLECT_EXPORT log_tag_config_node {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] impl Pointer to the internal data.
     *
     * \warning This constructor cannot be used from user code.
     */
    explicit log_tag_config_node(impl::log_tag_config_node_impl* impl) noexcept;

    /*!
     * \brief Copy constructor.
     *
     * \param[in] other Another instance to copy from.
     */
    log_tag_config_node(const log_tag_config_node& other) noexcept = default;

    /*!
     * \brief Move constructor.
     *
     * \param[in] other Another instance to move from.
     */
    log_tag_config_node(log_tag_config_node&& other) noexcept = default;

    /*!
     * \brief Copy assignment operator.
     *
     * \param[in] other Another instance to copy from.
     * \return Reference to this instance.
     */
    auto operator=(const log_tag_config_node& other) noexcept
        -> log_tag_config_node& = default;

    /*!
     * \brief Move assignment operator.
     *
     * \param[in] other Another instance to move from.
     * \return Reference to this instance.
     */
    auto operator=(log_tag_config_node&& other) noexcept
        -> log_tag_config_node& = default;

    /*!
     * \brief Destructor.
     */
    ~log_tag_config_node() = default;

    /*!
     * \brief Get the log sink.
     *
     * \return Log sink.
     */
    [[nodiscard]] auto sink() const noexcept -> const sinks::log_sink&;

    /*!
     * \brief Set the log sink.
     *
     * \param[in] val Log sink.
     * \return This.
     */
    auto sink(const sinks::log_sink& val) -> log_tag_config_node&;

    /*!
     * \brief Get the minimum log level to output.
     *
     * \return Value.
     */
    [[nodiscard]] auto output_log_level() const noexcept -> log_level;

    /*!
     * \brief Set the minimum log level to output.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto output_log_level(log_level val) -> log_tag_config_node&;

    /*!
     * \brief Get the minimum log level to output in child iterations.
     *
     * \return Value.
     */
    [[nodiscard]] auto output_log_level_in_child_iterations() const noexcept
        -> log_level;

    /*!
     * \brief Set the minimum log level to output in child iterations.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto output_log_level_in_child_iterations(log_level val)
        -> log_tag_config_node&;

    /*!
     * \brief Get the period to write iteration logs.
     *
     * \return value.
     */
    [[nodiscard]] auto iteration_output_period() const noexcept -> index_type;

    /*!
     * \brief Set the period to write iteration logs.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto iteration_output_period(index_type val) -> log_tag_config_node&;

    /*!
     * \brief Get the period to write labels of iteration logs.
     *
     * \return value.
     */
    [[nodiscard]] auto iteration_label_period() const noexcept -> index_type;

    /*!
     * \brief Set the period to write labels of iteration logs.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto iteration_label_period(index_type val) -> log_tag_config_node&;

private:
    //! Internal data.
    impl::log_tag_config_node_impl* impl_;
};

/*!
 * \brief Get the node of logging configuration for a log tag for setting
 * parameters.
 *
 * \param[in] log_tag Log tag.
 * \return Node of logging configuration.
 */
[[nodiscard]] NUM_COLLECT_EXPORT auto edit_log_tag_config(log_tag_view log_tag)
    -> log_tag_config_node;

/*!
 * \brief Get the node of the default logging configuration for setting
 * parameters.
 *
 * \note The default configuration is the configuration in the root node in the
 * logging configuration tree.
 *
 * \return Node of logging configuration.
 */
[[nodiscard]] NUM_COLLECT_EXPORT auto edit_default_log_tag_config()
    -> log_tag_config_node;

}  // namespace num_collect::logging
