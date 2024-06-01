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
 * \brief Definition of create_diff_variable function.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/auto_diff/backward/variable.h"
#include "num_collect/base/concepts/real_scalar.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar_dense_vector.h"  // IWYU pragma: keep
#include "num_collect/base/index_type.h"
#include "num_collect/util/assert.h"

namespace num_collect::auto_diff::backward {

/*!
 * \brief Create a variable by which functions will be differentiated.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] value Value.
 * \return Variable.
 */
template <base::concepts::real_scalar Scalar>
[[nodiscard]] inline auto create_diff_variable(
    const Scalar& value) -> variable<Scalar> {
    return variable<Scalar>(value, variable_tag());
}

/*!
 * \brief Get type of vectors of variables.
 *
 * \tparam ValueVector Type of vectors of values.
 */
template <base::concepts::real_scalar_dense_vector ValueVector>
using variable_vector_type =
    Eigen::Matrix<variable<typename ValueVector::Scalar>,
        ValueVector::RowsAtCompileTime, 1, Eigen::ColMajor,
        ValueVector::MaxRowsAtCompileTime, 1>;

namespace impl {

/*!
 * \brief Class of functor to create a vector of variables by which functions
 * will be differentiated.
 *
 * \tparam Value
 * \tparam Diff
 */
template <typename ValueVector>
class create_diff_variable_vector_functor {
public:
    //! Type of scalars.
    using scalar_type = typename ValueVector::Scalar;

    //! Type of variables.
    using variable_type = variable<scalar_type>;

    //! Type of resulting differential coefficients.
    using result_type =
        Eigen::Matrix<variable_type, ValueVector::RowsAtCompileTime, 1,
            Eigen::ColMajor, ValueVector::MaxRowsAtCompileTime, 1>;

    /*!
     * \brief Constructor.
     *
     * \param[in] value_vec Values of the vector.
     */
    explicit create_diff_variable_vector_functor(const ValueVector& value_vec)
        : value_vec_(value_vec) {}

    /*!
     * \brief Get an element of the vector.
     *
     * \param[in] row Row index.
     * \param[in] col Column index.
     * \return Element.
     */
    [[nodiscard]] auto operator()(
        index_type row, index_type col) const -> variable_type {
        NUM_COLLECT_DEBUG_ASSERT(col == 0);
        return create_diff_variable<scalar_type>(value_vec_(row, col));
    }

private:
    //! Values of the vector.
    const ValueVector& value_vec_;
};

}  // namespace impl

/*!
 * \brief Create a vector of variables.
 *
 * \tparam ValueVector Type of vectors of values.
 * \param[in] value_vec Vector of values.
 * \return Vector of variables.
 */
template <base::concepts::real_scalar_dense_vector ValueVector>
[[nodiscard]] inline auto create_diff_variable_vector(
    const Eigen::MatrixBase<ValueVector>& value_vec)
    -> Eigen::CwiseNullaryOp<
        impl::create_diff_variable_vector_functor<ValueVector>,
        typename impl::create_diff_variable_vector_functor<
            ValueVector>::result_type> {
    NUM_COLLECT_ASSERT(value_vec.cols() == 1);

    using result_type = typename impl::create_diff_variable_vector_functor<
        ValueVector>::result_type;
    return result_type::NullaryExpr(value_vec.rows(), 1,
        impl::create_diff_variable_vector_functor<ValueVector>(
            value_vec.derived()));
}

}  // namespace num_collect::auto_diff::backward
