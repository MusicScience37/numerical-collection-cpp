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
 * \brief Definition of format_sparse_matrix function.
 */
#pragma once

#include <cstdint>

#include <Eigen/Core>
#include <Eigen/SparseCore>  // IWYU pragma: keep
#include <fmt/base.h>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/exception.h"
#include "num_collect/logging/logging_macros.h"

namespace num_collect::util {

/*!
 * \brief Enumeration of matrix format types.
 */
enum class sparse_matrix_format_type : std::uint8_t {
    //! One line.
    one_line,

    //! Multiple lines.
    multi_line
};

namespace impl {

/*!
 * \brief Class of expressions to format sparse matrices.
 *
 * \tparam Matrix Type of the matrix.
 */
template <base::concepts::sparse_matrix Matrix>
class sparse_matrix_format_view {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] mat Matrix.
     * \param[in] type Type.
     */
    sparse_matrix_format_view(const Matrix& mat, sparse_matrix_format_type type)
        : mat_(&mat), type_(type) {}

    /*!
     * \brief Get the matrix.
     *
     * \return Matrix.
     */
    [[nodiscard]] auto mat() const noexcept -> const Matrix& { return *mat_; }

    /*!
     * \brief Get the type.
     *
     * \return Type.
     */
    [[nodiscard]] auto type() const noexcept -> sparse_matrix_format_type {
        return type_;
    }

private:
    //! Matrix.
    const Matrix* mat_;

    //! Type.
    sparse_matrix_format_type type_;
};

}  // namespace impl

/*!
 * \brief Format a sparse matrix.
 *
 * \tparam Matrix Type of the matrix.
 * \param[in] mat Matrix.
 * \param[in] type Format type.
 * \return Expression for formatting using fmt library.
 *
 * \note Format of each element can be specified in fmt's format strings.
 */
template <base::concepts::sparse_matrix Matrix>
[[nodiscard]] inline auto format_sparse_matrix(const Matrix& mat,
    sparse_matrix_format_type type = sparse_matrix_format_type::one_line) {
    return impl::sparse_matrix_format_view<Matrix>(mat, type);
}

}  // namespace num_collect::util

namespace fmt {

/*!
 * \brief fmt::formatter for num_collect::util::impl::sparse_matrix_format_view.
 *
 * \tparam Matrix Type of the matrix.
 */
template <num_collect::base::concepts::sparse_matrix Matrix>
struct formatter<num_collect::util::impl::sparse_matrix_format_view<Matrix>>
    : public formatter<typename Matrix::Scalar> {
public:
    /*!
     * \brief Format a value.
     *
     * \param[in] val Value.
     * \param[in] context Context.
     * \return Output iterator after formatting.
     */
    auto format(
        const num_collect::util::impl::sparse_matrix_format_view<Matrix>& val,
        format_context& context) const -> decltype(context.out()) {
        const auto& mat = val.mat();
        switch (val.type()) {
        case num_collect::util::sparse_matrix_format_type::one_line:
            return format_one_line(mat, context);
        case num_collect::util::sparse_matrix_format_type::multi_line:
            return format_multi_line(mat, context);
        }
        NUM_COLLECT_LOG_AND_THROW(
            num_collect::invalid_argument, "Invalid format type.");
    }

private:
    /*!
     * \brief Format a matrix in one line.
     *
     * \param[in] mat Matrix.
     * \param[in] context Context.
     * \return Output iterator after formatting.
     */
    auto format_one_line(const Matrix& mat, format_context& context) const
        -> decltype(context.out()) {
        const Eigen::Index rows = mat.rows();
        const Eigen::Index cols = mat.cols();
        auto out = context.out();
        *out = '[';
        ++out;
        for (Eigen::Index row = 0; row < rows; ++row) {
            if (row != 0) {
                out = write_comma(out);
            }
            *out = '[';
            ++out;
            for (Eigen::Index col = 0; col < cols; ++col) {
                if (col != 0) {
                    out = write_comma(out);
                }
                context.advance_to(out);
                out = formatter<typename Matrix::Scalar>::format(
                    mat.coeff(row, col), context);
            }
            *out = ']';
            ++out;
        }
        *out = ']';
        ++out;
        return out;
    }

    /*!
     * \brief Format a matrix in multiple lines.
     *
     * \param[in] mat Matrix.
     * \param[in] context Context.
     * \return Output iterator after formatting.
     */
    auto format_multi_line(const Matrix& mat, format_context& context) const
        -> decltype(context.out()) {
        const Eigen::Index rows = mat.rows();
        const Eigen::Index cols = mat.cols();
        auto out = context.out();
        *out = '[';
        ++out;
        *out = '\n';
        ++out;
        for (Eigen::Index row = 0; row < rows; ++row) {
            if (row != 0) {
                *out = ',';
                ++out;
                *out = '\n';
                ++out;
            }
            *out = ' ';
            ++out;
            *out = ' ';
            ++out;
            *out = '[';
            ++out;
            for (Eigen::Index col = 0; col < cols; ++col) {
                if (col != 0) {
                    out = write_comma(out);
                }
                context.advance_to(out);
                out = formatter<typename Matrix::Scalar>::format(
                    mat.coeff(row, col), context);
            }
            *out = ']';
            ++out;
        }
        *out = '\n';
        ++out;
        *out = ']';
        ++out;
        return out;
    }

    /*!
     * \brief Write a comma.
     *
     * \param[in] out Output iterator.
     * \return Output iterator.
     */
    static auto write_comma(format_context::iterator out)
        -> format_context::iterator {
        *out = ',';
        ++out;
        *out = ' ';
        ++out;
        return out;
    }
};

}  // namespace fmt
