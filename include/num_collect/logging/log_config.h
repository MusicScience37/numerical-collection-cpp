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
 * \brief Definition of functions to get and set logging configurations.
 */
#pragma once

#include "num_collect/impl/num_collect_export.h"
#include "num_collect/logging/log_tag_config.h"
#include "num_collect/logging/log_tag_view.h"

namespace num_collect::logging {

/*!
 * \brief Get the default configuration of log tags.
 *
 * \return Default configuration of log tags.
 */
[[nodiscard]] NUM_COLLECT_EXPORT auto get_default_tag_config()
    -> log_tag_config;

/*!
 * \brief Set the default configuration of log tags.
 *
 * \param[in] config Configuration.
 */
NUM_COLLECT_EXPORT void set_default_tag_config(const log_tag_config& config);

/*!
 * \brief Get the configuration of a tag.
 *
 * \param[in] tag Tag.
 * \return Configuration.
 */
[[nodiscard]] NUM_COLLECT_EXPORT auto get_config_of(
    log_tag_view tag) -> log_tag_config;

/*!
 * \brief Set the configuration of a tag.
 *
 * \param[in] tag Tag.
 * \param[in] config Configuration.
 */
NUM_COLLECT_EXPORT void set_config_of(
    log_tag_view tag, const log_tag_config& config);

}  // namespace num_collect::logging
