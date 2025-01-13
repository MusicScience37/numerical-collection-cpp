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
 * \brief Definition of kernel_matrix_solver class.
 */
#pragma once

#include <cmath>
#include <limits>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/IterativeLinearSolvers>
#include <Eigen/LU>
#include <Eigen/SparseCore>
#include <Eigen/src/Core/util/Constants.h>

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/exception.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/rbf/kernel_matrix_type.h"

namespace num_collect::rbf::impl {

/*!
 * \brief Class to solve linear equations of kernel matrices.
 *
 * \tparam KernelValue Type of values of the kernel.
 * \tparam FunctionValue Type of the function values.
 * \tparam KernelMatrixType Type of the kernel matrices.
 * \tparam UsesGlobalLengthParameter Whether to uses the globally fixed length
 * parameters.
 */
template <typename KernelValue, typename FunctionValue,
    kernel_matrix_type KernelMatrixType, bool UsesGlobalLengthParameter>
class kernel_matrix_solver;

/*!
 * \brief Class to solve linear equations of kernel matrices.
 *
 * \tparam KernelValue Type of values of the kernel.
 * \tparam FunctionValue Type of the function values.
 */
template <typename KernelValue, typename FunctionValue>
class kernel_matrix_solver<KernelValue, FunctionValue,
    kernel_matrix_type::dense, true> {
public:
    //! Type of matrices.
    using kernel_matrix_type = Eigen::MatrixX<KernelValue>;

    //! Type of vectors.
    using vector_type = Eigen::VectorX<FunctionValue>;

    //! Type of scalars.
    using scalar_type = KernelValue;

    //! Constructor.
    kernel_matrix_solver() = default;

    /*!
     * \brief Compute internal matrices.
     *
     * \param[in] kernel_matrix Kernel matrix.
     * \param[in] data Data vector.
     */
    void compute(
        const kernel_matrix_type& kernel_matrix, const vector_type& data) {
        decomposition_.compute(kernel_matrix, Eigen::ComputeEigenvectors);
        spectre_ = decomposition_.eigenvectors().adjoint() * data;
    }

    /*!
     * \brief Solve the linear equation with a regularization parameter.
     *
     * \param[out] coefficients Vector of coefficients for each sample points.
     * \param[in] reg_param Regularization parameter.
     * \param[in] data Data vector.
     */
    void solve(vector_type& coefficients, scalar_type reg_param,
        const vector_type& data) const {
        (void)data;  // not used in this version.

        reg_param = correct_reg_param_if_needed(reg_param);

        coefficients = decomposition_.eigenvectors() *
            (decomposition_.eigenvalues().array() + reg_param)
                .inverse()
                .matrix()
                .asDiagonal() *
            spectre_;
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
        if (decomposition_.eigenvalues()(0) + reg_param <=
            static_cast<scalar_type>(0)) {
            return limit;
        }

        using std::log;
        const scalar_type value = static_cast<scalar_type>(spectre_.rows()) *
                log(calc_reg_term(reg_param)) +
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
    [[nodiscard]] auto calc_common_coeff(scalar_type reg_param) const
        -> scalar_type {
        reg_param = correct_reg_param_if_needed(reg_param);

        return calc_reg_term(reg_param) /
            static_cast<scalar_type>(spectre_.rows());
    }

    /*!
     * \brief Calculate the regularization term for a vector.
     *
     * \tparam InputData Type of the input data.
     * \param[in] reg_param Regularization parameter.
     * \param[in] data Data vector.
     * \return Value.
     */
    template <base::concepts::dense_vector InputData>
    [[nodiscard]] auto calc_reg_term(
        const InputData& data, scalar_type reg_param) const -> scalar_type {
        reg_param = correct_reg_param_if_needed(reg_param);

        return ((decomposition_.eigenvectors().adjoint() * data)
                    .array()
                    .abs2()
                    .rowwise()
                    .sum() /
            (decomposition_.eigenvalues().array() + reg_param))
            .sum();
    }

    /*!
     * \brief Get eigenvalues.
     *
     * \return Eigenvalues.
     */
    [[nodiscard]] auto eigenvalues() const -> decltype(auto) {
        return decomposition_.eigenvalues();
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
            (decomposition_.eigenvalues().array() + reg_param))
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
        return (decomposition_.eigenvalues().array() + reg_param).log().sum();
    }

    /*!
     * \brief Correct regularization parameter if needed.
     *
     * \param[in] reg_param Regularization parameter.
     * \return Corrected parameter value.
     */
    [[nodiscard]] auto correct_reg_param_if_needed(
        const scalar_type& reg_param) const noexcept -> scalar_type {
        const scalar_type smallest_eigenvalue = decomposition_.eigenvalues()(0);
        const scalar_type largest_eigenvalue = decomposition_.eigenvalues()(
            decomposition_.eigenvalues().size() - 1);
        const scalar_type eigenvalue_safe_limit =
            largest_eigenvalue * std::numeric_limits<scalar_type>::epsilon();
        const scalar_type reg_param_safe_limit =
            eigenvalue_safe_limit - smallest_eigenvalue;

        if (reg_param < reg_param_safe_limit) {
            return reg_param_safe_limit;
        }
        return reg_param;
    }

    //! Eigen decomposition of the kernel matrix.
    Eigen::SelfAdjointEigenSolver<kernel_matrix_type> decomposition_;

    //! Data transformed to the space determined by the eigen vectors.
    vector_type spectre_;
};

/*!
 * \brief Class to solve linear equations of kernel matrices.
 *
 * \tparam KernelValue Type of values of the kernel.
 * \tparam FunctionValue Type of the function values.
 */
template <typename KernelValue, typename FunctionValue>
class kernel_matrix_solver<KernelValue, FunctionValue,
    kernel_matrix_type::dense, false> {
public:
    //! Type of matrices.
    using kernel_matrix_type = Eigen::MatrixX<KernelValue>;

    //! Type of vectors.
    using vector_type = Eigen::VectorX<FunctionValue>;

    //! Type of scalars.
    using scalar_type = KernelValue;

    //! Constructor.
    kernel_matrix_solver() = default;

    /*!
     * \brief Compute internal matrices.
     *
     * \param[in] kernel_matrix Kernel matrix.
     * \param[in] data Data vector.
     */
    void compute(
        const kernel_matrix_type& kernel_matrix, const vector_type& data) {
        (void)data;  // not used in this version.

        solver_.compute(kernel_matrix);
    }

    /*!
     * \brief Solve the linear equation with a regularization parameter.
     *
     * \param[out] coefficients Vector of coefficients for each sample points.
     * \param[in] reg_param Regularization parameter.
     * \param[in] data Data vector.
     */
    void solve(vector_type& coefficients, scalar_type reg_param,
        const vector_type& data) const {
        if (reg_param != static_cast<scalar_type>(0)) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "Non-zero regularization parameter cannot be used in this "
                "configuration.");
        }

