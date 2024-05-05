/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of compute_polynomial_term_matrix function.
 */
#pragma once

#include <cmath>
#include <cstddef>

#include <fmt/core.h>

#include "num_collect/base/concepts/dense_matrix_of.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/dense_vector.h"     // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar.h"      // IWYU pragma: keep
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"

namespace num_collect::rbf {

/*!
 * \brief Compute a matrix of polynomial terms in RBF interpolation.
 *
 * \tparam PolynomialDegree Degree of polynomials.
 * \tparam Variable Type of variables.
 * \tparam Matrix Type of the matrix.
 * \param[in] variables Variables.
 * \param[out] matrix Matrix.
 */
template <index_type PolynomialDegree, base::concepts::real_scalar Variable,
    base::concepts::dense_matrix_of<Variable> Matrix>
    requires(PolynomialDegree >= 0)
inline void compute_polynomial_term_matrix(
    const std::vector<Variable>& variables, Matrix& matrix) {
    using scalar_type = typename Matrix::Scalar;

    const auto num_variables = static_cast<index_type>(variables.size());
    if (num_variables < PolynomialDegree + 2) {
        throw invalid_argument(
            "At least (PolynomialDegree + 2) variables must be given.");
    }

    matrix.resize(num_variables, PolynomialDegree + 1);
    {
        // degree = 0 (constant)
        matrix.col(0) = Eigen::MatrixX<scalar_type>::Constant(
            num_variables, 1, static_cast<scalar_type>(1));
    }
    for (index_type degree = 1; degree <= PolynomialDegree; ++degree) {
        for (index_type i = 0; i < num_variables; ++i) {
            using std::pow;
            matrix(i, degree) = static_cast<scalar_type>(
                pow(variables[static_cast<std::size_t>(i)], degree));
        }
    }
}

/*!
 * \brief Compute a matrix of polynomial terms in RBF interpolation.
 *
 * \tparam PolynomialDegree Degree of polynomials.
 * \tparam Variable Type of variables.
 * \tparam Matrix Type of the matrix.
 * \param[in] variables Variables.
 * \param[out] matrix Matrix.
 */
template <index_type PolynomialDegree, base::concepts::dense_vector Variable,
    base::concepts::dense_matrix_of<typename Variable::Scalar> Matrix>
    requires(PolynomialDegree >= 0)
inline void compute_polynomial_term_matrix(
    const std::vector<Variable>& variables, Matrix& matrix) {
    static_assert(
        PolynomialDegree < 2, "Currently, up to 1 degree is supported.");

    using scalar_type = typename Matrix::Scalar;

    const auto num_variables = static_cast<index_type>(variables.size());
    if (num_variables == 0) {
        throw invalid_argument("No variable is given.");
    }
    const auto num_dimensions = variables.front().size();

    index_type num_polynomials = 1;
    if constexpr (PolynomialDegree == 0) {
        num_polynomials = 1;
    } else {
        num_polynomials = 1 + num_dimensions;
    }
    if (num_variables < num_polynomials + 1) {
        throw invalid_argument(fmt::format(
            "At least {} variables must be given.", num_polynomials + 1));
    }

    matrix.resize(num_variables, num_polynomials);
    {
        // degree = 0 (constant)
        matrix.col(0) = Eigen::MatrixX<scalar_type>::Constant(
            num_variables, 1, static_cast<scalar_type>(1));
    }
    if constexpr (PolynomialDegree >= 1) {
        // degree = 1
        for (index_type j = 0; j < num_dimensions; ++j) {
            for (index_type i = 0; i < num_variables; ++i) {
                matrix(i, j + 1) = variables[static_cast<std::size_t>(i)](j);
            }
        }
    }
}

}  // namespace num_collect::rbf
