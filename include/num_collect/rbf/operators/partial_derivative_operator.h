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

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/rbf/concepts/csrbf.h"
#include "num_collect/rbf/concepts/differentiable_rbf.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/operators/general_differential_operator_evaluator.h"
#include "num_collect/rbf/operators/operator_base.h"
#include "num_collect/rbf/operators/operator_evaluator.h"
#include "num_collect/rbf/rbfs/differentiated.h"

namespace num_collect::rbf::operators {

/*!
 * \brief Class of operators to evaluate partial derivatives for the specified
 * dimension.
 *
 * \tparam Variable Type of variables.
 * \tparam DifferentiatedDimension Dimension to partially differentiate.
 *
 * For all dimensions of partial derivatives, use
 * \ref num_collect::rbf::operators::gradient_operator.
 */
template <base::concepts::real_scalar_dense_vector Variable,
    index_type DifferentiatedDimension>
class partial_derivative_operator
    : public operator_base<
          partial_derivative_operator<Variable, DifferentiatedDimension>> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] variable Variable to evaluate the partial derivative at.
     */
    explicit partial_derivative_operator(Variable variable)
        : variable_(std::move(variable)) {
        NUM_COLLECT_PRECONDITION(DifferentiatedDimension >= 0 &&
                DifferentiatedDimension < variable_.size(),
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

private:
    //! Variable to evaluate the slope at.
    Variable variable_;
};

/*!
 * \brief Specialization of num_collect::rbf::operators::operator_evaluator for
 * num_collect::rbf::operators::partial_derivative_operator for scalar
 * variables.
 */
template <base::concepts::real_scalar_dense_vector Variable,
    concepts::differentiable_rbf RBF, index_type DifferentiatedDimension>
struct operator_evaluator<
    partial_derivative_operator<Variable, DifferentiatedDimension>, RBF,
    distance_functions::euclidean_distance_function<Variable>>
    : general_differential_operator_evaluator<
          operator_evaluator<
              partial_derivative_operator<Variable, DifferentiatedDimension>,
              RBF, distance_functions::euclidean_distance_function<Variable>>,
          partial_derivative_operator<Variable, DifferentiatedDimension>, RBF,
          distance_functions::euclidean_distance_function<Variable>> {
    //! Type of the base.
    using base_type = general_differential_operator_evaluator<
        operator_evaluator<
            partial_derivative_operator<Variable, DifferentiatedDimension>, RBF,
            distance_functions::euclidean_distance_function<Variable>>,
        partial_derivative_operator<Variable, DifferentiatedDimension>, RBF,
        distance_functions::euclidean_distance_function<Variable>>;

    using base_type::variable_dimensions;
    using typename base_type::distance_function_type;
    using typename base_type::kernel_value_type;
    using typename base_type::operator_type;
    using typename base_type::rbf_type;
    using typename base_type::variable_type;

    /*!
     * \brief Get the orders of differentiations.
     *
     * \return Orders of differentiations.
     */
    [[nodiscard]] static auto differentiations()
        -> std::array<Eigen::Vector<int, variable_dimensions>, 1> {
        std::array<Eigen::Vector<int, variable_dimensions>, 1> orders_list;
        Eigen::Vector<int, variable_dimensions> orders =
            Eigen::Vector<int, variable_dimensions>::Zero();
        orders(DifferentiatedDimension) = 1;
        orders_list[0] = orders;
        return orders_list;
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

        const index_type dimension = DifferentiatedDimension;

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
};

}  // namespace num_collect::rbf::operators
