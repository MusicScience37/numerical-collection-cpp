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

#include "num_collect/base/concepts/dense_matrix_of.h"
#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/concepts/implicitly_convertible_to.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/polynomial_term_generator.h"
#include "num_collect/util/vector_view.h"

namespace num_collect::rbf {

/*!
 * \brief Compute a matrix of polynomial terms in RBF interpolation.
 *
 * \tparam Variable Type of variables.
 * \tparam Matrix Type of the matrix.
 * \param[in] variables Variables.
 * \param[out] matrix Matrix.
 * \param[in] generator Generator for polynomial terms.
 */
template <base::concepts::real_scalar Variable,
    base::concepts::dense_matrix_of<Variable> Matrix>
inline void compute_polynomial_term_matrix(
    util::vector_view<const Variable> variables, Matrix& matrix,
    const polynomial_term_generator<1>& generator) {
    matrix.resize(variables.size(), generator.terms().size());
    for (index_type j = 0; j < generator.terms().size(); ++j) {
        for (index_type i = 0; i < variables.size(); ++i) {
            matrix(i, j) = generator.terms()[j](variables[i]);
        }
    }
}

/*!
 * \brief Compute a matrix of polynomial terms in RBF interpolation.
 *
 * \tparam Variable Type of variables.
 * \tparam Matrix Type of the matrix.
 * \param[in] variables Variables.
 * \param[out] matrix Matrix.
 * \param[in] generator Generator for polynomial terms.
 */
template <base::concepts::dense_vector Variable,
    base::concepts::dense_matrix_of<typename Variable::Scalar> Matrix>
    requires(Variable::RowsAtCompileTime > 0)
inline void compute_polynomial_term_matrix(
    util::vector_view<const Variable> variables, Matrix& matrix,
    const polynomial_term_generator<Variable::RowsAtCompileTime>& generator) {
    matrix.resize(variables.size(), generator.terms().size());
    for (index_type j = 0; j < generator.terms().size(); ++j) {
        for (index_type i = 0; i < variables.size(); ++i) {
            matrix(i, j) = generator.terms()[j](variables[i]);
        }
    }
}

/*!
 * \brief Compute a matrix of polynomial terms in RBF interpolation.
 *
 * \tparam Variable Type of variables.
 * \tparam Matrix Type of the matrix.
 * \param[in] variables Variables.
 * \param[out] matrix Matrix.
 * \param[in] generator Generator for polynomial terms.
 */
template <typename VariableVector, typename Matrix,
    typename PolynomialTermGenerator>
    requires base::concepts::implicitly_convertible_to<VariableVector,
        util::vector_view<
            const std::decay_t<decltype(*VariableVector{}.data())>>>
inline void compute_polynomial_term_matrix(VariableVector&& variables,
    Matrix& matrix, const PolynomialTermGenerator& generator) {
    using variable_type = std::decay_t<decltype(*variables.data())>;
    compute_polynomial_term_matrix<variable_type>(variables, matrix, generator);
}

}  // namespace num_collect::rbf
