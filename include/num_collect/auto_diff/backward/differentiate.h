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
 * \brief Definition of differentiate function.
 */
#pragma once

#include <Eigen/Core>
#include <type_traits>

#include "num_collect/auto_diff/backward/graph/node.h"
#include "num_collect/auto_diff/backward/graph/node_differentiator.h"
#include "num_collect/auto_diff/backward/variable.h"

namespace num_collect::auto_diff::backward {

/*!
 * \brief Compute a differential coefficient.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] func_value Variable of the function value.
 * \param[in] arg Variable of the argument.
 * \return Differential coefficient.
 */
template <typename Scalar>
[[nodiscard]] inline auto differentiate(
    const variable<Scalar>& func_value, const variable<Scalar>& arg) -> Scalar {
    graph::node_differentiator<Scalar> diff;
    diff.compute(func_value.node());
    return diff.coeff(arg.node());
}

/*!
 * \brief Compute differential coefficients.
 *
 * \tparam Scalar Type of scalars.
 * \tparam ArgDerived Derived class of argument.
 * \tparam ResDerived Derived class of the result.
 * \param[in] func_value Variable of the function value.
 * \param[in] arg Variable of the argument.
 * \param[out] result Differential coefficients.
 */
template <typename Scalar, typename ArgDerived, typename ResDerived>
inline void differentiate(const variable<Scalar>& func_value,
    const Eigen::MatrixBase<ArgDerived>& arg,
    Eigen::MatrixBase<ResDerived>& result) {
    static_assert(
        std::is_same_v<typename ArgDerived::Scalar, variable<Scalar>>);
    static_assert(std::is_same_v<typename ResDerived::Scalar, Scalar>);

    graph::node_differentiator<Scalar> diff;
    diff.compute(func_value.node());
    result.derived().resize(arg.rows(), arg.cols());
    for (Eigen::Index i = 0; i < arg.rows(); ++i) {
        for (Eigen::Index j = 0; j < arg.cols(); ++j) {
            result(i, j) = diff.coeff(arg(i, j).node());
        }
    }
}

/*!
 * \brief Compute differential coefficients.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Rows Number of rows at compile time.
 * \tparam Cols Number of columns at compile time.
 * \tparam Options Options in Eigen::Matrix.
 * \tparam MaxRows Maximum number of rows.
 * \tparam MaxCols Maximum number of columns.
 * \param[in] func_value Variable of the function value.
 * \param[in] arg Variable of the argument.
 * \return Differential coefficients.
 *
 * \todo Use NullaryExpr as in
 * https://eigen.tuxfamily.org/dox/TopicCustomizing_NullaryExpr.html
 * for more performance.
 */
template <typename Scalar, int Rows, int Cols, int Options, int MaxRows,
    int MaxCols>
[[nodiscard]] inline auto differentiate(const variable<Scalar>& func_value,
    const Eigen::Matrix<variable<Scalar>, Rows, Cols, Options, MaxRows,
        MaxCols>& arg)
    -> Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols> {
    Eigen::Matrix<Scalar, Rows, Cols, Options, MaxRows, MaxCols> res;
    differentiate(func_value, arg, res);
    return res;
}

}  // namespace num_collect::auto_diff::backward
