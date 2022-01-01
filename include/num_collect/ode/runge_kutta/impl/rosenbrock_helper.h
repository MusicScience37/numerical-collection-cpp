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
 * \brief Definition of ros3w_formula class.
 */
#pragma once

#include <type_traits>

#include <Eigen/LU>

#include "num_collect/constants/one.h"
#include "num_collect/util/is_eigen_matrix.h"

namespace num_collect::ode::runge_kutta::impl {

/*!
 * \brief Class to help implementation of Rosenbrock method.
 *
 * \tparam Jacobian Type of Jacobian.
 */
template <typename Jacobian, typename = void>
class rosenbrock_helper;

/*!
 * \brief Class to help implementation of Rosenbrock method.
 *
 * \tparam Jacobian Type of Jacobian.
 */
template <typename Jacobian>
class rosenbrock_helper<Jacobian,
    std::enable_if_t<is_eigen_matrix_v<Jacobian>>> {
public:
    /*!
     * \brief Compute LU decomposition.
     *
     * \tparam T Type of matrix.
     * \param[in] matrix Matrix.
     */
    template <typename T>
    void compute(const T& matrix) {
        const auto size = matrix.rows();
        lu_.compute(Jacobian::Identity(size, size) - matrix);
    }

    /*!
     * \brief Solve an equation.
     *
     * \tparam T Variable type.
     * \param[in] right Right-hand-side variable.
     * \return Solution.
     */
    template <typename T>
    auto solve(const T& right) {
        return lu_.solve(right);
    }

private:
    //! Solver.
    Eigen::PartialPivLU<Jacobian> lu_{};
};

/*!
 * \brief Class to help implementation of Rosenbrock method.
 *
 * \tparam Jacobian Type of Jacobian.
 */
template <typename Jacobian>
class rosenbrock_helper<Jacobian,
    std::enable_if_t<std::is_floating_point_v<Jacobian>>> {
public:
    /*!
     * \brief Compute the inverse of the Jacobian.
     *
     * \param[in] jacobian Jacobian.
     */
    void compute(const Jacobian& jacobian) {
        inverse_ =
            constants::one<Jacobian> / (constants::one<Jacobian> - jacobian);
    }

    /*!
     * \brief Solve an equation.
     *
     * \param[in] right Right-hand-side variable.
     * \return Solution.
     */
    auto solve(const Jacobian& right) { return inverse_ * right; }

private:
    //! Inverse.
    Jacobian inverse_{};
};

}  // namespace num_collect::ode::runge_kutta::impl
