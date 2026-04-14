/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of compute_polynomial_zeros function.
 */
#pragma once

#include <complex>
#include <concepts>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>

#include "num_collect/base/precondition.h"
#include "num_collect/polynomials/polynomial.h"
#include "num_collect/util/vector.h"

namespace num_collect::polynomials {

/*!
 * \brief Compute zeros of a polynomial.
 *
 * \tparam Coeff Type of coefficients.
 * \param[in] poly Polynomial to compute zeros.
 * \return Zeros of the polynomial.
 */
template <std::floating_point Coeff>
[[nodiscard]] auto compute_zeros(const polynomial<Coeff>& poly)
    -> util::vector<std::complex<Coeff>> {
    NUM_COLLECT_PRECONDITION(
        poly.degree() >= 1, "Polynomial must have degree at least 1.");

    const auto& coeffs = poly.coeffs();
    Eigen::MatrixX<Coeff> matrix =
        Eigen::MatrixX<Coeff>::Zero(coeffs.size() - 1, coeffs.size() - 1);
    for (num_collect::index_type i = 0; i < coeffs.size() - 1; ++i) {
        matrix(0, coeffs.size() - 2 - i) = -coeffs[i] / coeffs.back();
    }
    for (num_collect::index_type i = 1; i < coeffs.size() - 1; ++i) {
        matrix(i, i - 1) = 1.0;
    }
    Eigen::EigenSolver<Eigen::MatrixX<Coeff>> solver(matrix);

    util::vector<std::complex<Coeff>> zeros(solver.eigenvalues().size());
    Eigen::Map<Eigen::VectorX<std::complex<Coeff>>> zeros_map(
        zeros.data(), zeros.size());
    zeros_map = solver.eigenvalues();
    return zeros;
}

}  // namespace num_collect::polynomials
