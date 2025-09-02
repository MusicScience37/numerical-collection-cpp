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
 * \brief Definition of general_differential_operator_evaluator struct.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/rbf/impl/differentiate_polynomial_term.h"
#include "num_collect/rbf/operators/general_operator_evaluator.h"
#include "num_collect/rbf/polynomial_term_generator.h"

namespace num_collect::rbf::operators {

/*!
 * \brief Struct of common implementation for
 * num_collect::rbf::operators::operator_evaluator for differential operators
 * returning scalars.
 *
 * \tparam Operator Type of the operator.
 * \tparam RBF Type of the RBF.
 * \tparam DistanceFunction Type of the distance function.
 */
template <typename Derived, typename Operator, concepts::rbf RBF,
    concepts::distance_function DistanceFunction>
struct general_differential_operator_evaluator
    : general_operator_evaluator<Derived, Operator, RBF, DistanceFunction> {
    //! Type of the base.
    using base_type =
        general_operator_evaluator<Derived, Operator, RBF, DistanceFunction>;

    using base_type::variable_dimensions;
    using typename base_type::distance_function_type;
    using typename base_type::kernel_value_type;
    using typename base_type::operator_type;
    using typename base_type::rbf_type;
    using typename base_type::variable_type;

    /*!
     * \brief Get the initial value for accumulation of values evaluated for
     * samples points.
     *
     * \tparam KernelCoeff Type of the coefficients of kernels.
     * \return Initial value.
     */
    template <typename KernelCoeff>
    [[nodiscard]] static auto initial_value() -> KernelCoeff {
        return static_cast<KernelCoeff>(0);
    }

    /*!
     * \brief Evaluate a polynomial.
     *
     * \tparam CoeffVector Type of the vector of coefficients of the polynomial.
     * \param[in] target_operator Operator to evaluate.
     * \param[in] term_generator Generator of polynomial terms.
     * \param[in] polynomial_coefficients Coefficients of the polynomial.
     * \return Evaluated polynomial value.
     */
    template <base::concepts::dense_vector CoeffVector>
    [[nodiscard]] static auto evaluate_polynomial(
        const operator_type& target_operator,
        const polynomial_term_generator<variable_dimensions>& term_generator,
        const CoeffVector& polynomial_coefficients) {
        using coeff_type = typename CoeffVector::Scalar;

        NUM_COLLECT_DEBUG_ASSERT(
            term_generator.terms().size() == polynomial_coefficients.size());

        static const auto orders_list = Derived::differentiations();

        auto value = initial_value<coeff_type>();
        for (index_type i = 0; i < term_generator.terms().size(); ++i) {
            for (const auto& orders : orders_list) {
                const auto differentiation_result =
                    impl::differentiate_polynomial_term<coeff_type>(
                        term_generator.terms()[i], orders);
                if (differentiation_result) {
                    value += differentiation_result->first(
                                 target_operator.variable()) *
                        differentiation_result->second *
                        polynomial_coefficients(i);
                }
            }
        }
        return value;
    }
};

}  // namespace num_collect::rbf::operators
