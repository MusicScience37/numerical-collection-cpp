/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of log_sink class.
 */
#pragma once

#include <string_view>

#include "num_collect/impl/num_collect_export.h"
#include "num_collect/logging/log_level.h"
#include "num_collect/logging/time_stamp.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::logging::sinks {

/*!
 * \brief Class of log sinks.
 *
 * \note This class holds pointers passed to the constructor and their reference
 * count as in std::shared_ptr. A finalizer is called when the reference count
 * comes to zero.
 */
class NUM_COLLECT_EXPORT log_sink {
public:
    /*!
     * \brief Type of functions to write logs.
     *
     * The first argument is the user data given in the constructor.
     * The other arguments are the same as the arguments of write().
     *
     * \note This function is assumed to be thread-safe.
     */
    using write_function_type = void (*)(void* /*user_data*/,
        time_stamp /*time*/, std::string_view /*tag*/, log_level /*level*/,
        util::source_info_view /*source*/, std::string_view /*body*/) noexcept;

    /*!
     * \brief Type of functions to finalize the lgo sink.
     *
     * The argument is the user data given in the constructor.
     */
    using finalizer_type = void (*)(void* /*user_data*/) noexcept;

    /*!
     * \brief Constructor.
     *
     * \param[in] user_data User data.
     * \param[in] write_function Function to write logs.
     * \param[in] finalizer Function to finalize the log sink.
     */
    log_sink(void* user_data, write_function_type write_function,
        finalizer_type finalizer);

    /*!
     * \brief Copy constructor.
     *
     * \param[in] obj Object to copy from.
     */
    log_sink(const log_sink& obj) noexcept;

    /*!
     * \brief Move constructor.
     *
     * \param[in] obj Object to move from.
     */
    log_sink(log_sink&& obj) noexcept;

    /*!
     * \brief Copy assignment operator.
     *
     * \param[in] obj Object to copy from.
     * \return This object.
     */
    auto operator=(const log_sink& obj) noexcept -> log_sink&;

    /*!
     * \brief Move assignment operator.
     *
     * \param[in] obj Object to move from.
     * \return This object.
     */
    auto operator=(log_sink&& obj) noexcept -> log_sink&;

    /*!
     * \brief Destructor.
     */
    ~log_sink();

    /*!
     * \brief Write a log.
     *
     * \param[in] time Time.
     * \param[in] tag Tag.
     * \param[in] level Log level.
     * \param[in] source Information of the source code.
     * \param[in] body Log body.
     *
     * \note This function can be called from multiple threads.
     */
    void write(time_stamp time, std::string_view tag, log_level level,
        util::source_info_view source, std::string_view body) const noexcept;

private:
    //! Struct to hold reference count.
    struct reference_count;

    //! User data.
    void* user_data_;

    //! Function to write logs.
    write_function_type write_function_;

    //! Function to finalize the log sink.
    finalizer_type finalizer_;

    //! Reference count.
    reference_count* reference_count_;
};

}  // namespace num_collect::logging::sinks
