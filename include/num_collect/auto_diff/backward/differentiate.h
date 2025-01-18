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

#include <cstddef>
#include <memory>
#include <utility>  // IWYU pragma: keep
#include <vector>

#include <Eigen/Core>

#include "num_collect/auto_diff/backward/graph/node_differentiator.h"
#include "num_collect/auto_diff/backward/variable.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/util/safe_cast.h"

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

namespace impl {

/*!
 * \brief Class of functor to assign differential coefficients to matrices
 *        on the condition that a scalar-valued function is differentiated
 *        by a matrix-valued variable.
 *
 * \tparam ArgType Type of function argument.
 */
template <typename ArgType>
class differentiate_scalar_with_matrix_functor {
public:
    //! Type of variables.
    using variable_type = typename ArgType::Scalar;

    //! Type of scalars.
    using scalar_type = typename variable_type::scalar_type;

    //! Type of resulting differential coefficients.
    using result_type = Eigen::Matrix<scalar_type, ArgType::RowsAtCompileTime,
        ArgType::ColsAtCompileTime, Eigen::ColMajor,
        ArgType::MaxRowsAtCompileTime, ArgType::MaxColsAtCompileTime>;

    /*!
     * \brief Constructor.
     *
     * \param[in] arg Matrix of variable.
     * \param[in] diff Differentiator.
     */
    differentiate_scalar_with_matrix_functor(const ArgType& arg,
        std::shared_ptr<const graph::node_differentiator<scalar_type>> diff)
        : arg_(arg), diff_(std::move(diff)) {}

    /*!
     * \brief Get an element of differential coefficients.
     *
     * \param[in] row Row index.
     * \param[in] col Column index.
     * \return Differential coefficient.
     */
    auto operator()(index_type row, index_type col) const -> scalar_type {
        return diff_->coeff(arg_(row, col).node());
    }

private:
    //! Matrix of variable.
    const ArgType& arg_;

    //! Differentiator.
    std::shared_ptr<const graph::node_differentiator<scalar_type>> diff_;
};

}  // namespace impl

/*!
 * \brief Compute differential coefficients.
 *
 * \tparam Scalar Type of scalars.
 * \tparam ArgDerived Derived class of argument.
 * \param[in] func_value Variable of the function value.
 * \param[in] arg Variable of the argument.
 * \return Expression of differential coefficients.
 */
template <typename Scalar, typename ArgDerived>
[[nodiscard]] inline auto differentiate(const variable<Scalar>& func_value,
    const Eigen::MatrixBase<ArgDerived>& arg)
    -> Eigen::CwiseNullaryOp<
        impl::differentiate_scalar_with_matrix_functor<ArgDerived>,
        typename impl::differentiate_scalar_with_matrix_functor<
            ArgDerived>::result_type> {
    static_assert(
        std::is_same_v<typename ArgDerived::Scalar, variable<Scalar>>);

    auto diff = std::make_shared<graph::node_differentiator<Scalar>>();
    diff->compute(func_value.node());

    using result_type = typename impl::differentiate_scalar_with_matrix_functor<
        ArgDerived>::result_type;
    return result_type::NullaryExpr(arg.rows(), arg.cols(),
        impl::differentiate_scalar_with_matrix_functor<ArgDerived>(
            arg.derived(), std::move(diff)));
}

namespace impl {

/*!
 * \brief Class of functor to assign differential coefficients to matrices
 *        on the condition that a vector-valued function is differentiated
 *        by a vector-valued variable.
 *
 * \tparam FuncValType Type of function values.
 * \tparam ArgType Type of function arguments.
 */
template <typename FuncValType, typename ArgType>
class differentiate_vector_with_vector_functor {
public:
    //! Type of variables.
    using variable_type = typename FuncValType::Scalar;

    static_assert(std::is_same_v<typename ArgType::Scalar, variable_type>);

    //! Type of scalars.
    using scalar_type = typename variable_type::scalar_type;

    //! Type of resulting differential coefficients.
    using result_type =
        Eigen::Matrix<scalar_type, FuncValType::RowsAtCompileTime,
            ArgType::RowsAtCompileTime, Eigen::ColMajor,
            FuncValType::MaxRowsAtCompileTime, ArgType::MaxRowsAtCompileTime>;

    /*!
     * \brief Constructor.
     *
     * \param[in] arg Matrix of variable.
     * \param[in] diff Differentiators.
     */
    differentiate_vector_with_vector_functor(const ArgType& arg,
        std::shared_ptr<std::vector<graph::node_differentiator<scalar_type>>>
            diff)
        : arg_(arg), diff_(std::move(diff)) {}

    /*!
     * \brief Get an element of differential coefficients.
     *
     * \param[in] row Row index.
     * \param[in] col Column index.
     * \return Differential coefficient.
     */
    auto operator()(index_type row, index_type col) const -> scalar_type {
        return diff_->at(util::safe_cast<std::size_t>(row))
            .coeff(arg_(col, 0).node());
    }

private:
    //! Matrix of variable.
    const ArgType& arg_;

    //! Differentiators.
    std::shared_ptr<std::vector<graph::node_differentiator<scalar_type>>> diff_;
};

}  // namespace impl

/*!
 * \brief Compute differential coefficients.
 *
 * \tparam FuncValType Type of function values.
 * \tparam ArgType Type of function arguments.
 * \param[in] func_value Variable of the function value.
 * \param[in] arg Variable of the argument.
 * \return Expression of differential coefficients.
 */
template <typename FuncValType, typename ArgType>
[[nodiscard]] inline auto differentiate(
    const Eigen::MatrixBase<FuncValType>& func_value,
    const Eigen::MatrixBase<ArgType>& arg)
    -> Eigen::CwiseNullaryOp<
        impl::differentiate_vector_with_vector_functor<FuncValType, ArgType>,
        typename impl::differentiate_vector_with_vector_functor<FuncValType,
            ArgType>::result_type> {
    using variable_type = typename FuncValType::Scalar;
    using scalar_type = typename variable_type::scalar_type;

    NUM_COLLECT_PRECONDITION(func_value.cols() == 1 && arg.cols() == 1,
        "differentiate function requires vectors as arguments.");

    auto diff = std::make_shared<
        std::vector<graph::node_differentiator<scalar_type>>>();
    diff->resize(util::safe_cast<std::size_t>(func_value.size()));
    for (index_type i = 0; i < func_value.size(); ++i) {
        diff->operator[](util::safe_cast<std::size_t>(i))
            .compute(func_value(i, 0).node());
    }

    using result_type =
        typename impl::differentiate_vector_with_vector_functor<FuncValType,
            ArgType>::result_type;
    return result_type::NullaryExpr(func_value.size(), arg.size(),
        impl::differentiate_vector_with_vector_functor<FuncValType, ArgType>(
            arg.derived(), std::move(diff)));
}

}  // namespace num_collect::auto_diff::backward
