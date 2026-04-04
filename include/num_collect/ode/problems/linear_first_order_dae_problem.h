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
 * \brief Definition of linear_first_order_dae_problem class.
 */
#pragma once

#include "num_collect/base/concepts/dense_matrix_of.h"
#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/concepts/sparse_matrix_of.h"
#include "num_collect/ode/evaluation_type.h"

namespace num_collect::ode::problems {

/*!
 * \brief Class of linear first-order DAE problems.
 *
 * This class defines a problem as follows:
 *
 * \f[
 * M \frac{d}{dt} \boldsymbol{u} = A \boldsymbol{u} + \boldsymbol{b}
 * \f]
 *
 * - \f$M\f$ is a mass matrix.
 * - \f$A\f$ is a stiffness matrix.
 * - \f$\boldsymbol{b}\f$ is a load vector.
 *
 * \tparam Vector Type of vectors.
 * \tparam Matrix Type of coefficients matrices.
 */
template <base::concepts::real_scalar_dense_vector Vector, typename Matrix>
    requires base::concepts::dense_matrix_of<Matrix,
                 typename Vector::value_type> ||
    base::concepts::sparse_matrix_of<Matrix, typename Vector::value_type>
class linear_first_order_dae_problem {
public:
    //! Type of vectors.
    using vector_type = Vector;

    //! Type of coefficients matrix.
    using matrix_type = Matrix;

    //! Type of scalars.
    using scalar_type = typename vector_type::value_type;

    //! Type of variables. (Used by ODE solvers.)
    using variable_type = vector_type;

    //! Type of mass matrix. (Used by ODE solvers.)
    using mass_type = matrix_type;

    //! Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{.diff_coeff = true, .mass = true};

    /*!
     * \brief Constructor.
     *
     * \param[in] mass_matrix Mass matrix.
     * \param[in] stiffness_matrix Stiffness matrix.
     * \param[in] load_vector Load vector.
     */
    linear_first_order_dae_problem(const mass_type& mass_matrix,
        const matrix_type& stiffness_matrix, const vector_type& load_vector)
        : mass_matrix_(mass_matrix),
          stiffness_matrix_(stiffness_matrix),
          load_vector_(load_vector) {}

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(scalar_type /*time*/, const variable_type& variable,
        num_collect::ode::evaluation_type /*evaluations*/) {
        diff_coeff_ = load_vector_;
        diff_coeff_.noalias() += stiffness_matrix_ * variable;
    }

    /*!
     * \brief Get the differential coefficient.
     *
     * \return Differential coefficient.
     */
    [[nodiscard]] auto diff_coeff() const noexcept -> const variable_type& {
        return diff_coeff_;
    }

    /*!
     * \brief Get the mass matrix.
     *
     * \return Mass matrix.
     */
    [[nodiscard]] auto mass() const noexcept -> const mass_type& {
        return mass_matrix_;
    }

private:
    //! Mass matrix.
    matrix_type mass_matrix_;

    //! Stiffness matrix.
    matrix_type stiffness_matrix_;

    //! Load vector.
    vector_type load_vector_;

    //! Differential coefficient.
    vector_type diff_coeff_;
};

}  // namespace num_collect::ode::problems
