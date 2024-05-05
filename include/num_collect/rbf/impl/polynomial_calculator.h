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

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar.h"  // IWYU pragma: keep
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"

namespace num_collect::rbf::impl {

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
     * \brief Evaluate a polynomial for a variable.
     *
     * \param[in] variable Variable.
     * \param[in] coeffs Coefficients of the polynomial.
     * \return Value.
     */
    [[nodiscard]] auto evaluate_polynomial_for_variable(Variable variable,
        const Eigen::VectorX<Variable>& coeffs) const -> Variable {
        if (coeffs.size() != PolynomialDegree + 1) {
            throw invalid_argument("Invalid size of coefficients.");
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

}  // namespace num_collect::rbf::impl
