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
 * \brief Definition of rbf_polynomial_interpolator class.
 */
#pragma once

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/rbf/compute_kernel_matrix.h"
#include "num_collect/rbf/compute_polynomial_term_matrix.h"
#include "num_collect/rbf/concepts/distance_function.h"  // IWYU pragma: keep
#include "num_collect/rbf/concepts/length_parameter_calculator.h"  // IWYU pragma: keep
#include "num_collect/rbf/concepts/rbf.h"  // IWYU pragma: keep
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/impl/general_spline_equation_solver.h"
#include "num_collect/rbf/impl/get_default_scalar_type.h"
#include "num_collect/rbf/impl/get_variable_type.h"
#include "num_collect/rbf/kernel_matrix_type.h"
#include "num_collect/rbf/length_parameter_calculators/global_length_parameter_calculator.h"
#include "num_collect/rbf/length_parameter_calculators/local_length_parameter_calculator.h"
#include "num_collect/rbf/polynomial_calculator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"

namespace num_collect::rbf {

//! Tag of rbf_polynomial_interpolator.
inline constexpr auto rbf_polynomial_interpolator_tag =
    logging::log_tag_view("num_collect::rbf::rbf_polynomial_interpolator");

/*!
 * \brief Class to interpolate using RBF and polynomials.
 *
 * \tparam FunctionSignature Signature of the function to interpolate.
 * (Example: `double(double)`, `double(Eigen::Vector3d)`, ...)
 * \tparam RBF Type of the RBF.
 * \tparam PolynomialDegree Degree of the polynomial.
 * \tparam KernelMatrixType Type of kernel matrices.
 * \tparam DistanceFunction Type of the distance function.
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 */
template <typename FunctionSignature,
    concepts::rbf RBF =
        rbfs::gaussian_rbf<impl::get_default_scalar_type<FunctionSignature>>,
    index_type PolynomialDegree = 1,
    kernel_matrix_type KernelMatrixType = kernel_matrix_type::dense,
    concepts::distance_function DistanceFunction =
        distance_functions::euclidean_distance_function<
            impl::get_variable_type_t<FunctionSignature>>,
    concepts::length_parameter_calculator LengthParameterCalculator =
        length_parameter_calculators::local_length_parameter_calculator<
            DistanceFunction>>
class rbf_polynomial_interpolator;

/*!
 * \brief Class to interpolate using RBF and polynomials.
 *
 * \tparam Variable Type of variables.
 * \tparam FunctionValue Type of function values.
 * \tparam RBF Type of the RBF.
 * \tparam PolynomialDegree Degree of the polynomial.
 * \tparam KernelMatrixType Type of kernel matrices.
 * \tparam DistanceFunction Type of the distance function.
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 */
template <typename Variable, typename FunctionValue, concepts::rbf RBF,
    index_type PolynomialDegree, kernel_matrix_type KernelMatrixType,
    concepts::distance_function DistanceFunction,
    concepts::length_parameter_calculator LengthParameterCalculator>
class rbf_polynomial_interpolator<FunctionValue(Variable), RBF,
    PolynomialDegree, KernelMatrixType, DistanceFunction,
    LengthParameterCalculator> : public logging::logging_mixin {
public:
    //! Type of variables.
    using variable_type = Variable;

    //! Type of function values.
    using function_value_type = FunctionValue;

    //! Type of the RBF.
    using rbf_type = RBF;

    //! Type of the distance function.
    using distance_function_type = DistanceFunction;

    //! Type of the calculator of length parameters.
    using length_parameter_calculator_type = LengthParameterCalculator;

    //! Whether this calculator uses the globally fixed length parameters.
    static constexpr bool uses_global_length_parameter =
        length_parameter_calculator_type::uses_global_length_parameter;

    //! Type of kernel values.
    using kernel_value_type = typename DistanceFunction::value_type;

    //! Type of the solver of linear equations.
    using equation_solver_type =
        impl::general_spline_equation_solver<kernel_value_type,
            function_value_type, KernelMatrixType,
            uses_global_length_parameter>;

    //! Type of kernel matrices.
    using kernel_matrix_type =
        typename equation_solver_type::kernel_matrix_type;

    //! Type of matrices of polynomial terms.
    using polynomial_matrix_type =
        typename equation_solver_type::additional_matrix_type;

    //! Type of vectors of function values.
    using function_value_vector_type = Eigen::VectorX<function_value_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     */
    explicit rbf_polynomial_interpolator(
        distance_function_type distance_function = distance_function_type(),
        rbf_type rbf = rbf_type())
        : logging::logging_mixin(rbf_polynomial_interpolator_tag),
          distance_function_(std::move(distance_function)),
          rbf_(std::move(rbf)) {}

    /*!
     * \brief Compute parameters for interpolation.
     *
     * \param[in] variables Variables.
     * \param[in] function_values Function values.
     *
     * \note Pointer to the variables are saved internally,
     * so do not destruct it.
     */
    void compute(const std::vector<variable_type>& variables,
        const function_value_vector_type& function_values) {
        compute_kernel_matrix(distance_function_, rbf_,
            length_parameter_calculator_, variables, kernel_matrix_);
        compute_polynomial_term_matrix<PolynomialDegree>(
            variables, polynomial_matrix_);
        equation_solver_.compute(
            kernel_matrix_, polynomial_matrix_, function_values);
        equation_solver_.solve(kernel_coeffs_, polynomial_coeffs_, reg_param);
        variables_ = &variables;
    }

    /*!
     * \brief Interpolate for a variable.
     *
     * \param[in] variable Variable on which the function value is interpolated.
     * \return Interpolated value.
     */
    [[nodiscard]] auto interpolate(const variable_type& variable) const
        -> function_value_type {
        auto value = static_cast<function_value_type>(0);

        for (std::size_t i = 0; i < variables_->size(); ++i) {
            const kernel_value_type distance_rate =
                distance_function_(variable, (*variables_)[i]) /
                length_parameter_calculator_.length_parameter_at(
                    static_cast<index_type>(i));
            if constexpr (concepts::csrbf<rbf_type>) {
                if (distance_rate < rbf_type::support_boundary()) {
                    value += kernel_coeffs_(static_cast<index_type>(i)) *
                        rbf_(distance_rate);
                }
            } else {
                value += kernel_coeffs_(static_cast<index_type>(i)) *
                    rbf_(distance_rate);
            }
        }

        value += polynomial_calculator_.evaluate_polynomial_for_variable(
            variable, polynomial_coeffs_);

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
     * \brief Get the distance function.
     *
     * \return Distance function.
     */
    [[nodiscard]] auto distance_function() const noexcept
        -> const distance_function_type& {
        return distance_function_;
    }

    /*!
     * \brief Get the RBF.
     *
     * \return RBF.
     */
    [[nodiscard]] auto rbf() const noexcept -> const rbf_type& { return rbf_; }

    /*!
     * \brief Get the calculator of length parameters.
     *
     * \return Calculator of length parameters.
     */
    [[nodiscard]] auto length_parameter_calculator() const noexcept
        -> const length_parameter_calculator_type& {
        return length_parameter_calculator_;
    }

    /*!
     * \brief Get the coefficients for kernels.
     *
     * \return Coefficients.
     */
    [[nodiscard]] auto kernel_coeffs() const noexcept
        -> const function_value_vector_type& {
        return kernel_coeffs_;
    }

    /*!
     * \brief Get the coefficients for polynomials.
     *
     * \return Coefficients.
     */
    [[nodiscard]] auto polynomial_coeffs() const noexcept
        -> const function_value_vector_type& {
        return polynomial_coeffs_;
    }

private:
    //! Regularization parameter.
    static constexpr auto reg_param = static_cast<kernel_value_type>(0);

    // TODO Implementation of regularization.

    //! Distance function.
    distance_function_type distance_function_;

    //! RBF.
    rbf_type rbf_;

    //! Calculator of length parameters.
    length_parameter_calculator_type length_parameter_calculator_{};

    //! Calculator of polynomials.
    polynomial_calculator<variable_type, PolynomialDegree>
        polynomial_calculator_{};

    //! Kernel matrix.
    kernel_matrix_type kernel_matrix_{};

    //! Matrix of polynomial terms.
    polynomial_matrix_type polynomial_matrix_{};

    //! Variables.
    const std::vector<variable_type>* variables_{nullptr};

    //! Solver of linear equations.
    equation_solver_type equation_solver_{};

    //! Coefficients for kernels.
    function_value_vector_type kernel_coeffs_{};

    //! Coefficients for polynomials.
    function_value_vector_type polynomial_coeffs_{};
};

/*!
 * \brief Class to interpolate using RBF and polynomials with globally fixed
 * length parameters.
 *
 * \tparam FunctionSignature Signature of the function to interpolate.
 * (Example: `double(double)`, `double(Eigen::Vector3d)`, ...)
 * \tparam RBF Type of the RBF.
 * \tparam PolynomialDegree Degree of the polynomial.
 * \tparam KernelMatrixType Type of kernel matrices.
 * \tparam DistanceFunction Type of the distance function.
 */
template <typename FunctionSignature,
    concepts::rbf RBF =
        rbfs::gaussian_rbf<impl::get_default_scalar_type<FunctionSignature>>,
    index_type PolynomialDegree = 1,
    kernel_matrix_type KernelMatrixType = kernel_matrix_type::dense,
    concepts::distance_function DistanceFunction =
        distance_functions::euclidean_distance_function<
            impl::get_variable_type_t<FunctionSignature>>>
using global_rbf_polynomial_interpolator =
    rbf_polynomial_interpolator<FunctionSignature, RBF, PolynomialDegree,
        KernelMatrixType, DistanceFunction,
        length_parameter_calculators::global_length_parameter_calculator<
            DistanceFunction>>;

}  // namespace num_collect::rbf
