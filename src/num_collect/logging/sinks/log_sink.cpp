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
 * \brief Implementation of log_sink class.
 */
#include "num_collect/logging/sinks/log_sink.h"

#include <atomic>
#include <cassert>
#include <utility>

namespace num_collect::logging::sinks {

//! Struct to hold reference count.
struct log_sink::reference_count {
    //! Reference count.
    std::atomic<int> count{};
};

log_sink::log_sink(void* user_data, write_function_type write_function,
    finalizer_type finalizer)
    : user_data_(user_data),
      write_function_(write_function),
      finalizer_(finalizer),
      reference_count_(new reference_count{}) {
    assert(user_data_ != nullptr);
    assert(write_function_ != nullptr);
    assert(finalizer_ != nullptr);
    reference_count_->count.store(1, std::memory_order_relaxed);
}

log_sink::log_sink(const log_sink& obj) noexcept
    : user_data_(obj.user_data_),
      write_function_(obj.write_function_),
      finalizer_(obj.finalizer_),
      reference_count_(obj.reference_count_) {
    assert(user_data_ != nullptr);
    assert(write_function_ != nullptr);
    assert(finalizer_ != nullptr);
    assert(reference_count_ != nullptr);
    reference_count_->count.fetch_add(1, std::memory_order_relaxed);
}

log_sink::log_sink(log_sink&& obj) noexcept
    : user_data_(std::exchange(obj.user_data_, nullptr)),
      write_function_(std::exchange(obj.write_function_, nullptr)),
      finalizer_(std::exchange(obj.finalizer_, nullptr)),
      reference_count_(std::exchange(obj.reference_count_, nullptr)) {}

auto log_sink::operator=(const log_sink& obj) noexcept -> log_sink& {
    if (this == &obj) {
        return *this;
    }
    *this = log_sink(obj);
    return *this;
}

auto log_sink::operator=(log_sink&& obj) noexcept -> log_sink& {
    std::swap(user_data_, obj.user_data_);
    std::swap(write_function_, obj.write_function_);
    std::swap(finalizer_, obj.finalizer_);
    std::swap(reference_count_, obj.reference_count_);
    return *this;
}

log_sink::~log_sink() {
    if (reference_count_ == nullptr) {
        return;
    }
    assert(user_data_ != nullptr);
    assert(finalizer_ != nullptr);
    const auto count =
        reference_count_->count.fetch_sub(1, std::memory_order_acq_rel);
    if (count == 1) {
        finalizer_(user_data_);
        delete reference_count_;
    }
}

void log_sink::write(time_stamp time, std::string_view tag, log_level level,
    util::source_info_view source, std::string_view body) const noexcept {
    assert(user_data_ != nullptr);
    assert(write_function_ != nullptr);
    write_function_(user_data_, time, tag, level, source, body);
}

}  // namespace num_collect::logging::sinks
