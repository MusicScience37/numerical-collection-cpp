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

#include <vector>

#include "num_collect/base/concepts/dense_matrix_of.h"
#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/rbf/polynomial_calculator.h"

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
    polynomial_calculator<Variable, PolynomialDegree> calculator;
    calculator.prepare(1);
    calculator.compute_polynomial_term_matrix(variables, matrix);
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
    // TODO support of more degrees.
    static_assert(
        PolynomialDegree < 2, "Currently, up to 1 degree is supported.");

    using scalar_type = typename Matrix::Scalar;

    const auto num_variables = static_cast<index_type>(variables.size());
    NUM_COLLECT_PRECONDITION(num_variables > 0, "Variables must be given.");
    const auto num_dimensions = variables.front().size();

    polynomial_calculator<Variable, PolynomialDegree> calculator;
    calculator.prepare(num_dimensions);
    calculator.compute_polynomial_term_matrix(variables, matrix);
}

}  // namespace num_collect::rbf
