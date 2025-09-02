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
 * \brief Definition of partial_derivative_operator class.
 */
#pragma once

#include <utility>

#include <Eigen/Core>

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/rbf/concepts/csrbf.h"
#include "num_collect/rbf/concepts/differentiable_rbf.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/impl/differentiate_polynomial_term.h"
#include "num_collect/rbf/operators/general_operator_evaluator.h"
#include "num_collect/rbf/operators/operator_evaluator.h"
#include "num_collect/rbf/polynomial_term_generator.h"
#include "num_collect/rbf/rbfs/differentiated.h"
#include "num_collect/util/assert.h"

namespace num_collect::rbf::operators {

/*!
 * \brief Class of operators to evaluate partial derivatives for the specified
 * dimension.
 *
 * \tparam Variable Type of variables.
 *
 * For all dimensions of partial derivatives, use
 * \ref num_collect::rbf::operators::gradient_operator.
 */
template <base::concepts::real_scalar_dense_vector Variable>
class partial_derivative_operator {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] variable Variable to evaluate the partial derivative at.
     * \param[in] dimension Dimension to partially differentiate.
     */
    partial_derivative_operator(Variable variable, index_type dimension)
        : variable_(std::move(variable)), dimension_(dimension) {
        NUM_COLLECT_PRECONDITION(
            dimension_ >= 0 && dimension_ < variable_.size(),
            "Dimension must be in [0, variable.size()).");
    }

    /*!
     * \brief Get the variable to evaluate the partial derivative at.
     *
     * \return Variable.
     */
    [[nodiscard]] auto variable() const noexcept -> const Variable& {
        return variable_;
    }

    /*!
     * \brief Get the dimension to partially differentiate.
     *
     * \return Dimension.
     */
    [[nodiscard]] auto dimension() const noexcept -> index_type {
        return dimension_;
    }

private:
    //! Variable to evaluate the slope at.
    Variable variable_;

    //! Dimension to partially differentiate.
    index_type dimension_;
};

/*!
 * \brief Specialization of num_collect::rbf::operators::operator_evaluator for
 * num_collect::rbf::operators::partial_derivative_operator for scalar
 * variables.
 */
template <base::concepts::real_scalar_dense_vector Variable,
    concepts::differentiable_rbf RBF>
struct operator_evaluator<partial_derivative_operator<Variable>, RBF,
    distance_functions::euclidean_distance_function<Variable>>
    : general_operator_evaluator<
          operator_evaluator<partial_derivative_operator<Variable>, RBF,
              distance_functions::euclidean_distance_function<Variable>>,
          partial_derivative_operator<Variable>, RBF,
          distance_functions::euclidean_distance_function<Variable>> {
    //! Type of the base.
    using base_type = general_operator_evaluator<
        operator_evaluator<partial_derivative_operator<Variable>, RBF,
            distance_functions::euclidean_distance_function<Variable>>,
        partial_derivative_operator<Variable>, RBF,
        distance_functions::euclidean_distance_function<Variable>>;

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
     * \brief Evaluate an operator for one sample point.
     *
     * \tparam FunctionValue Type of function values.
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     * \param[in] length_parameter Length parameter.
     * \param[in] target_operator Operator to evaluate.
     * \param[in] sample_variable Variable of the sample.
     * \param[in] kernel_coeff Coefficient of the kernel for the sample.
     * \return Evaluated function value.
     */
    template <typename FunctionValue>
    [[nodiscard]] static auto evaluate_for_one_sample(
        const distance_function_type& distance_function, const rbf_type& rbf,
        const kernel_value_type& length_parameter,
        const operator_type& target_operator,
        const variable_type& sample_variable, const FunctionValue& kernel_coeff)
        -> FunctionValue {
        // Not used for this operator.
        (void)rbf;

        NUM_COLLECT_PRECONDITION(
            length_parameter > static_cast<kernel_value_type>(0),
            "Length parameter must be a positive value.");
        const kernel_value_type distance_rate =
            distance_function(target_operator.variable(), sample_variable) /
            length_parameter;

        const rbfs::differentiated_t<rbf_type> differentiated_rbf;

        const index_type dimension = target_operator.dimension();

        if constexpr (concepts::csrbf<rbf_type>) {
            if (distance_rate < rbf_type::support_boundary()) {
                return -kernel_coeff * differentiated_rbf(distance_rate) *
                    (target_operator.variable()(dimension) -
                        sample_variable(dimension)) /
                    (length_parameter * length_parameter);
            }
            return static_cast<FunctionValue>(0);
        } else {
            return -kernel_coeff * differentiated_rbf(distance_rate) *
                (target_operator.variable()(dimension) -
                    sample_variable(dimension)) /
                (length_parameter * length_parameter);
        }
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

        Eigen::Vector<int, variable_dimensions> orders =
            Eigen::Vector<int, variable_dimensions>::Zero();
        orders(target_operator.dimension()) = 1;

        auto value = initial_value<coeff_type>();
        for (index_type i = 0; i < term_generator.terms().size(); ++i) {
            const auto differentiation_result =
                impl::differentiate_polynomial_term<coeff_type>(
                    term_generator.terms()[i], orders);
            if (differentiation_result) {
                value +=
                    differentiation_result->first(target_operator.variable()) *
                    differentiation_result->second * polynomial_coefficients(i);
            }
        }
        return value;
    }
};

}  // namespace num_collect::rbf::operators
