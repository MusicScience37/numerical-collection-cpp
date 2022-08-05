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
 * \brief Definition of logging_mixin class.
 */
#pragma once

#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"

namespace num_collect::logging {

/*!
 * \brief Class to incorporate logging in algorithms.
 *
 * Inherit this class to use logger with the common interface.
 */
class logging_mixin {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] tag Tag.
     */
    explicit logging_mixin(log_tag_view tag) : logger_(tag) {}

    /*!
     * \brief Access to the logger.
     *
     * \return Logger.
     */
    [[nodiscard]] auto logger() const noexcept
        -> const num_collect::logging::logger& {
        return logger_;
    }

    /*!
     * \brief Access to the logger.
     *
     * \return Logger.
     */
    [[nodiscard]] auto logger() noexcept -> num_collect::logging::logger& {
        return logger_;
    }

private:
    //! Logger.
    num_collect::logging::logger logger_;
};

}  // namespace num_collect::logging
