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
 * \brief Definition of local_exact_rbf_interpolator class.
 */
#pragma once

#include <utility>
#include <vector>

#include <Eigen/Core>
#include <Eigen/LU>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/rbf/compute_kernel_matrix.h"
#include "num_collect/rbf/concepts/distance_function.h"  // IWYU pragma: keep
#include "num_collect/rbf/concepts/length_parameter_calculator.h"  // IWYU pragma: keep
#include "num_collect/rbf/concepts/rbf.h"  // IWYU pragma: keep
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/length_parameter_calculators/local_length_parameter_calculator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"

namespace num_collect::rbf {

//! Tag of local_exact_rbf_interpolator.
inline constexpr auto local_exact_rbf_interpolator_tag =
    logging::log_tag_view("num_collect::rbf::local_exact_rbf_interpolator");

/*!
 * \brief Class to interpolate using RBF using localized length parameter,
 * without regularization.
 *
 * \tparam Variable Type of variables.
 * \tparam FunctionValue Type of function values.
 * \tparam RBF Type of the RBF.
 * \tparam DistanceFunction Type of the distance function.
 */
template <typename Variable,
    typename FunctionValue = typename distance_functions::
        euclidean_distance_function<Variable>::value_type,
    concepts::rbf RBF = rbfs::gaussian_rbf<typename distance_functions::
            euclidean_distance_function<Variable>::value_type>,
    concepts::distance_function DistanceFunction =
        distance_functions::euclidean_distance_function<Variable>>
    requires std::is_same_v<typename DistanceFunction::value_type,
        typename RBF::scalar_type>
class local_exact_rbf_interpolator : public logging::logging_mixin {
public:
    //! Type of the distance function.
    using distance_function_type = DistanceFunction;

    //! Type of the RBF.
    using rbf_type = RBF;

    //! Type of the calculator of length parameters.
    using length_parameter_calculator_type =
        length_parameter_calculators::local_length_parameter_calculator<
            distance_function_type>;

    //! Type of variables.
    using variable_type = typename DistanceFunction::variable_type;

    //! Type of kernel values.
    using kernel_value_type = typename DistanceFunction::value_type;

    //! Type of kernel matrices.
    using kernel_matrix_type = Eigen::MatrixX<kernel_value_type>;

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
    explicit local_exact_rbf_interpolator(
        distance_function_type distance_function = distance_function_type(),
        rbf_type rbf = rbf_type())
        : logging::logging_mixin(local_exact_rbf_interpolator_tag),
          distance_function_(std::move(distance_function)),
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
        kernel_matrix_solver_.compute(kernel_matrix_);
        coeffs_ = kernel_matrix_solver_.solve(function_values);
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
     * \brief Set the scale of length parameters to a fixed value.
     *
     * \param[in] value Value.
     */
    void fix_length_parameter_scale(kernel_value_type value) {
        length_parameter_calculator_.scale(value);
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
    //! Distance function.
    distance_function_type distance_function_;

    //! RBF.
    rbf_type rbf_;

    //! Calculator of length parameters.
    length_parameter_calculator_type length_parameter_calculator_{};

    //! Kernel matrix.
    kernel_matrix_type kernel_matrix_{};

    //! Solver of the linear equation of kernel matrix.
    Eigen::PartialPivLU<kernel_matrix_type> kernel_matrix_solver_{};

    //! Coefficients for sample points.
    function_value_vector_type coeffs_{};
};

}  // namespace num_collect::rbf
