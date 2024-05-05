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
 * \brief Definition of general_spline_equation_solver class.
 */
#pragma once

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/QR>
#include <fmt/core.h>

#include "num_collect/base/concepts/real_scalar.h"  // IWYU pragma: keep
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/kernel_matrix_type.h"

namespace num_collect::rbf::impl {

/*!
 * \brief Class to solve linear equations of kernel matrices and matrices of
 * additional terms in RBF interpolation.
 *
 * \tparam KernelValue Type of values of the kernel.
 * \tparam FunctionValue Type of the function values.
 * \tparam KernelMatrixType Type of the kernel matrices.
 * \tparam UsesGlobalLengthParameter Whether to uses the globally fixed length
 * parameters.
 */
template <typename KernelValue, typename FunctionValue,
    kernel_matrix_type KernelMatrixType, bool UsesGlobalLengthParameter>
class general_spline_equation_solver;

/*!
 * \brief Class to solve linear equations of kernel matrices and matrices of
 * additional terms in RBF interpolation.
 *
 * \tparam KernelValue Type of values of the kernel.
 * \tparam FunctionValue Type of the function values.
 */
template <base::concepts::real_scalar KernelValue, typename FunctionValue>
class general_spline_equation_solver<KernelValue, FunctionValue,
    kernel_matrix_type::dense, true> {
public:
    //! Type of matrices of kernels.
    using kernel_matrix_type = Eigen::MatrixX<KernelValue>;

    //! Type of matrices of additional terms.
    using additional_matrix_type = Eigen::MatrixX<KernelValue>;

    //! Type of vectors.
    using vector_type = Eigen::VectorX<FunctionValue>;

    //! Type of scalars.
    using scalar_type = KernelValue;

    //! Constructor.
    general_spline_equation_solver() = default;

    /*!
     * \brief Compute internal matrices.
     *
     * \param[in] kernel_matrix Kernel matrix.
     * \param[in] additional_matrix Matrix of additional terms.
     * \param[in] data Data vector.
     */
    void compute(const kernel_matrix_type& kernel_matrix,
        const additional_matrix_type& additional_matrix,
        const vector_type& data) {
        num_variables_ = kernel_matrix.rows();
        if (kernel_matrix.cols() != num_variables_) {
            throw invalid_argument("Kernel matrix must be a square matrix.");
        }
        if (additional_matrix.rows() != num_variables_) {
            throw invalid_argument(
                "Matrix of additional terms must have the same number of rows "
                "as the kernel matrix.");
        }
        num_additional_terms_ = additional_matrix.cols();
        if (num_variables_ <= num_additional_terms_) {
            throw invalid_argument(
                "The number of variables must be larger than the number of "
                "additional terms.");
        }
        kernel_subspace_dimensions_ = num_variables_ - num_additional_terms_;

        qr_decomposition_.compute(additional_matrix);
        if (qr_decomposition_.rank() != additional_matrix.cols()) {
            throw algorithm_failure(
                fmt::format("The matrix of additional terms must have full "
                            "column rank. (columns={}, rand={})",
                    additional_matrix.cols(), qr_decomposition_.rank()));
        }
        q_matrix_ = qr_decomposition_.householderQ();

        transformed_kernel_matrix_ =
            q_matrix_.rightCols(kernel_subspace_dimensions_).transpose() *
            kernel_matrix * q_matrix_.rightCols(kernel_subspace_dimensions_);

        eigen_value_decomposition_.compute(
            transformed_kernel_matrix_, Eigen::ComputeEigenvectors);
        data_transformation_matrix_ =
            eigen_value_decomposition_.eigenvectors().transpose() *
            q_matrix_.rightCols(kernel_subspace_dimensions_).transpose();
        spectre_ = data_transformation_matrix_ * data;

        kernel_matrix_ = &kernel_matrix;
        data_ = &data;
    }

    /*!
     * \brief Solve the linear equation with a regularization parameter.
     *
     * \param[out] kernel_coefficients Vector of coefficients of kernels for
     * each sample points.
     * \param[out] additional_coefficients Vector of coefficients of additional
     * terms.
     * \param[in] reg_param Regularization parameter.
     */
    void solve(vector_type& kernel_coefficients,
        vector_type& additional_coefficients, scalar_type reg_param) const {
        if (kernel_matrix_ == nullptr || data_ == nullptr) {
            throw precondition_not_satisfied(
                "compute() must be called before solve().");
        }

        reg_param = correct_reg_param_if_needed(reg_param);

        kernel_coefficients = data_transformation_matrix_.transpose() *
            (eigen_value_decomposition_.eigenvalues().array() + reg_param)
                .inverse()
                .matrix()
                .asDiagonal() *
            spectre_;

        additional_coefficients = qr_decomposition_.solve(
            (*data_) - (*kernel_matrix_) * kernel_coefficients);
    }

    /*!
     * \brief Calculate maximum likelihood estimation (MLE) objective function
     * \cite Scheuerer2011.
     *
     * Minimize this function to get the optimal parameter.
     *
     * \param[in] reg_param Regularization parameter.
     * \return Value of the MLE objective function.
     */
    [[nodiscard]] auto calc_mle_objective(scalar_type reg_param) const
        -> scalar_type {
        reg_param = correct_reg_param_if_needed(reg_param);

        constexpr scalar_type limit = std::numeric_limits<scalar_type>::max() *
            static_cast<scalar_type>(1e-20);
        if (eigen_value_decomposition_.eigenvalues()(0) + reg_param <=
            static_cast<scalar_type>(0)) {
            return limit;
        }

        using std::log;
        const scalar_type value =
            static_cast<scalar_type>(kernel_subspace_dimensions_) *
                log(calc_reg_term(reg_param)) +
            calc_log_determinant(reg_param);
        if (value < limit) {
            return value;
        }
        return limit;
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
            (eigen_value_decomposition_.eigenvalues().array() + reg_param))
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
        return (eigen_value_decomposition_.eigenvalues().array() + reg_param)
            .log()
            .sum();
    }

