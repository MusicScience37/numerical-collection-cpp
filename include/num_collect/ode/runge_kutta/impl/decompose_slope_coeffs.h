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
 * \brief Declaration of decompose_slope_coeffs function.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/impl/num_collect_export.h"

namespace num_collect::ode::runge_kutta::impl {

/*!
 * \brief Decompose the coefficients of intermidiate slopes.
 *
 * \param[in] slope_coeffs Coefficients of intermidiate slopes.
 * \param[in] block_diagonal_matrix Block-diagonal matrix in eigenvalue
 * decomposition.
 * \param[in] eigenvectors Eigenvectors in eigenvalue decomposition.
 * \param[in] eigenvectors_inverse Inverse of eigenvectors in eigenvalue
 * decomposition.
 *
 * \note This function was separated to a pre-built library to avoid too much
 * memory usage in compilation. Without this, compilers use 2.5 GB memory per a
 * compilation unit, and it caused PC to freeze every time when compiling.
 */
NUM_COLLECT_EXPORT void decompose_slope_coeffs_impl(
    const Eigen::MatrixX<long double>& slope_coeffs,
    Eigen::MatrixX<long double>& block_diagonal_matrix,
    Eigen::MatrixX<long double>& eigenvectors,
    Eigen::MatrixX<long double>& eigenvectors_inverse);

/*!
 * \brief Decompose the coefficients of intermidiate slopes.
 *
 * \param[in] slope_coeffs Coefficients of intermidiate slopes.
 * \param[in] block_diagonal_matrix Block-diagonal matrix in eigenvalue
 * decomposition.
 * \param[in] eigenvectors Eigenvectors in eigenvalue decomposition.
 * \param[in] eigenvectors_inverse Inverse of eigenvectors in eigenvalue
 * decomposition.
 */
template <std::floating_point Scalar, int NumStages>
void decompose_slope_coeffs(
    const Eigen::Matrix<Scalar, NumStages, NumStages>& slope_coeffs,
    Eigen::Matrix<Scalar, NumStages, NumStages>& block_diagonal_matrix,
    Eigen::Matrix<Scalar, NumStages, NumStages>& eigenvectors,
    Eigen::Matrix<Scalar, NumStages, NumStages>& eigenvectors_inverse) {
    Eigen::MatrixX<long double> slope_coeffs_long_double =
        slope_coeffs.template cast<long double>();
    Eigen::MatrixX<long double> block_diagonal_matrix_long_double;
    Eigen::MatrixX<long double> eigenvectors_long_double;
    Eigen::MatrixX<long double> eigenvectors_inverse_long_double;

    decompose_slope_coeffs_impl(slope_coeffs_long_double,
        block_diagonal_matrix_long_double, eigenvectors_long_double,
        eigenvectors_inverse_long_double);

    block_diagonal_matrix =
        block_diagonal_matrix_long_double.template cast<Scalar>();
    eigenvectors = eigenvectors_long_double.template cast<Scalar>();
    eigenvectors_inverse =
        eigenvectors_inverse_long_double.template cast<Scalar>();
}

}  // namespace num_collect::ode::runge_kutta::impl
