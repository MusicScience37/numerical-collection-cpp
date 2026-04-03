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
 * \brief Definition of linear_first_order_ode_problem class.
 */
#pragma once

#include "num_collect/base/concepts/dense_matrix_of.h"
#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/concepts/sparse_matrix_of.h"
#include "num_collect/ode/evaluation_type.h"

namespace num_collect::ode::problems {

/*!
 * \brief Class of linear first-order ODE problems.
 *
 * This class defines a problem as follows:
 *
 * \f[
 * \frac{d}{dt} \boldsymbol{u} = A \boldsymbol{u} + \boldsymbol{b}
 * \f]
 *
 * \tparam Variable Type of variables.
 * \tparam Matrix Type of coefficients matrices.
 */
template <base::concepts::real_scalar_dense_vector Variable, typename Matrix>
    requires base::concepts::dense_matrix_of<Matrix,
                 typename Variable::value_type> ||
    base::concepts::sparse_matrix_of<Matrix, typename Variable::value_type>
class linear_first_order_ode_problem {
public:
    //! Type of variables.
    using variable_type = Variable;

    //! Type of coefficients matrix.
    using matrix_type = Matrix;

    //! Type of scalars.
    using scalar_type = typename Variable::value_type;

    //! Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{.diff_coeff = true};

    /*!
     * \brief Constructor.
     *
     * \param[in] variable_coefficients Coefficients for variables.
     * \param[in] constant_term Constant term.
     */
    linear_first_order_ode_problem(const matrix_type& variable_coefficients,
        const variable_type& constant_term)
        : variable_coefficients_(variable_coefficients),
          constant_term_(constant_term) {}

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(scalar_type /*time*/, const variable_type& variable,
        num_collect::ode::evaluation_type /*evaluations*/) {
        diff_coeff_ = constant_term_;
        diff_coeff_.noalias() += variable_coefficients_ * variable;
    }

    /*!
     * \brief Get the differential coefficient.
     *
     * \return Differential coefficient.
     */
    [[nodiscard]] auto diff_coeff() const noexcept -> const variable_type& {
        return diff_coeff_;
    }

private:
    //! Coefficients for variables.
    matrix_type variable_coefficients_;

    //! Constant term.
    variable_type constant_term_;

    //! Differential coefficient.
    variable_type diff_coeff_;
};

}  // namespace num_collect::ode::problems
