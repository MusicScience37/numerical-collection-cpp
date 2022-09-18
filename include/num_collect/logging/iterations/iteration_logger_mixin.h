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
 * \brief Definition of iteration_logger_mixin class.
 */
#pragma once

#include <optional>

#include "num_collect/logging/iterations/iteration_logger.h"

namespace num_collect::logging::iterations {

/*!
 * \brief Class to incorporate
 * num_collect::logging::iterations::iteration_logger in algorithms.
 *
 * Inherit this class to use num_collect::logging::iterations::iteration_logger
 * with the common interface.
 *
 * \tparam Derived Derived class.
 */
template <typename Derived>
class iteration_logger_mixin {
public:
    /*!
     * \brief Constructor.
     */
    iteration_logger_mixin() = default;

    /*!
     * \brief Configure an iteration logger.
     *
     * \param[in] iteration_logger Iteration logger.
     */
    void configure_iteration_logger(
        num_collect::logging::iterations::iteration_logger<Derived>&
            iteration_logger) const {
        static_cast<const Derived*>(this)->configure_iteration_logger(
            iteration_logger);
    }

    /*!
     * \brief Get the iteration logger.
     *
     * \note This will automatically call configure_iteration_logger function..
     *
     * \return Iteration logger.
     */
    auto initialize_iteration_logger()
        -> num_collect::logging::iterations::iteration_logger<Derived>& {
        if (iteration_logger_) {
            iteration_logger_->start(static_cast<Derived*>(this)->logger());
        } else {
            iteration_logger_.emplace(static_cast<Derived*>(this)->logger());
            configure_iteration_logger(*iteration_logger_);
        }
        return *iteration_logger_;
    }

private:
    //! Iteration logger.
    std::optional<num_collect::logging::iterations::iteration_logger<Derived>>
        iteration_logger_{};
};

}  // namespace num_collect::logging::iterations
