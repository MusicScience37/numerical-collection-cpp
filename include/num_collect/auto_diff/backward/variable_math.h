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
 * \brief Definition of mathematical functions for variable class.
 */
#pragma once

#include <cmath>

#include "num_collect/auto_diff/backward/graph/node.h"
#include "num_collect/auto_diff/backward/variable.h"

namespace num_collect::auto_diff::backward {

namespace impl {

/*!
 * \brief Process a function with one argument.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] arg Argument.
 * \param[in] val New value.
 * \param[in] sensitivity Partial differential coefficient.
 * \return Resulting variable.
 */
template <typename Scalar>
[[nodiscard]] inline auto process_one_arg_function(const variable<Scalar>& arg,
    const Scalar& val, const Scalar& sensitivity) -> variable<Scalar> {
    if (arg.node()) {
        return variable<Scalar>(
            val, graph::create_node<Scalar>(arg.node(), sensitivity));
    }
    return variable<Scalar>(val);
}

}  // namespace impl

/*!
 * \brief Calculate exponential.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] x Argument.
 * \return Value.
 */
template <typename Scalar>
[[nodiscard]] inline auto exp(const variable<Scalar>& x) -> variable<Scalar> {
    using std::exp;
    const auto val = exp(x.value());
    return impl::process_one_arg_function(x, val, val);
}

/*!
 * \brief Calculate logarithm.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] x Argument.
 * \return Value.
 */
template <typename Scalar>
[[nodiscard]] inline auto log(const variable<Scalar>& x) -> variable<Scalar> {
    using std::log;
    return impl::process_one_arg_function(
        x, log(x.value()), static_cast<Scalar>(1) / x.value());
}

/*!
 * \brief Calculate square root.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] x Argument.
 * \return Value.
 */
template <typename Scalar>
[[nodiscard]] inline auto sqrt(const variable<Scalar>& x) -> variable<Scalar> {
    using std::sqrt;
    const auto val = sqrt(x.value());
    const auto half = static_cast<Scalar>(0.5);
    return impl::process_one_arg_function(x, val, half / val);
}

}  // namespace num_collect::auto_diff::backward
