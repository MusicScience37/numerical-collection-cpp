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
 * \brief Definition of make_jacobian function.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

#include <Eigen/Core>

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/util/assert.h"

namespace num_collect::auto_diff::forward {

namespace impl {

/*!
 * \brief Class of functor to make Jacobian.
 *
 * \tparam VectorType Type of vectors of variables.
 */
template <base::concepts::dense_vector VectorType>
class make_jacobian_functor {
public:
    //! Type of vectors of variables.
    using variable_vector_type = VectorType;

    //! Type of variables.
    using variable_type = typename variable_vector_type::Scalar;

    //! Type of values.
    using value_type = typename variable_type::value_type;

    //! Type of differential coefficients.
    using diff_type = typename variable_type::diff_type;

    static_assert(std::is_same_v<typename diff_type::Scalar, value_type>);

    //! Type of resulting matrix.
    using result_type = Eigen::Matrix<value_type,
        variable_vector_type::RowsAtCompileTime, diff_type::RowsAtCompileTime,
        Eigen::ColMajor, variable_vector_type::MaxRowsAtCompileTime,
        diff_type::MaxRowsAtCompileTime>;

    /*!
     * \brief Constructor.
     *
     * \param[in] vector Vector of variables.
     */
    explicit make_jacobian_functor(const variable_vector_type& vector)
        : vector_(vector) {}

    /*!
     * \brief Get an element of Jacobian.
     *
     * \param[in] row Row index.
     * \param[in] col Column index.
     * \return Element.
     */
    [[nodiscard]] auto operator()(index_type row, index_type col) const
        -> value_type {
        return vector_(row).diff()(col);
    }

private:
    //! Vector of variables.
    const variable_vector_type& vector_;
};

}  // namespace impl

/*!
 * \brief Make Jacobian from a vector of variables.
 *
 * \tparam VectorType Type of vectors of variables.
 * \param[in] vector Vector of variables.
 * \return Jacobian.
 */
template <base::concepts::dense_vector VectorType>
[[nodiscard]] inline auto make_jacobian(
    const Eigen::MatrixBase<VectorType>& vector)
    -> Eigen::CwiseNullaryOp<impl::make_jacobian_functor<VectorType>,
        typename impl::make_jacobian_functor<VectorType>::result_type> {
    using result_type =
        typename impl::make_jacobian_functor<VectorType>::result_type;

    if (vector.cols() != 1) {
        NUM_COLLECT_LOG_AND_THROW(invalid_argument,
            "differentiate function requires a vector as the argument.");
    }
    if (vector.rows() < 2) {
        NUM_COLLECT_LOG_AND_THROW(invalid_argument,
            "differentiate function requires a vector with at least two "
            "elements.");
    }
    for (index_type i = 0; i < vector.rows(); ++i) {
        NUM_COLLECT_ASSERT(vector(i).diff().cols() == 1);
    }
    const auto result_rows = vector.rows();
    const auto result_cols = vector(0).diff().rows();

    return result_type::NullaryExpr(result_rows, result_cols,
        impl::make_jacobian_functor<VectorType>(vector.derived()));
}

}  // namespace num_collect::auto_diff::forward
