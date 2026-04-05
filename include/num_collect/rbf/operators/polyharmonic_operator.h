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
 * \brief Definition of polyharmonic_operator class.
 */
#pragma once

#include <cmath>
#include <utility>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/operators/general_differential_operator_evaluator.h"
#include "num_collect/rbf/operators/impl/generate_polyharmonic_operator_differentiations.h"
#include "num_collect/rbf/operators/operator_base.h"
#include "num_collect/rbf/operators/operator_evaluator.h"
#include "num_collect/rbf/rbfs/polyharmonic_spline_rbf.h"

namespace num_collect::rbf::operators {

/*!
 * \brief Class of operators to evaluate polyharmonic operators
 * \f$ \triangle^m \f$.
 *
 * \tparam PolyharmonicOrder Order of the polyharmonic operator.
 * \tparam Variable Type of variables.
 */
template <int PolyharmonicOrder, typename Variable>
    requires(base::concepts::real_scalar<Variable> ||
                base::concepts::real_scalar_dense_vector<Variable>) &&
    (PolyharmonicOrder > 0)
class polyharmonic_operator
    : public operator_base<polyharmonic_operator<PolyharmonicOrder, Variable>> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] variable Variable to evaluate the polyharmonic operator at.
     */
    explicit polyharmonic_operator(Variable variable)
        : variable_(std::move(variable)) {}

    /*!
     * \brief Get the variable to evaluate the polyharmonic operator at.
     *
     * \return Variable.
     */
    [[nodiscard]] auto variable() const noexcept -> const Variable& {
        return variable_;
    }

private:
    //! Variable to evaluate the polyharmonic operator at.
    Variable variable_;
};

/*!
 * \brief Specialization of num_collect::rbf::operators::operator_evaluator for
 * num_collect::rbf::operators::polyharmonic_operator
 * with polyharmonic spline RBFs of odd degrees and Euclidean distance function.
 */
template <int PolyharmonicOrder, typename Variable,
    base::concepts::real_scalar Scalar, index_type Degree>
    requires(static_cast<int>(Degree) - 2 * PolyharmonicOrder >= 0) &&
    (Degree % 2 == 1)
struct operator_evaluator<polyharmonic_operator<PolyharmonicOrder, Variable>,
    rbfs::polyharmonic_spline_rbf<Scalar, Degree>,
    distance_functions::euclidean_distance_function<Variable>>
    : general_differential_operator_evaluator<
          operator_evaluator<polyharmonic_operator<PolyharmonicOrder, Variable>,
              rbfs::polyharmonic_spline_rbf<Scalar, Degree>,
              distance_functions::euclidean_distance_function<Variable>>,
          polyharmonic_operator<PolyharmonicOrder, Variable>,
          rbfs::polyharmonic_spline_rbf<Scalar, Degree>,
          distance_functions::euclidean_distance_function<Variable>> {
    //! Type of the base.
    using base_type = general_differential_operator_evaluator<
        operator_evaluator<polyharmonic_operator<PolyharmonicOrder, Variable>,
            rbfs::polyharmonic_spline_rbf<Scalar, Degree>,
            distance_functions::euclidean_distance_function<Variable>>,
        polyharmonic_operator<PolyharmonicOrder, Variable>,
        rbfs::polyharmonic_spline_rbf<Scalar, Degree>,
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
            functions::pow(variable_dimensions, PolyharmonicOrder)> {
        return impl::generate_polyharmonic_operator_differentiations<
            PolyharmonicOrder, variable_dimensions>();
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

        constexpr FunctionValue differentiation_coeff =
            calculate_differentiation_coeff();
        constexpr int degree_after_diff =
            static_cast<int>(Degree) - 2 * PolyharmonicOrder;

        using std::pow;
        return kernel_coeff * differentiation_coeff *
            pow(distance_rate, degree_after_diff) *
            pow(length_parameter, -2 * PolyharmonicOrder);
    }

private:
    /*!
     * \brief Calculate the coefficient of the derivative of RBF.
     *
     * \return Coefficient of the derivative of RBF.
     */
    [[nodiscard]] static constexpr auto
    calculate_differentiation_coeff() noexcept -> Scalar {
        auto coeff = static_cast<Scalar>(1);
        int current_degree = static_cast<int>(Degree);
        for (int i = 0; i < PolyharmonicOrder; ++i) {
            coeff *= static_cast<Scalar>(current_degree) *
                static_cast<Scalar>(current_degree + variable_dimensions - 2);
            current_degree -= 2;
        }
        return coeff;
    }
};

}  // namespace num_collect::rbf::operators
