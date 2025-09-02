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
 * \brief Definition of biharmonic_operator class.
 */
#pragma once

#include <utility>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/get_size.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/rbf/concepts/csrbf.h"
#include "num_collect/rbf/concepts/fourth_order_differentiable_rbf.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/operators/general_differential_operator_evaluator.h"
#include "num_collect/rbf/operators/operator_evaluator.h"
#include "num_collect/rbf/rbfs/differentiated.h"

namespace num_collect::rbf::operators {

/*!
 * \brief Class of operators to evaluate biharmonic operators
 * \f$ \triangle^2 \f$.
 *
 * \tparam Variable Type of variables.
 */
template <typename Variable>
    requires base::concepts::real_scalar<Variable> ||
    base::concepts::real_scalar_dense_vector<Variable>
class biharmonic_operator {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] variable Variable to evaluate the biharmonic operator at.
     */
    explicit biharmonic_operator(Variable variable)
        : variable_(std::move(variable)) {}

    /*!
     * \brief Get the variable to evaluate the biharmonic operator at.
     *
     * \return Variable.
     */
    [[nodiscard]] auto variable() const noexcept -> const Variable& {
        return variable_;
    }

private:
    //! Variable to evaluate the biharmonic operator at.
    Variable variable_;
};

/*!
 * \brief Specialization of num_collect::rbf::operators::operator_evaluator for
 * num_collect::rbf::operators::biharmonic_operator.
 */
template <typename Variable, concepts::fourth_order_differentiable_rbf RBF>
struct operator_evaluator<biharmonic_operator<Variable>, RBF,
    distance_functions::euclidean_distance_function<Variable>>
    : general_differential_operator_evaluator<
          operator_evaluator<biharmonic_operator<Variable>, RBF,
              distance_functions::euclidean_distance_function<Variable>>,
          biharmonic_operator<Variable>, RBF,
          distance_functions::euclidean_distance_function<Variable>> {
    //! Type of the base.
    using base_type = general_differential_operator_evaluator<
        operator_evaluator<biharmonic_operator<Variable>, RBF,
            distance_functions::euclidean_distance_function<Variable>>,
        biharmonic_operator<Variable>, RBF,
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
        -> std::array<Eigen::Vector<int, variable_dimensions>,
            variable_dimensions * variable_dimensions> {
        std::array<Eigen::Vector<int, variable_dimensions>,
            variable_dimensions * variable_dimensions>
            orders_list;
        Eigen::Vector<int, variable_dimensions> orders =
            Eigen::Vector<int, variable_dimensions>::Zero();
        for (int i = 0; i < variable_dimensions; ++i) {
            for (int j = 0; j < variable_dimensions; ++j) {
                Eigen::Vector<int, variable_dimensions> orders =
                    Eigen::Vector<int, variable_dimensions>::Zero();
                orders(i) += 2;
                orders(j) += 2;
                orders_list[static_cast<std::size_t>(
                    i * variable_dimensions + j)] = orders;
            }
        }
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

        const rbfs::differentiated_t<rbfs::differentiated_t<rbf_type>>
            second_differentiated_rbf;
        const rbfs::differentiated_t<
            rbfs::differentiated_t<rbfs::differentiated_t<rbf_type>>>
            third_differentiated_rbf;
        const rbfs::differentiated_t<rbfs::differentiated_t<
            rbfs::differentiated_t<rbfs::differentiated_t<rbf_type>>>>
            fourth_differentiated_rbf;

        const index_type dimension = get_size(target_operator.variable());

        if constexpr (concepts::csrbf<rbf_type>) {
            // TODO Case of CSRBF cannot be tested currently because
            // fourth-order differentiable CSRBF is not implemented in this
            // library.
            if (distance_rate < rbf_type::support_boundary()) {
                const FunctionValue squared_length_parameter =
                    length_parameter * length_parameter;
                const FunctionValue squared_distance_rate =
                    distance_rate * distance_rate;
                return kernel_coeff *
                    (fourth_differentiated_rbf(distance_rate) *
                            squared_distance_rate * squared_distance_rate -
                        third_differentiated_rbf(distance_rate) *
                            static_cast<FunctionValue>(2 * dimension + 4) *
                            squared_distance_rate +
                        second_differentiated_rbf(distance_rate) *
                            static_cast<FunctionValue>(
                                dimension * dimension + 2 * dimension)) /
                    (squared_length_parameter * squared_length_parameter);
            }
            return static_cast<FunctionValue>(0);
        } else {
            const FunctionValue squared_length_parameter =
                length_parameter * length_parameter;
            const FunctionValue squared_distance_rate =
                distance_rate * distance_rate;
            return kernel_coeff *
                (fourth_differentiated_rbf(distance_rate) *
                        squared_distance_rate * squared_distance_rate -
                    third_differentiated_rbf(distance_rate) *
                        static_cast<FunctionValue>(2 * dimension + 4) *
                        squared_distance_rate +
                    second_differentiated_rbf(distance_rate) *
                        static_cast<FunctionValue>(
                            dimension * dimension + 2 * dimension)) /
                (squared_length_parameter * squared_length_parameter);
        }
    }
};

}  // namespace num_collect::rbf::operators
