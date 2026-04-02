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
 * \brief Declaration of general_spline_equation_solver class.
 */
#pragma once

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
 * \tparam UsesPositiveDefiniteKernel Whether to use positive definite kernels.
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
template <typename KernelValue, typename FunctionValue,
    kernel_matrix_type KernelMatrixType, bool UsesGlobalLengthParameter,
    bool UsesPositiveDefiniteKernel>
class general_spline_equation_solver;

}  // namespace num_collect::rbf::impl
