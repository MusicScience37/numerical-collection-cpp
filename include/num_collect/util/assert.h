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
 * \brief Definition of assertion macros.
 */
#pragma once

#include <iterator>

#include <fmt/format.h>

#include "num_collect/base/exception.h"
#include "num_collect/logging/log_and_throw.h"
#include "num_collect/util/impl/assertion_decomposer.h"
#include "num_collect/util/source_info_view.h"

namespace num_collect::util::impl {

/*!
 * \brief Evaluate assertion.
 *
 * \tparam Derived Type of the derived class of assertion_expression_base.
 * \param[in] assertion Assertion expression.
 * \param[in] condition_str Raw string expression of the condition.
 * \param[in] source Information of the source code.
 */
template <typename Derived>
void evaluate_assertion(const assertion_expression_base<Derived>& assertion,
    std::string_view condition_str,
    source_info_view source = source_info_view()) {
    if (assertion.evaluate_to_bool()) {
        return;
    }

    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer),
        FMT_STRING("Assertion failure: {} ({})."), condition_str,
        assertion.derived());
    logging::log_and_throw<assertion_failure>(
        std::string_view(buffer.data(), buffer.size()), source);
}

}  // namespace num_collect::util::impl

/*!
 * \brief Macro to check whether a condition is satisfied.
 *
 * \param[in] CONDITION Condition.
 */
#define NUM_COLLECT_ASSERT_IMPL(CONDITION) /* NOLINT */      \
    ::num_collect::util::impl::evaluate_assertion(           \
        (::num_collect::util::impl::assertion_decomposer() < \
            CONDITION), /* NOLINT */                         \
        (#CONDITION))
