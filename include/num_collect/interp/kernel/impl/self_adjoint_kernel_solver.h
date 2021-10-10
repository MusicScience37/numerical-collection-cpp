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
 * \brief Definition of self_adjoint_kernel_solver class.
 */
#pragma once

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <limits>
#include <type_traits>

#include "num_collect/util/assert.h"

namespace num_collect::interp::kernel::impl {

/*!
 * \brief Class to solve linear equations with self-adjoint kernel matrices.
 *
 * \tparam KernelMatrix Type of kernel matrices.
 * \tparam Data Type of data vectors.
 */
template <typename KernelMatrix, typename Data>
class self_adjoint_kernel_solver {
public:
    //! Type of kernel matrices.
    using kernel_mat_type = KernelMatrix;

    //! Type of data vectors.
    using data_type = Data;

    static_assert(std::is_same_v<typename kernel_mat_type::Scalar,
        typename data_type::Scalar>);

    //! Type of scalars.
    using scalar_type = typename kernel_mat_type::Scalar;

    self_adjoint_kernel_solver() = default;

    /*!
     * \brief Compute internal matrices.
     *
     * \tparam InputKernel Type of the input kernel matrix.
     * \tparam InputData Type of the input data vector.
     * \param[in] kernel_mat Input kernel matrix.
     * \param[in] data Input data vector.
     */
    template <typename InputKernel, typename InputData>
    void compute(const Eigen::MatrixBase<InputKernel>& kernel_mat,
        const Eigen::MatrixBase<InputData>& data) {
        kernel_eigen_.compute(kernel_mat, Eigen::ComputeEigenvectors);
        spectre_ = kernel_eigen_.eigenvectors().adjoint() * data;
    }

    /*!
     * \brief Solve for a regularization parameter.
     *
     * \tparam OutputType Type of the output vector.
     * \param[in] reg_param Regularization parameter.
     * \param[out] solution Solution.
     */
    template <typename OutputType>
    void solve(const scalar_type& reg_param,
        Eigen::MatrixBase<OutputType>& solution) const {
        solution = kernel_eigen_.eigenvectors() *
            (kernel_eigen_.eigenvalues().array() + reg_param)
                .inverse()
                .matrix()
                .asDiagonal() *
            spectre_;
    }

    /*!
     * \brief Get eigenvalues.
     *
     * \return Eigenvalues.
     */
    [[nodiscard]] auto eigenvalues() const -> const
        typename Eigen::SelfAdjointEigenSolver<
            kernel_mat_type>::RealVectorType& {
        return kernel_eigen_.eigenvalues();
    }

    /*!
     * \brief Calculate maximum likelihood estimation (MLE) objective function.
     *
     * Minimize this function to get the optimal parameter.
     *
     * \param[in] reg_param Regularization parameter.
     * \return Value of the MLE objective function.
     */
    [[nodiscard]] auto calc_mle_objective(const scalar_type& reg_param) const
        -> scalar_type {
        constexpr scalar_type limit = std::numeric_limits<scalar_type>::max() *
            static_cast<scalar_type>(1e-20);
        if (kernel_eigen_.eigenvalues()(0) + reg_param <=
            static_cast<scalar_type>(0)) {
            return limit;
        }

        using std::log;
        const scalar_type value =
            spectre_.rows() * log(calc_reg_term(reg_param)) +
            calc_log_determinant(reg_param);
        if (value < limit) {
            return value;
        }
        return limit;
    }

    /*!
     * \brief Calculate the coefficient of the kernel common in variables.
     *
     * \param[in] reg_param Regularization parameter.
     * \return Value.
     */
    [[nodiscard]] auto calc_common_coeff(const scalar_type& reg_param) const
        -> scalar_type {
        return calc_reg_term(reg_param) / spectre_.rows();
    }

private:
    /*!
     * \brief Calculate the regularization term.
     *
     * \param[in] reg_param Regularization parameter.
     * \return Value.
     */
    [[nodiscard]] auto calc_reg_term(const scalar_type& reg_param) const
        -> scalar_type {
        return (spectre_.array().abs2().rowwise().sum() /
            (kernel_eigen_.eigenvalues().array() + reg_param))
            .sum();
    }

    /*!
     * \brief Calculate the logarithm of the determinant of kernel matrix plus
     * regularization parameter.
     *
     * \param[in] reg_param Regularization parameter.
     * \return Value.
     */
    [[nodiscard]] auto calc_log_determinant(const scalar_type& reg_param) const
        -> scalar_type {
        return (kernel_eigen_.eigenvalues().array() + reg_param).log().sum();
    }

    //! Eigen-decomposition of the kernel matrix.
    Eigen::SelfAdjointEigenSolver<kernel_mat_type> kernel_eigen_{};

    //! Transformed data vector.
    data_type spectre_{};
};

}  // namespace num_collect::interp::kernel::impl
