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
 * \brief Definition of hessian_operator class.
 */
#pragma once

#include <utility>

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/rbf/concepts/csrbf.h"
#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/operators/general_operator_evaluator.h"
#include "num_collect/rbf/operators/operator_evaluator.h"
#include "num_collect/rbf/rbfs/differentiated.h"

namespace num_collect::rbf::operators {

/*!
 * \brief Class of operators to evaluate Hessian.
 *
 * \tparam Variable Type of variables.
 *
 * This class limits variables to vectors.
 * If you want to use scalars, use
 * \ref num_collect::rbf::operators::laplacian_operator.
 */
template <base::concepts::real_scalar_dense_vector Variable>
    requires(Variable::RowsAtCompileTime > 0)  // Prohibit dynamic vector.
class hessian_operator {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] variable Variable to evaluate the Hessian at.
     */
    explicit hessian_operator(Variable variable)
        : variable_(std::move(variable)) {}

    /*!
     * \brief Get the variable to evaluate the Hessian at.
     *
     * \return Variable.
     */
    [[nodiscard]] auto variable() const noexcept -> const Variable& {
        return variable_;
    }

private:
    //! Variable to evaluate the Hessian at.
    Variable variable_;
};

/*!
 * \brief Specialization of num_collect::rbf::operators::operator_evaluator for
 * num_collect::rbf::operators::hessian_operator for scalar
 * variables.
 */
template <base::concepts::real_scalar_dense_vector Variable, concepts::rbf RBF>
    requires(Variable::RowsAtCompileTime > 0)  // Prohibit dynamic vector.
struct operator_evaluator<hessian_operator<Variable>, RBF,
    distance_functions::euclidean_distance_function<Variable>>
    : general_operator_evaluator<
          operator_evaluator<hessian_operator<Variable>, RBF,
              distance_functions::euclidean_distance_function<Variable>>,
          hessian_operator<Variable>, RBF,
          distance_functions::euclidean_distance_function<Variable>> {
    //! Type of the base.
    using base_type = general_operator_evaluator<
        operator_evaluator<hessian_operator<Variable>, RBF,
            distance_functions::euclidean_distance_function<Variable>>,
        hessian_operator<Variable>, RBF,
        distance_functions::euclidean_distance_function<Variable>>;

    using typename base_type::distance_function_type;
    using typename base_type::kernel_value_type;
    using typename base_type::operator_type;
    using typename base_type::rbf_type;
    using typename base_type::variable_type;

    //! Size of the vectors.
    static constexpr index_type vector_size = Variable::RowsAtCompileTime;

    /*!
     * \brief Get the initial value for accumulation of values evaluated for
     * samples points.
     *
     * \tparam KernelCoeff Type of the coefficients of kernels.
     * \return Initial value.
     */
    template <typename KernelCoeff>
    [[nodiscard]] static auto initial_value()
        -> Eigen::Matrix<KernelCoeff, vector_size, vector_size> {
        return Eigen::Matrix<KernelCoeff, vector_size, vector_size>::Zero();
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
        -> Eigen::Matrix<FunctionValue, vector_size, vector_size> {
        // Not used for this operator.
        (void)rbf;

        NUM_COLLECT_PRECONDITION(
            length_parameter > static_cast<kernel_value_type>(0),
            "Length parameter must be a positive value.");
        const kernel_value_type distance_rate =
            distance_function(target_operator.variable(), sample_variable) /
            length_parameter;

        const rbfs::differentiated_t<rbf_type> differentiated_rbf;
        const rbfs::differentiated_t<rbfs::differentiated_t<rbf_type>>
            twice_differentiated_rbf;

        if constexpr (concepts::csrbf<rbf_type>) {
            if (distance_rate < rbf_type::support_boundary()) {
                const FunctionValue squared_length_parameter =
                    length_parameter * length_parameter;
                const Eigen::Vector<FunctionValue, vector_size> diff =
                    target_operator.variable() - sample_variable;

                return kernel_coeff / squared_length_parameter *
                    (twice_differentiated_rbf(distance_rate) /
                            squared_length_parameter * diff * diff.transpose() -
                        differentiated_rbf(distance_rate) *
                            Eigen::Matrix<FunctionValue, vector_size,
                                vector_size>::Identity());
            }
            return Eigen::Matrix<FunctionValue, vector_size,
                vector_size>::Zero();
        } else {
            const FunctionValue squared_length_parameter =
                length_parameter * length_parameter;
            const Eigen::Vector<FunctionValue, vector_size> diff =
                target_operator.variable() - sample_variable;

            return kernel_coeff / squared_length_parameter *
                (twice_differentiated_rbf(distance_rate) /
                        squared_length_parameter * diff * diff.transpose() -
                    differentiated_rbf(distance_rate) *
                        Eigen::Matrix<FunctionValue, vector_size,
                            vector_size>::Identity());
        }
    }
};

}  // namespace num_collect::rbf::operators
