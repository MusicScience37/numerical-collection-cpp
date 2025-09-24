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
 * \brief Definition of iteration_period_checker class.
 */
#pragma once

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/logging_macros.h"

namespace num_collect::util {

/*!
 * \brief Class to check periods of iterations.
 */
class iteration_period_checker {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] period Period.
     */
    explicit iteration_period_checker(index_type period) : period_(period) {
        NUM_COLLECT_PRECONDITION(
            period > 0, "Period of iterations must be a positive integer.");
    }

    /*!
     * \brief Iterate.
     */
    void iterate() noexcept {
        ++counter_;
        counter_ %= period_;  // period_ is checked in constructor.
    }

    /*!
     * \brief Check whether the current iteration is a start of the period.
     *
     * \return Whether the current iteration is a start of the period.
     */
    [[nodiscard]] auto is_start_of_period() const noexcept {
        return counter_ == 0;
    }

    /*!
     * \brief Reset the counter.
     */
    void reset() noexcept { counter_ = 0; }

    /*!
     * \brief Iterate.
     *
     * \return This.
     */
    auto operator++() noexcept -> iteration_period_checker& {
        iterate();
        return *this;
    }

    /*!
     * \brief Check whether the current iteration is a start of the period.
     *
     * \return Whether the current iteration is a start of the period.
     */
    explicit operator bool() const noexcept { return is_start_of_period(); }

private:
    //! Period.
    index_type period_;

    //! Iteration counter.
    index_type counter_{0};
};

}  // namespace num_collect::util