        coefficients = solver_.solve(data);
    }

private:
    //! Solver of the kernel matrix.
    Eigen::PartialPivLU<kernel_matrix_type> solver_;
};

/*!
 * \brief Class to solve linear equations of kernel matrices.
 *
 * \tparam KernelValue Type of values of the kernel.
 * \tparam FunctionValue Type of the function values.
 */
template <typename KernelValue, typename FunctionValue>
class kernel_matrix_solver<KernelValue, FunctionValue,
    kernel_matrix_type::sparse, false> {
public:
    //! Type of matrices.
    using kernel_matrix_type =
        Eigen::SparseMatrix<KernelValue, Eigen::RowMajor>;

    //! Type of vectors.
    using vector_type = Eigen::VectorX<FunctionValue>;

    //! Type of scalars.
    using scalar_type = KernelValue;

    //! Constructor.
    kernel_matrix_solver() = default;

    /*!
     * \brief Compute internal matrices.
     *
     * \param[in] kernel_matrix Kernel matrix.
     * \param[in] data Data vector.
     */
    void compute(
        const kernel_matrix_type& kernel_matrix, const vector_type& data) {
        (void)data;  // not used in this version.

        solver_.compute(kernel_matrix);
    }

    /*!
     * \brief Solve the linear equation with a regularization parameter.
     *
     * \param[out] coefficients Vector of coefficients for each sample points.
     * \param[in] reg_param Regularization parameter.
     * \param[in] data Data vector.
     */
    void solve(vector_type& coefficients, scalar_type reg_param,
        const vector_type& data) const {
        if (reg_param != static_cast<scalar_type>(0)) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "Non-zero regularization parameter cannot be used in this "
                "configuration.");
        }

        coefficients = solver_.solve(data);
    }

private:
    //! Solver of the kernel matrix.
    Eigen::BiCGSTAB<kernel_matrix_type> solver_;
};

}  // namespace num_collect::rbf::impl
