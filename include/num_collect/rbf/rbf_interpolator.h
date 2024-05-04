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
 * \brief Definition of rbf_interpolator class.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/function_object_wrapper.h"
#include "num_collect/rbf/compute_kernel_matrix.h"
#include "num_collect/rbf/concepts/distance_function.h"  // IWYU pragma: keep
#include "num_collect/rbf/concepts/length_parameter_calculator.h"  // IWYU pragma: keep
#include "num_collect/rbf/concepts/rbf.h"  // IWYU pragma: keep
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/impl/kernel_matrix_solver.h"
#include "num_collect/rbf/kernel_matrix_type.h"
#include "num_collect/rbf/length_parameter_calculators/global_length_parameter_calculator.h"
#include "num_collect/rbf/length_parameter_calculators/local_length_parameter_calculator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/rbf/rbfs/wendland_csrbf.h"

namespace num_collect::rbf {

//! Tag of rbf_interpolator.
inline constexpr auto rbf_interpolator_tag =
    logging::log_tag_view("num_collect::rbf::rbf_interpolator");

namespace impl {

/*!
 * \brief Get the type of variables from function signature.
 *
 * \tparam FunctionSignature Function signature.
 */
template <typename FunctionSignature>
struct get_variable_type;

/*!
 * \brief Get the type of variables from function signature.
 *
 * \tparam Variable Type of variables.
 * \tparam FunctionValue Type of function values.
 */
template <typename Variable, typename FunctionValue>
struct get_variable_type<FunctionValue(Variable)> {
    //! Type.
    using type = Variable;
};

/*!
 * \brief Get the type of variables from function signature.
 *
 * \tparam FunctionSignature Function signature.
 */
template <typename FunctionSignature>
using get_variable_type_t = typename get_variable_type<FunctionSignature>::type;

/*!
 * \brief Get the default type of scalars from function signature.
 *
 * \tparam FunctionSignature Function signature.
 */
template <typename FunctionSignature>
using get_default_scalar_type =
    typename distance_functions::euclidean_distance_function<
        impl::get_variable_type_t<FunctionSignature>>::value_type;

}  // namespace impl

/*!
 * \brief Class to interpolate using RBF.
 *
 * \tparam FunctionSignature Signature of the function to interpolate.
 * (Example: `double(double)`, `double(Eigen::Vector3d)`, ...)
 * \tparam RBF Type of the RBF.
 * \tparam KernelMatrixType Type of kernel matrices.
 * \tparam DistanceFunction Type of the distance function.
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 */
template <typename FunctionSignature,
    concepts::rbf RBF =
        rbfs::gaussian_rbf<impl::get_default_scalar_type<FunctionSignature>>,
    kernel_matrix_type KernelMatrixType = kernel_matrix_type::dense,
    concepts::distance_function DistanceFunction =
        distance_functions::euclidean_distance_function<
            impl::get_variable_type_t<FunctionSignature>>,
    concepts::length_parameter_calculator LengthParameterCalculator =
        length_parameter_calculators::local_length_parameter_calculator<
            DistanceFunction>>
class rbf_interpolator;

/*!
 * \brief Class to interpolate using RBF.
 *
 * \tparam Variable Type of variables.
 * \tparam FunctionValue Type of function values.
 * \tparam RBF Type of the RBF.
 * \tparam KernelMatrixType Type of kernel matrices.
 * \tparam DistanceFunction Type of the distance function.
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 */
template <typename Variable, typename FunctionValue, concepts::rbf RBF,
    kernel_matrix_type KernelMatrixType,
    concepts::distance_function DistanceFunction,
    concepts::length_parameter_calculator LengthParameterCalculator>
    requires std::is_same_v<Variable,
                 typename DistanceFunction::variable_type> &&
    std::is_same_v<typename DistanceFunction::value_type,
        typename RBF::scalar_type> &&
    std::is_same_v<DistanceFunction,
        typename LengthParameterCalculator::distance_function_type>
class rbf_interpolator<FunctionValue(Variable), RBF, KernelMatrixType,
    DistanceFunction, LengthParameterCalculator>
    : public logging::logging_mixin {
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

    //! Type of the solver of the linear equation of kernel matrix.
    using kernel_matrix_solver_type =
        impl::kernel_matrix_solver<kernel_value_type, function_value_type,
            KernelMatrixType, uses_global_length_parameter>;

    //! Type of kernel matrices.
    using kernel_matrix_type =
        typename kernel_matrix_solver_type::kernel_matrix_type;

    //! Type of vectors of function values.
    using function_value_vector_type = Eigen::VectorX<function_value_type>;

    //! Default value of maximum number of evaluations of objective functions in MLE.
    static constexpr index_type default_max_mle_evaluations = 20;

    /*!
     * \brief Constructor.
     *
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     */
    explicit rbf_interpolator(
        distance_function_type distance_function = distance_function_type(),
        rbf_type rbf = rbf_type())
        : logging::logging_mixin(rbf_interpolator_tag),
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
        kernel_matrix_solver_.compute(kernel_matrix_, function_values);
        kernel_matrix_solver_.solve(coeffs_, reg_param, function_values);
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
            value += coeffs_(static_cast<index_type>(i)) *
                rbf_(distance_function_(variable, (*variables_)[i]) /
                    length_parameter_calculator_.length_parameter_at(
                        static_cast<index_type>(i)));
        }
        // TODO optimization for CSRBF.
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
     * \brief Set the scale of length parameters with optimization using MLE
     * \cite Scheuerer2011.
     *
     * \param[in] variables Variables.
     * \param[in] function_values Function values.
     * \param[in] max_mle_evaluations Maximum number of evaluations of objective
     * functions in MLE.
     *
     * \note After call of this function, call compute() for calculation of
     * internal parameter.
     */
    void optimize_length_parameter_scale(
        const std::vector<variable_type>& variables,
        const function_value_vector_type& function_values,
        index_type max_mle_evaluations = default_max_mle_evaluations)
        requires uses_global_length_parameter
    {
        static constexpr auto base = static_cast<kernel_value_type>(10);

        auto objective_function =
            [this, &variables, &function_values](
                kernel_value_type log_scale) -> kernel_value_type {
            const kernel_value_type scale = std::pow(base, log_scale);
            this->length_parameter_calculator_.scale(scale);
            compute_kernel_matrix(this->distance_function_, this->rbf_,
                this->length_parameter_calculator_, variables,
                this->kernel_matrix_);
            this->kernel_matrix_solver_.compute(
                this->kernel_matrix_, function_values);
            return std::log10(
                this->kernel_matrix_solver_.calc_mle_objective(reg_param));
        };

        using objective_function_object_type = decltype(objective_function);
        using objective_function_wrapper_type =
            opt::function_object_wrapper<kernel_value_type(kernel_value_type),
                objective_function_object_type>;
        using optimizer_type =
            opt::dividing_rectangles<objective_function_wrapper_type>;

        optimizer_type optimizer{
            objective_function_wrapper_type{objective_function}};
        configure_child_algorithm_logger_if_exists(optimizer);
        constexpr auto lower_boundary = static_cast<kernel_value_type>(-1);
        constexpr auto upper_boundary = static_cast<kernel_value_type>(2);
        optimizer.max_evaluations(max_mle_evaluations);
        optimizer.init(lower_boundary, upper_boundary);
        optimizer.solve();

        const kernel_value_type log_scale = optimizer.opt_variable();
        const kernel_value_type scale = std::pow(base, log_scale);
        this->logger().debug()(
            "Selected an optimized scale of length parameters: {}", scale);
        this->length_parameter_calculator_.scale(scale);
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

    // TODO Implementation of regularization.

    //! Distance function.
    distance_function_type distance_function_;

    //! RBF.
    rbf_type rbf_;

    //! Calculator of length parameters.
    length_parameter_calculator_type length_parameter_calculator_{};

    //! Kernel matrix.
    kernel_matrix_type kernel_matrix_{};

    //! Variables.
    const std::vector<variable_type>* variables_{nullptr};

    //! Solver of the linear equation of kernel matrix.
    kernel_matrix_solver_type kernel_matrix_solver_{};

    //! Coefficients for sample points.
    function_value_vector_type coeffs_{};
};

/*!
 * \brief Class to interpolate using RBF with length parameters localized for
 * sample points.
 *
 * \tparam FunctionSignature Signature of the function to interpolate.
 * (Example: `double(double)`, `double(Eigen::Vector3d)`, ...)
 * \tparam RBF Type of the RBF.
 * \tparam KernelMatrixType Type of kernel matrices.
 * \tparam DistanceFunction Type of the distance function.
 */
template <typename FunctionSignature,
    concepts::rbf RBF =
        rbfs::gaussian_rbf<impl::get_default_scalar_type<FunctionSignature>>,
    kernel_matrix_type KernelMatrixType = kernel_matrix_type::dense,
    concepts::distance_function DistanceFunction =
        distance_functions::euclidean_distance_function<
            impl::get_variable_type_t<FunctionSignature>>>
using local_rbf_interpolator =
    rbf_interpolator<FunctionSignature, RBF, KernelMatrixType, DistanceFunction,
        length_parameter_calculators::local_length_parameter_calculator<
            DistanceFunction>>;

/*!
 * \brief Class to interpolate using RBF with globally fixed length parameters.
 *
 * \tparam FunctionSignature Signature of the function to interpolate.
 * (Example: `double(double)`, `double(Eigen::Vector3d)`, ...)
 * \tparam RBF Type of the RBF.
 * \tparam KernelMatrixType Type of kernel matrices.
 * \tparam DistanceFunction Type of the distance function.
 */
template <typename FunctionSignature,
    concepts::rbf RBF =
        rbfs::gaussian_rbf<impl::get_default_scalar_type<FunctionSignature>>,
    kernel_matrix_type KernelMatrixType = kernel_matrix_type::dense,
    concepts::distance_function DistanceFunction =
        distance_functions::euclidean_distance_function<
            impl::get_variable_type_t<FunctionSignature>>>
using global_rbf_interpolator =
    rbf_interpolator<FunctionSignature, RBF, KernelMatrixType, DistanceFunction,
        length_parameter_calculators::global_length_parameter_calculator<
            DistanceFunction>>;

/*!
 * \brief Class to interpolate using compactly supported RBF with length
 * parameters localized for sample points.
 *
 * \tparam FunctionSignature Signature of the function to interpolate.
 * (Example: `double(double)`, `double(Eigen::Vector3d)`, ...)
 * \tparam RBF Type of the RBF.
 * \tparam DistanceFunction Type of the distance function.
 */
template <typename FunctionSignature,
    concepts::rbf RBF = rbfs::wendland_csrbf<
        impl::get_default_scalar_type<FunctionSignature>, 3, 1>,
    concepts::distance_function DistanceFunction =
        distance_functions::euclidean_distance_function<
            impl::get_variable_type_t<FunctionSignature>>>
using local_csrbf_interpolator = rbf_interpolator<FunctionSignature, RBF,
    kernel_matrix_type::sparse, DistanceFunction,
    length_parameter_calculators::local_length_parameter_calculator<
        DistanceFunction>>;

}  // namespace num_collect::rbf
