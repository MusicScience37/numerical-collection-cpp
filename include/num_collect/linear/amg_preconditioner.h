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
 * \brief Definition of amg_preconditioner class.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/linear/algebraic_multigrid_solver.h"

namespace num_collect::linear {

/*!
 * \brief Class to perform preconditioning in linear solvers of Eigen library
 * using algebraic multigrid method \cite Ruge1987.
 *
 * \tparam Matrix Type of the matrix.
 *
 * \note APIs are compatible with Eigen's preconditioners.
 *
 * This class assumes that the coefficient matrix is symmetric positive
 * definite.
 */
template <base::concepts::sparse_matrix Matrix>
class amg_preconditioner {
public:
    /*!
     * \brief Constructor.
     */
    amg_preconditioner() { solver_.max_iterations(1); }

    /*!
     * \brief Analyze the sparsity pattern of the matrix.
     *
     * \tparam MatType Type of the matrix.
     * \return This object.
     *
     * \note This function does nothing in this class.
     */
    template <typename MatType>
    auto analyzePattern(const MatType& /*mat*/)  // NOLINT(*-naming)
        -> amg_preconditioner& {
        return *this;
    }

    /*!
     * \brief Factorize the matrix.
     *
     * \tparam MatType Type of the matrix.
     * \param[in] mat The matrix to factorize.
     * \return This object.
     */
    template <typename MatType>
    auto factorize(const MatType& mat) -> amg_preconditioner& {
        solver_.compute(mat);
        return *this;
    }

    /*!
     * \brief Prepare to solve.
     *
     * \tparam MatType Type of the matrix.
     * \param[in] mat The matrix to prepare.
     * \return This object.
     */
    template <typename MatType>
    auto compute(const MatType& mat) -> amg_preconditioner& {
        factorize(mat);
        return *this;
    }

    /*!
     * \brief Solve a linear equation.
     *
     * \tparam Right Type of the right-hand-side vector.
     * \param[in] right Right-hand-side vector.
     * \return Expression to solve the linear equation.
     */
    template <typename Right>
    [[nodiscard]] auto solve(const Right& right) const
        -> Eigen::Solve<algebraic_multigrid_solver<Matrix>, Right> {
        return solver_.solve(right);
    }

    /*!
     * \brief Get the computation information.
     *
     * \return Computation information.
     */
    auto info() -> Eigen::ComputationInfo { return Eigen::Success; }

private:
    //! AMG solver.
    algebraic_multigrid_solver<Matrix> solver_;
};

}  // namespace num_collect::linear
