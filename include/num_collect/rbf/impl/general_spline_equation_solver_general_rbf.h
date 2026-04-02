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
 * \brief Definition of general_spline_equation_solver for general RBFs.
 */
#pragma once

#include <cmath>

#include <Eigen/Core>
#include <Eigen/LU>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/rbf/impl/general_spline_equation_solver_decl.h"
#include "num_collect/rbf/kernel_matrix_type.h"

namespace num_collect::rbf::impl {

/*!
 * \brief Class to solve linear equations of kernel matrices and matrices of
 * additional terms in RBF interpolation for general RBFs.
 * \tparam UsesGlobalLengthParameter Whether to uses the globally fixed length
 * parameters.
 * \tparam UsesPositiveDefiniteKernel Whether to use positive definite kernels.
 *
 * \tparam KernelValue Type of values of the kernel.
 * \tparam FunctionValue Type of the function values.
 *
 * This class solves the following linear equations:
 *
 * \f[
 * \begin{pmatrix}
 * K & P \\
 * P^T & O
 * \end{pmatrix}
 * \begin{pmatrix}
 * \boldsymbol{c} \\
 * \boldsymbol{d}
 * \end{pmatrix}
 * =
 * \begin{pmatrix}
 * \boldsymbol{y} \\
 * \boldsymbol{0}
 * \end{pmatrix}
 * \f]
 *
 * where
 * - \f$ K \f$ is a kernel matrix,
 * - \f$ P \f$ is a matrix of additional terms,
 * - \f$ \boldsymbol{c} \f$ is a vector of coefficients for the kernel,
 * - \f$ \boldsymbol{d} \f$ is a vector of coefficients for the additional
 * terms,
 * - \f$ \boldsymbol{y} \f$ is a vector of function values.
 */
template <base::concepts::real_scalar KernelValue, typename FunctionValue,
    bool UsesGlobalLengthParameter, bool UsesPositiveDefiniteKernel>
class general_spline_equation_solver<KernelValue, FunctionValue,
    kernel_matrix_type::dense, UsesGlobalLengthParameter,
    UsesPositiveDefiniteKernel> {
public:
    //! Type of matrices of kernels.
    using matrix_type = Eigen::MatrixX<KernelValue>;

    //! Type of matrices of kernels.
    using kernel_matrix_type = matrix_type;

    //! Type of matrices of additional terms.
    using additional_matrix_type = matrix_type;

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
        NUM_COLLECT_PRECONDITION(kernel_matrix.cols() == kernel_matrix.rows(),
            "Kernel matrix must be a square matrix.");
        num_variables_ = kernel_matrix.rows();
        NUM_COLLECT_PRECONDITION(additional_matrix.rows() == num_variables_,
            "Matrix of additional terms must have the same number of rows as "
            "the kernel matrix.");
        num_additional_terms_ = additional_matrix.cols();
        NUM_COLLECT_PRECONDITION(num_variables_ > num_additional_terms_,
            "The number of variables must be larger than the number of "
            "additional terms.");
        const index_type whole_size = num_variables_ + num_additional_terms_;

        whole_coefficient_matrix_.resize(whole_size, whole_size);
        whole_coefficient_matrix_.topLeftCorner(
            num_variables_, num_variables_) = kernel_matrix;
        whole_coefficient_matrix_.topRightCorner(
            num_variables_, num_additional_terms_) = additional_matrix;
        whole_coefficient_matrix_.bottomLeftCorner(num_additional_terms_,
            num_variables_) = additional_matrix.transpose();
        whole_coefficient_matrix_
            .bottomRightCorner(num_additional_terms_, num_additional_terms_)
            .setZero();
        lu_decomposition_.compute(whole_coefficient_matrix_);

        whole_rhs_vector_.resize(whole_coefficient_matrix_.rows());
        whole_rhs_vector_.head(num_variables_) = data;
        whole_rhs_vector_.tail(num_additional_terms_).setZero();
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
        vector_type& additional_coefficients, scalar_type reg_param) {
        NUM_COLLECT_PRECONDITION(reg_param == static_cast<scalar_type>(0),
            "Non-zero regularization parameter cannot be used in this "
            "configuration.");

        whole_solution_vector_ = lu_decomposition_.solve(whole_rhs_vector_);
        kernel_coefficients = whole_solution_vector_.head(num_variables_);
        additional_coefficients =
            whole_solution_vector_.tail(num_additional_terms_);
    }

private:
    //! Whole coefficient matrix of the linear equation.
    matrix_type whole_coefficient_matrix_{};

    //! Whole right-hand-side vector of the linear equation.
    vector_type whole_rhs_vector_{};

    //! Whole solution vector of the linear equation.
    vector_type whole_solution_vector_{};

    //! LU decomposition of the whole coefficient matrix.
    Eigen::PartialPivLU<matrix_type> lu_decomposition_{};

    //! Number of variables.
    index_type num_variables_{};

    //! Number of additional terms.
    index_type num_additional_terms_{};
};

}  // namespace num_collect::rbf::impl
