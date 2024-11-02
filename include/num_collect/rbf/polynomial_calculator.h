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
 * \brief Definition of polynomial_calculator class.
 */
#pragma once

#include <cmath>
#include <cstddef>
#include <vector>

#include <Eigen/Core>

#include "num_collect/base/concepts/dense_matrix_of.h"
#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logging_macros.h"

namespace num_collect::rbf {

/*!
 * \brief Class to calculate polynomials used with RBF interpolation.
 *
 * \tparam Variable Type of variables.
 * \tparam PolynomialDegree Degree of polynomials.
 */
template <typename Variable, index_type PolynomialDegree>
class polynomial_calculator;

/*!
 * \brief Class to calculate polynomials used with RBF interpolation.
 *
 * \tparam Variable Type of variables.
 * \tparam PolynomialDegree Degree of polynomials.
 */
template <base::concepts::real_scalar Variable, index_type PolynomialDegree>
    requires(PolynomialDegree >= 0)
class polynomial_calculator<Variable, PolynomialDegree> {
public:
    /*!
     * \brief Prepare internal parameters.
     *
     * \param[in] num_dimensions Dimensions of variables.
     */
    void prepare(index_type num_dimensions) {
        // No operation.
        (void)num_dimensions;
    }

    /*!
     * \brief Compute a matrix of polynomial terms.
     *
     * \tparam Matrix Type of the matrix.
     * \param[in] variables Variables.
     * \param[out] matrix Matrix.
     */
    template <base::concepts::dense_matrix_of<Variable> Matrix>
    void compute_polynomial_term_matrix(
        const std::vector<Variable>& variables, Matrix& matrix) const {
        const auto num_variables = static_cast<index_type>(variables.size());
        if (num_variables < PolynomialDegree + 2) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "At least (PolynomialDegree + 2) variables must be given.");
        }

        matrix.resize(num_variables, PolynomialDegree + 1);
        {
            // degree = 0 (constant)
            matrix.col(0) = Eigen::MatrixX<Variable>::Constant(
                num_variables, 1, static_cast<Variable>(1));
        }
        for (index_type degree = 1; degree <= PolynomialDegree; ++degree) {
            for (index_type i = 0; i < num_variables; ++i) {
                using std::pow;
                matrix(i, degree) = static_cast<Variable>(
                    pow(variables[static_cast<std::size_t>(i)], degree));
            }
        }
    }

    /*!
     * \brief Evaluate a polynomial for a variable.
     *
     * \param[in] variable Variable.
     * \param[in] coeffs Coefficients of the polynomial.
     * \return Value.
     */
    [[nodiscard]] auto evaluate_polynomial_for_variable(Variable variable,
        const Eigen::VectorX<Variable>& coeffs) const -> Variable {
        if (coeffs.size() != PolynomialDegree + 1) {
            NUM_COLLECT_LOG_AND_THROW(
                invalid_argument, "Invalid size of coefficients.");
        }

        // degree = 0 (constant)
        auto value = coeffs(0);

        for (index_type degree = 1; degree <= PolynomialDegree; ++degree) {
            using std::pow;
            value +=
                coeffs(degree) * static_cast<Variable>(pow(variable, degree));
        }

        return value;
    }
};

/*!
 * \brief Class to calculate polynomials used with RBF interpolation.
 *
 * \tparam Variable Type of variables.
 * \tparam PolynomialDegree Degree of polynomials.
 */
template <base::concepts::dense_vector Variable, index_type PolynomialDegree>
    requires(PolynomialDegree >= 0)
class polynomial_calculator<Variable, PolynomialDegree> {
public:
    //! Type of scalars.
    using scalar_type = typename Variable::Scalar;

    /*!
     * \brief Prepare internal parameters.
     *
     * \param[in] num_dimensions Dimensions of variables.
     */
    void prepare(index_type num_dimensions) {
        static_assert(
            PolynomialDegree < 2, "Currently, up to 1 degree is supported.");
        index_type num_patterns = 1;
        if constexpr (PolynomialDegree == 1) {
            num_patterns += num_dimensions;
        }

        degrees_.resize(num_patterns, num_dimensions);
        {
            // degree = 0 (constant)
            for (index_type d = 0; d < num_dimensions; ++d) {
                degrees_(0, d) = 0;
            }
        }
        if constexpr (PolynomialDegree >= 1) {
            // degree = 1
            degrees_.bottomRows(num_dimensions) =
                Eigen::MatrixX<index_type>::Identity(
                    num_dimensions, num_dimensions);
        }
    }

    /*!
     * \brief Compute a matrix of polynomial terms.
     *
     * \tparam Matrix Type of the matrix.
     * \param[in] variables Variables.
     * \param[out] matrix Matrix.
     */
    template <base::concepts::dense_matrix_of<scalar_type> Matrix>
    void compute_polynomial_term_matrix(
        const std::vector<Variable>& variables, Matrix& matrix) const {
        const auto num_variables = static_cast<index_type>(variables.size());
        if (num_variables == 0) {
            NUM_COLLECT_LOG_AND_THROW(
                invalid_argument, "No variable is given.");
        }
        const index_type num_dimensions = variables.front().size();

        const index_type num_patterns = degrees_.rows();
        matrix.resize(num_variables, num_patterns);
        for (index_type i = 0; i < num_variables; ++i) {
            const auto& variable = variables[static_cast<std::size_t>(i)];
            for (index_type p = 0; p < num_patterns; ++p) {
                auto value = static_cast<scalar_type>(1);
                for (index_type d = 0; d < num_dimensions; ++d) {
                    if (degrees_(p, d) > 0) {
                        using std::pow;
                        value *= pow(variable(d), degrees_(p, d));
                    }
                }
                matrix(i, p) = value;
            }
        }
    }

    /*!
     * \brief Evaluate a polynomial for a variable.
     *
     * \param[in] variable Variable.
     * \param[in] coeffs Coefficients of the polynomial.
     * \return Value.
     */
    [[nodiscard]] auto evaluate_polynomial_for_variable(Variable variable,
        const Eigen::VectorX<scalar_type>& coeffs) const -> scalar_type {
        const index_type num_dimensions = variable.size();

        const index_type num_patterns = degrees_.rows();
        if (coeffs.size() != num_patterns) {
            NUM_COLLECT_LOG_AND_THROW(
                invalid_argument, "Invalid size of coefficients.");
        }

        auto value = static_cast<scalar_type>(0);
        for (index_type p = 0; p < num_patterns; ++p) {
            scalar_type current_term = coeffs(p);
            for (index_type d = 0; d < num_dimensions; ++d) {
                if (degrees_(p, d) > 0) {
                    using std::pow;
                    current_term *= pow(variable(d), degrees_(p, d));
                }
            }
            value += current_term;
        }

        return value;
    }

private:
    //! Degrees of elements of variables. (Rows: patterns, Cols: dimension.)
    Eigen::Matrix<index_type, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
        degrees_{};
};

}  // namespace num_collect::rbf