    /*!
     * \brief Correct regularization parameter if needed.
     *
     * \param[in] reg_param Regularization parameter.
     * \return Corrected parameter value.
     */
    [[nodiscard]] auto correct_reg_param_if_needed(
        const scalar_type& reg_param) const noexcept -> scalar_type {
        const scalar_type smallest_eigenvalue =
            eigen_value_decomposition_.eigenvalues()(0);
        const scalar_type largest_eigenvalue =
            eigen_value_decomposition_.eigenvalues()(
                eigen_value_decomposition_.eigenvalues().size() - 1);
        const scalar_type eigenvalue_safe_limit =
            largest_eigenvalue * std::numeric_limits<scalar_type>::epsilon();
        const scalar_type reg_param_safe_limit =
            eigenvalue_safe_limit - smallest_eigenvalue;

        if (reg_param < reg_param_safe_limit) {
            return reg_param_safe_limit;
        }
        return reg_param;
    }

    //! Eigenvalue decomposition.
    Eigen::SelfAdjointEigenSolver<kernel_matrix_type>
        eigen_value_decomposition_{};

    //! QR decomposition_.
    Eigen::ColPivHouseholderQR<additional_matrix_type> qr_decomposition_{};

    //! Q matrix in QR decomposition.
    kernel_matrix_type q_matrix_{};

    //! Kernel matrix transformed to the subspace not expressed by additional terms.
    kernel_matrix_type transformed_kernel_matrix_{};

    //! Matrix to transform data into the space determined by the eigen vectors.
    kernel_matrix_type data_transformation_matrix_{};

    //! Data transformed to the space determined by the eigen vectors.
    vector_type spectre_{};

    //! Kernel matrix.
    const kernel_matrix_type* kernel_matrix_{nullptr};

    //! Data.
    const vector_type* data_{nullptr};

    //! Number of variables.
    index_type num_variables_{};

    //! Number of additional terms.
    index_type num_additional_terms_{};

    //! Number of dimensions of the subspace which will be expressed using kernel matrices.
    index_type kernel_subspace_dimensions_{};
};

}  // namespace num_collect::rbf::impl
