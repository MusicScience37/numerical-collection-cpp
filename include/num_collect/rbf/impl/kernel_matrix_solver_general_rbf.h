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
 * \brief Definition of kernel_matrix_solver class for general RBFs.
 */
#pragma once

#include <cmath>

#include <Eigen/Core>
#include <Eigen/LU>

#include "num_collect/base/precondition.h"
#include "num_collect/rbf/impl/kernel_matrix_solver_decl.h"
#include "num_collect/rbf/kernel_matrix_type.h"

namespace num_collect::rbf::impl {

/*!
 * \brief Class to solve linear equations of kernel matrices for general RBFs.
 *
 * \tparam KernelValue Type of values of the kernel.
 * \tparam FunctionValue Type of the function values.
 * \tparam UsesGlobalLengthParameter Whether to uses the globally fixed length
 * parameters.
 * \tparam UsesPositiveDefiniteKernel Whether to use positive definite kernels.
 *
 * This class solves the following linear equations:
 *
 * \f[
 * K \boldsymbol{c} = \boldsymbol{y}
 * \f]
 *
 * where
 * - \f$ K \f$ is a kernel matrix,
 * - \f$ \boldsymbol{c} \f$ is a vector of coefficients for the kernel,
 * - \f$ \boldsymbol{y} \f$ is a vector of function values.
 *
 * This specialization handles the case where the kernel matrix is dense and
 * uses the localized length parameters which causes a non-symmetric kernel
 * matrix.
 */
template <typename KernelValue, typename FunctionValue,
    bool UsesGlobalLengthParameter, bool UsesPositiveDefiniteKernel>
class kernel_matrix_solver<KernelValue, FunctionValue,
    kernel_matrix_type::dense, UsesGlobalLengthParameter,
    UsesPositiveDefiniteKernel> {
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
        NUM_COLLECT_PRECONDITION(reg_param == static_cast<scalar_type>(0),
            "Non-zero regularization parameter cannot be used in this "
            "configuration.");

        coefficients = solver_.solve(data);
    }

private:
    //! Solver of the kernel matrix.
    Eigen::PartialPivLU<kernel_matrix_type> solver_;
};

}  // namespace num_collect::rbf::impl
