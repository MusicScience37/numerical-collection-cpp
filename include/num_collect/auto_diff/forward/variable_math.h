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
 * \brief Definition of mathemttical functions for variable class.
 */
#pragma once

#include <cmath>

#include "num_collect/auto_diff/forward/variable.h"

namespace num_collect::auto_diff::forward {

namespace impl {

/*!
 * \brief Process a function with one argument.
 *
 * \tparam Value Type of values.
 * \tparam Diff Type of differential coefficients.
 * \param[in] arg Argument.
 * \param[in] val New value.
 * \param[in] sensitivity Partial differential coefficient.
 * \return Resulting variable.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto process_one_arg_function(
    const variable<Value, Diff>& arg, const Value& val,
    const Value& sensitivity) -> variable<Value, Diff> {
    if (arg.has_diff()) {
        return variable<Value, Diff>(val, sensitivity * arg.diff());
    }
    return variable<Value, Diff>(val);
}

}  // namespace impl

/*!
 * \brief Calculate exponential.
 *
 * \tparam Value Type of values.
 * \tparam Diff Type of differential coefficients.
 * \param[in] x Argument.
 * \return Value.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto exp(const variable<Value, Diff>& x)
    -> variable<Value, Diff> {
    using std::exp;
    const auto val = exp(x.value());
    return impl::process_one_arg_function(x, val, val);
}

/*!
 * \brief Calculate logarithm.
 *
 * \tparam Value Type of values.
 * \tparam Diff Type of differential coefficients.
 * \param[in] x Argument.
 * \return Value.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto log(const variable<Value, Diff>& x)
    -> variable<Value, Diff> {
    using std::log;
    return impl::process_one_arg_function(
        x, log(x.value()), static_cast<Value>(1) / x.value());
}

/*!
 * \brief Calculate square root.
 *
 * \tparam Value Type of values.
 * \tparam Diff Type of differential coefficients.
 * \param[in] x Argument.
 * \return Value.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto sqrt(const variable<Value, Diff>& x)
    -> variable<Value, Diff> {
    using std::sqrt;
    const auto val = sqrt(x.value());
    const auto half = static_cast<Value>(0.5);
    return impl::process_one_arg_function(x, val, half / val);
}

}  // namespace num_collect::auto_diff::forward
