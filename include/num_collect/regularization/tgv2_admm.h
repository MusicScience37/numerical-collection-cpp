/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of tgv2_admm class.
 */
#pragma once

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/regularization/iterative_regularized_solver_base.h"

namespace num_collect::regularization {

/*!
 * \brief Class to solve linear equations with 2nd order total generalized
 * variation (TGV) regularization \cite Bredies2010 using the alternating
 * direction method of multipliers (ADMM) \cite Boyd2010.
 *
 * \tparam Coeff Type of coefficient matrices.
 * \tparam DerivativeMatrix Type of matrices to compute derivatives.
 * \tparam Data Type of data vectors.
 *
 * This class minimizes the following evaluation function \cite Li2016 :
 *
 * \f[
 * E(\boldsymbol{x}, \boldsymbol{z}) =
 * \| A \boldsymbol{x} - \boldsymbol{y} \|_2^2
 * + \lambda (\| D \boldsymbol{x} - \boldsymbol{z} \|_1
 * + \alpha \| E \boldsymbol{z} \|_1)
 * \f]
 *
 * where variables are defined as follows:
 *
 * - \f$A\f$ is a coefficient matrix.
 * - \f$\boldsymbol{x}\f$ is a solution vector.
 * - \f$\boldsymbol{y}\f$ is a data vector.
 * - \f$\boldsymbol{z}\f$ is a component of the 1st order derivative
 *   of the solution which is used to compute the 2nd order derivative.
 * - \f$\lambda\f$ is a regularization parameter.
 * - \f$D\f$ is a matrix to compute derivatives.
 * - \f$E\f$ is a matrix to compute the 2nd order derivative
 *   from the 1st order derivative.
 *
 * For solving the above problem using ADMM, this class solves the following
 * optimization problem:
 *
 * \f[
 * \begin{aligned}
 *     \text{minimize} \hspace{1em} & \|A \boldsymbol{x} - \boldsymbol{y}\|_2^2
 *     + \lambda \left( \|\boldsymbol{s}\|_1 + \alpha \|\boldsymbol{t}\|_1
 * \right)
 *     \\
 *     \text{s.t.} \hspace{1em}     & D \boldsymbol{x} - \boldsymbol{z} =
 * \boldsymbol{s} \\ & E \boldsymbol{z} = \boldsymbol{t}
 * \end{aligned}
 * \f]
 *
 * where additional variables are defined as follows:
 *
 * - \f$\boldsymbol{s}\f$ is a component of the 1st order derivative of the
 *   solution for regularization.
 * - \f$\boldsymbol{t}\f$ is the 2nd order derivative of the solution.
 *
 * This class uses the following augmented Lagrangian function:
 *
 * \f[
 *   L_{\rho}(\boldsymbol{x}, \boldsymbol{z}, \boldsymbol{s}, \boldsymbol{t},
 * \boldsymbol{p}, \boldsymbol{u})
 *   \equiv
 *   \|A \boldsymbol{x} - \boldsymbol{y}\|_2^2
 *   + \lambda \left( \|\boldsymbol{s}\|_1 + \alpha \|\boldsymbol{t}\|_1 \right)
 *   + \boldsymbol{p}^\top (D \boldsymbol{x} - \boldsymbol{z} - \boldsymbol{s})
 *   + \boldsymbol{u}^\top (E \boldsymbol{z} - \boldsymbol{t})
 *   + \frac{\rho}{2} \|D \boldsymbol{x} - \boldsymbol{z} - \boldsymbol{s}\|_2^2
 *   + \frac{\rho}{2} \|E \boldsymbol{z} - \boldsymbol{t}\|_2^2
 * \f]
 *
 * where additional variables are defined as follows:
 *
 * - \f$\boldsymbol{p}\f$ is a Lagrange multiplier for the 1st order
 *   derivative constraint.
 * - \f$\boldsymbol{u}\f$ is a Lagrange multiplier for the 2nd order
 *   derivative constraint.
 * - \f$\rho\f$ is a coefficient of the quadratic term for the constraints.
 *
 * This class uses the following update rules:
 *
 * \f[
 * \begin{aligned}
 *     \boldsymbol{x}_{k+1}
 *      & = (2 A^\top A + \rho D^\top D)^{-1} \left(
 *     2 A^\top \boldsymbol{y} - D^\top \boldsymbol{p}_k + \rho D^\top
 * \boldsymbol{z}_k + \rho D^\top \boldsymbol{s}_k
 *     \right)
 *     \\
 *     \boldsymbol{z}_{k+1}
 *      & = (\rho I + \rho E^\top E)^{-1} \left(
 *     \boldsymbol{p}_k - E^\top \boldsymbol{u}_k + \rho D \boldsymbol{x}_{k+1}
 * - \rho \boldsymbol{s}_k + \rho E^\top \boldsymbol{t}_k
 *     \right)
 *     \\
 *     \boldsymbol{s}_{k+1}
 *      & = \mathcal{T}_{\lambda/\rho} \left( D \boldsymbol{x}_{k+1} -
 * \boldsymbol{z}_{k+1} + \frac{\boldsymbol{p}_k}{\rho} \right)
 *     \\
 *     \boldsymbol{t}_{k+1}
 *      & = \mathcal{T}_{\lambda/\rho \alpha} \left( E \boldsymbol{z}_{k+1} +
 * \frac{\boldsymbol{u}_k}{\rho} \right)
 *     \\
 *     \boldsymbol{p}_{k+1}
 *      & = \boldsymbol{p}_k + \rho (D \boldsymbol{x}_{k+1} -
 * \boldsymbol{z}_{k+1} - \boldsymbol{s}_{k+1})
 *     \\
 *     \boldsymbol{u}_{k+1}
 *      & = \boldsymbol{u}_k + \rho (E \boldsymbol{z}_{k+1} -
 * \boldsymbol{t}_{k+1})
 * \end{aligned}
 * \f]
 *
 * where \f$\mathcal{T}_\lambda\f$ is the thresholding operator
 * implemented in \ref
 * num_collect::regularization::impl::apply_shrinkage_operator function.
 *
 * This class uses the following variable names:
 *
 * | In Formula | In C++ |
 * | :--- | :--- |
 * | \f$\boldsymbol{x}\f$ | `solution` |
 * | \f$\boldsymbol{y}\f$ | `data` |
 * | \f$\boldsymbol{z}\f$ | `z` |
 * | \f$\boldsymbol{s}\f$ | `s` |
 * | \f$\boldsymbol{t}\f$ | `t` |
 * | \f$\boldsymbol{p}\f$ | `p` |
 * | \f$\boldsymbol{u}\f$ | `u` |
 * | \f$A\f$ | `coeff` |
 * | \f$D\f$ | `derivative_matrix` |
 * | \f$E\f$ | `divergence_matrix` |
 * | \f$\lambda\f$ | `param` |
 * | \f$\alpha\f$ | `second_derivative_ratio` |
 * | \f$\rho\f$ | `constraint_coeff` |
 */
template <typename Coeff, typename DerivativeMatrix,
    base::concepts::dense_vector Data>
    requires((base::concepts::sparse_matrix<Coeff> ||
                 base::concepts::dense_matrix<Coeff>) &&
        (base::concepts::sparse_matrix<DerivativeMatrix> ||
            base::concepts::dense_matrix<DerivativeMatrix>))
class tgv2_admm : public iterative_regularized_solver_base<
                      tgv2_admm<Coeff, DerivativeMatrix, Data>, Data> {
    // TODO implement here
};

}  // namespace num_collect::regularization
