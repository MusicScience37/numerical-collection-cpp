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

#include <array>

#include <Eigen/Core>

#include "num_collect/impl/num_collect_export.h"

namespace num_collect::ode::runge_kutta::impl {

/*!
 * \brief Decompose the coefficients of intermediate slopes.
 *
 * \param[in] num_stages Number of stages.
 * \param[in] slope_coeffs Coefficients of intermediate slopes.
 * \param[out] block_diagonal_matrix_coeffs Coefficients of block-diagonal
 * matrix in eigenvalue decomposition.
 * \param[out] eigenvectors_coeffs Coefficients of eigenvectors in eigenvalue
 * decomposition.
 * \param[out] eigenvectors_inverse_coeffs Coefficients of inverse of
 * eigenvectors in eigenvalue decomposition.
 *
 * Each matrix is stored in column-major order as in Eigen.
 * Size of each matrix is num_stages x num_stages.
 *
 * \note This function was extracted into a pre-built library to avoid excessive
 * memory usage during compilation. Without this, compilers use 2.5 GB of memory
 * per compilation unit, and it caused PC to freeze every time when compiling.
 */
NUM_COLLECT_EXPORT void decompose_slope_coeffs_impl(int num_stages,
    const long double* slope_coeffs, long double* block_diagonal_matrix_coeffs,
    long double* eigenvectors_coeffs, long double* eigenvectors_inverse_coeffs);

/*!
 * \brief Decompose the coefficients of intermediate slopes.
 *
 * \param[in] slope_coeffs Coefficients of intermediate slopes.
 * \param[out] block_diagonal_matrix Block-diagonal matrix in eigenvalue
 * decomposition.
 * \param[out] eigenvectors Eigenvectors in eigenvalue decomposition.
 * \param[out] eigenvectors_inverse Inverse of eigenvectors in eigenvalue
 * decomposition.
 */
template <std::floating_point Scalar, int NumStages>
    requires(NumStages > 0)
void decompose_slope_coeffs(
    const Eigen::Matrix<Scalar, NumStages, NumStages>& slope_coeffs,
    Eigen::Matrix<Scalar, NumStages, NumStages>& block_diagonal_matrix,
    Eigen::Matrix<Scalar, NumStages, NumStages>& eigenvectors,
    Eigen::Matrix<Scalar, NumStages, NumStages>& eigenvectors_inverse) {
    constexpr std::size_t buffer_size = static_cast<std::size_t>(NumStages) *
        static_cast<std::size_t>(NumStages);
    // Initialization is not needed here.
    std::array<long double, buffer_size> slope_coeffs_vec;           // NOLINT
    std::array<long double, buffer_size> block_diagonal_matrix_vec;  // NOLINT
    std::array<long double, buffer_size> eigenvectors_vec;           // NOLINT
    std::array<long double, buffer_size> eigenvectors_inverse_vec;   // NOLINT

    using mapped_matrix_type =
        Eigen::Matrix<long double, NumStages, NumStages, Eigen::ColMajor>;
    Eigen::Map<mapped_matrix_type>(slope_coeffs_vec.data(), NumStages,
        NumStages) = slope_coeffs.template cast<long double>();

    decompose_slope_coeffs_impl(NumStages, slope_coeffs_vec.data(),
        block_diagonal_matrix_vec.data(), eigenvectors_vec.data(),
        eigenvectors_inverse_vec.data());

    block_diagonal_matrix =
        Eigen::Map<const mapped_matrix_type>(block_diagonal_matrix_vec.data())
            .template cast<Scalar>();
    eigenvectors = Eigen::Map<const mapped_matrix_type>(eigenvectors_vec.data())
                       .template cast<Scalar>();
    eigenvectors_inverse =
        Eigen::Map<const mapped_matrix_type>(eigenvectors_inverse_vec.data())
            .template cast<Scalar>();
}

}  // namespace num_collect::ode::runge_kutta::impl
