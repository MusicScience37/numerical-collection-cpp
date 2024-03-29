/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of eigen_solver_of concept.
 */
#pragma once

namespace num_collect {
inline namespace base {
namespace concepts {

/*!
 * \brief Concept of Eigen's solvers of linear equations.
 *
 * \tparam T Type.
 * \tparam Matrix Type of matrices.
 * \tparam Vector Type of vectors.
 */
template <typename T, typename Matrix, typename Vector>
concept eigen_solver_of = requires(
    T& solver, const Matrix& coeff, const Vector& right, Vector& solution) {
    solver.compute(coeff);
    solution = solver.solve(right);
};

}  // namespace concepts
}  // namespace base
}  // namespace num_collect
