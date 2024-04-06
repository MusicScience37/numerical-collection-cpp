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
 * \brief Implementation of time_stamp class.
 */
#include "num_collect/logging/time_stamp.h"

#include <cassert>
#include <ctime>

namespace num_collect::logging {

time_stamp::time_stamp(std::time_t seconds, std::uint32_t nanoseconds) noexcept
    : seconds_(seconds), nanoseconds_(nanoseconds) {}

auto time_stamp::seconds() const noexcept -> std::time_t { return seconds_; }

auto time_stamp::nanoseconds() const noexcept -> std::uint32_t {
    return nanoseconds_;
}

auto time_stamp::now() noexcept -> time_stamp {
    std::timespec spec{};
    const auto result = std::timespec_get(&spec, TIME_UTC);
    assert(result == TIME_UTC);
    (void)result;
    return time_stamp(spec.tv_sec, spec.tv_nsec);
}

}  // namespace num_collect::logging
