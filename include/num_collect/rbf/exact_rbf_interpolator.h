/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of exact_rbf_interpolator class.
 */
#pragma once

#include <utility>
#include <vector>

#include <Eigen/Core>

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/compute_kernel_matrix.h"
#include "num_collect/rbf/concepts/distance_function.h"  // IWYU pragma: keep
#include "num_collect/rbf/concepts/length_parameter_calculator.h"  // IWYU pragma: keep
#include "num_collect/rbf/concepts/rbf.h"  // IWYU pragma: keep
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/length_parameter_calculators/global_length_parameter_calculator.h"
#include "num_collect/rbf/symmetric_kernel_matrix_solver.h"

namespace num_collect::rbf {

/*!
 * \brief Class to interpolate using RBF without regularization.
 *
 * \tparam RBF Type of the RBF.
 * \tparam DistanceFunction Type of the distance function.
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 * \tparam KernelMatrixSolver Type of the solver of the linear equation of
 * kernel matrix.
 * \tparam DistanceFunction::value_type Type of function values.
 */
template <concepts::rbf RBF, concepts::distance_function DistanceFunction,
    concepts::length_parameter_calculator LengthParameterCalculator,
    typename KernelMatrixSolver,
    typename FunctionValue = typename DistanceFunction::value_type>
    requires std::is_same_v<
                 typename LengthParameterCalculator::distance_function_type,
                 DistanceFunction> &&
    std::is_same_v<typename DistanceFunction::value_type,
        typename RBF::scalar_type> &&
    std::is_same_v<typename DistanceFunction::value_type,
        typename KernelMatrixSolver::scalar_type>
class exact_rbf_interpolator {
public:
    //! Type of the distance function.
    using distance_function_type = DistanceFunction;

    //! Type of the RBF.
    using rbf_type = RBF;

    //! Type of the calculator of length parameters.
    using length_parameter_calculator_type = LengthParameterCalculator;

    //! Type of variables.
    using variable_type = typename DistanceFunction::variable_type;

    //! Type of kernel values.
    using kernel_value_type = typename DistanceFunction::value_type;

    //! Type of kernel matrices.
    using kernel_matrix_type = Eigen::MatrixX<kernel_value_type>;

    //! Type of the solver of the linear equation of kernel matrix.
    using kernel_matrix_solver_type = KernelMatrixSolver;

    //! Type of function values.
    using function_value_type = FunctionValue;

    //! Type of vectors of function values.
    using function_value_vector_type = Eigen::VectorX<function_value_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     */
    explicit exact_rbf_interpolator(
        distance_function_type distance_function = distance_function_type(),
        rbf_type rbf = rbf_type())
        : distance_function_(std::move(distance_function)),
          rbf_(std::move(rbf)) {}

    /*!
     * \brief Compute parameters for interpolation.
     *
     * \param[in] variables Variables.
     * \param[in] function_values Function values.
     */
    void compute(const std::vector<variable_type>& variables,
        const function_value_vector_type& function_values) {
        compute_kernel_matrix(distance_function_, rbf_,
            length_parameter_calculator_, variables, kernel_matrix_);
        kernel_matrix_solver_.compute(kernel_matrix_, function_values);
        kernel_matrix_solver_.solve(coeffs_, reg_param);
        common_coeff_ = kernel_matrix_solver_.calc_common_coeff(reg_param);
    }

    /*!
     * \brief Interpolate for a variable.
     *
     * \param[in] variable Variable on which the function value is interpolated.
     * \param[in] variables_for_kernel Variables used in compute() function.
     * \return Interpolated value.
     *
     * \note If variables_for_kernel is different from variables given in
     * compute() function, the behaivior is undefined.
     */
    [[nodiscard]] auto interpolate(const variable_type& variable,
        const std::vector<variable_type>& variables_for_kernel) const
        -> function_value_type {
        auto value = static_cast<function_value_type>(0);
        for (std::size_t i = 0; i < variables_for_kernel.size(); ++i) {
            value += coeffs_(static_cast<index_type>(i)) *
                rbf_(distance_function_(variable, variables_for_kernel[i]) /
                    length_parameter_calculator_.length_parameter_at(
                        static_cast<index_type>(i)));
        }
        return value;
    }

    /*!
     * \brief Evaluate mean and variance in the gaussian process for a variable.
     *
     * \param[in] variable Variable on which the function value is interpolated.
     * \param[in] variables_for_kernel Variables used in compute() function.
     * \return Mean and variance.
     *
     * \note If variables_for_kernel is different from variables given in
     * compute() function, the behaivior is undefined.
     */
    [[nodiscard]] auto evaluate_mean_and_variance_on(
        const variable_type& variable,
        const std::vector<variable_type>& variables_for_kernel) const
        -> std::pair<function_value_type, function_value_type>
        requires length_parameter_calculator_type::uses_global_length_parameter
    {
        Eigen::VectorXd kernel_vec;
        kernel_vec.resize(static_cast<index_type>(variables_for_kernel.size()));
        for (std::size_t i = 0; i < variables_for_kernel.size(); ++i) {
            kernel_vec(static_cast<index_type>(i)) =
                rbf_(distance_function_(variable, variables_for_kernel[i]) /
                    length_parameter_calculator_.length_parameter_at(
                        static_cast<index_type>(i)));
        }

        const function_value_type mean = kernel_vec.dot(coeffs_);
        const function_value_type center_rbf_value =
            rbf_(static_cast<kernel_value_type>(0));
        const function_value_type variance = common_coeff_ *
            std::max<function_value_type>(center_rbf_value -
                    kernel_matrix_solver_.calc_reg_term(kernel_vec, reg_param),
                static_cast<function_value_type>(0));
        return {mean, variance};
    }

    /*!
     * \brief Get the coefficients for samples points.
     *
     * \return Coefficients.
     */
    [[nodiscard]] auto coeffs() const noexcept
        -> const function_value_vector_type& {
        return coeffs_;
    }

private:
    //! Regularization parameter.
    static constexpr auto reg_param = static_cast<kernel_value_type>(0);

    //! Distance function.
    distance_function_type distance_function_;

    //! RBF.
    rbf_type rbf_;

    //! Calculator of length parameters.
    length_parameter_calculator_type length_parameter_calculator_{};

    //! Kernel matrix.
    kernel_matrix_type kernel_matrix_{};

    //! Solver of the linear equation of kernel matrix.
    kernel_matrix_solver_type kernel_matrix_solver_{};

    //! Coefficients for sample points.
    function_value_vector_type coeffs_{};

    //! Common coefficients for RBF.
    function_value_type common_coeff_{};
};

/*!
 * \brief Class to interpolate using RBF without regularization.
 *
 * \tparam RBF Type of the RBF.
 * \tparam Variable Type of variables.
 * \tparam FunctionValue Type of function values.
 * \tparam DistanceFunction Type of the distance function.
 */
template <concepts::rbf RBF, typename Variable,
    typename FunctionValue = typename RBF::scalar_type,
    concepts::distance_function DistanceFunction =
        distance_functions::euclidean_distance_function<Variable>>
using global_exact_rbf_interpolator =
    exact_rbf_interpolator<RBF, DistanceFunction,
        length_parameter_calculators::global_length_parameter_calculator<
            DistanceFunction>,
        symmetric_kernel_matrix_solver<
            Eigen::MatrixX<typename DistanceFunction::value_type>,
            Eigen::VectorX<FunctionValue>>>;

}  // namespace num_collect::rbf
