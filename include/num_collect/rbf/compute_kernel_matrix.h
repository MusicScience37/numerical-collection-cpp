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
 * \brief Definition of compute_kernel_matrix function.
 */
#pragma once

#include <cstddef>
#include <type_traits>

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/concepts/sparse_matrix_of.h"
#include "num_collect/rbf/concepts/csrbf.h"
#include "num_collect/rbf/concepts/distance_function.h"
#include "num_collect/rbf/concepts/length_parameter_calculator.h"
#include "num_collect/rbf/concepts/rbf.h"
#include "num_collect/rbf/impl/compute_kernel_matrix_parallel.h"
#include "num_collect/rbf/impl/compute_kernel_matrix_serial.h"
#include "num_collect/rbf/impl/parallelized_num_points.h"
#include "num_collect/util/vector_view.h"

namespace num_collect::rbf {

/*!
 * \brief Compute a kernel matrix.
 *
 * \tparam DistanceFunction Type of the distance function.
 * \tparam RBF Type of the RBF.
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 * \tparam KernelMatrix Type of the kernel matrix.
 * \param[in] distance_function Distance function.
 * \param[in] rbf RBF.
 * \param[in,out] length_parameter_calculator Calculator of length parameters.
 * \param[in] variables Variables.
 * \param[out] kernel_matrix Kernel matrix.
 */
template <concepts::distance_function DistanceFunction, concepts::rbf RBF,
    concepts::length_parameter_calculator LengthParameterCalculator,
    base::concepts::dense_matrix KernelMatrix>
    requires std::is_same_v<
                 typename LengthParameterCalculator::distance_function_type,
                 DistanceFunction> &&
    std::is_same_v<typename DistanceFunction::value_type,
        typename RBF::scalar_type> &&
    std::is_same_v<typename DistanceFunction::value_type,
        typename KernelMatrix::Scalar> &&
    LengthParameterCalculator::uses_global_length_parameter
inline void compute_kernel_matrix(const DistanceFunction& distance_function,
    const RBF& rbf, LengthParameterCalculator& length_parameter_calculator,
    util::vector_view<const typename DistanceFunction::variable_type> variables,
    KernelMatrix& kernel_matrix) {
    if (variables.size() < impl::parallelized_num_points) {
        impl::compute_kernel_matrix_serial(distance_function, rbf,
            length_parameter_calculator, variables, kernel_matrix);
    } else {
        impl::compute_kernel_matrix_parallel(distance_function, rbf,
            length_parameter_calculator, variables, kernel_matrix);
    }
}

/*!
 * \brief Compute a kernel matrix.
 *
 * \tparam DistanceFunction Type of the distance function.
 * \tparam RBF Type of the RBF.
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 * \tparam KernelMatrix Type of the kernel matrix.
 * \param[in] distance_function Distance function.
 * \param[in] rbf RBF.
 * \param[in,out] length_parameter_calculator Calculator of length parameters.
 * \param[in] variables Variables.
 * \param[out] kernel_matrix Kernel matrix.
 */
template <concepts::distance_function DistanceFunction, concepts::rbf RBF,
    concepts::length_parameter_calculator LengthParameterCalculator,
    base::concepts::dense_matrix KernelMatrix>
    requires std::is_same_v<
                 typename LengthParameterCalculator::distance_function_type,
                 DistanceFunction> &&
    std::is_same_v<typename DistanceFunction::value_type,
        typename RBF::scalar_type> &&
    std::is_same_v<typename DistanceFunction::value_type,
        typename KernelMatrix::Scalar> &&
    (!LengthParameterCalculator::uses_global_length_parameter)
inline void compute_kernel_matrix(const DistanceFunction& distance_function,
    const RBF& rbf, LengthParameterCalculator& length_parameter_calculator,
    util::vector_view<const typename DistanceFunction::variable_type> variables,
    KernelMatrix& kernel_matrix) {
    if (variables.size() < impl::parallelized_num_points) {
        impl::compute_kernel_matrix_serial(distance_function, rbf,
            length_parameter_calculator, variables, kernel_matrix);
    } else {
        impl::compute_kernel_matrix_parallel(distance_function, rbf,
            length_parameter_calculator, variables, kernel_matrix);
    }
}

/*!
 * \brief Compute a kernel matrix.
 *
 * \tparam DistanceFunction Type of the distance function.
 * \tparam RBF Type of the RBF.
 * \tparam LengthParameterCalculator Type of the calculator of length
 * parameters.
 * \tparam KernelMatrix Type of the kernel matrix.
 * \param[in] distance_function Distance function.
 * \param[in] rbf RBF.
 * \param[in,out] length_parameter_calculator Calculator of length parameters.
 * \param[in] variables Variables.
 * \param[out] kernel_matrix Kernel matrix.
 */
template <concepts::distance_function DistanceFunction, concepts::csrbf RBF,
    concepts::length_parameter_calculator LengthParameterCalculator,
    base::concepts::sparse_matrix_of<typename DistanceFunction::value_type>
        KernelMatrix>
    requires std::is_same_v<
                 typename LengthParameterCalculator::distance_function_type,
                 DistanceFunction> &&
    std::is_same_v<typename DistanceFunction::value_type,
        typename RBF::scalar_type> &&
    std::is_same_v<typename DistanceFunction::value_type,
        typename KernelMatrix::Scalar>
inline void compute_kernel_matrix(const DistanceFunction& distance_function,
    const RBF& rbf, LengthParameterCalculator& length_parameter_calculator,
    util::vector_view<const typename DistanceFunction::variable_type> variables,
    KernelMatrix& kernel_matrix) {
    if (variables.size() < impl::parallelized_num_points) {
        impl::compute_kernel_matrix_serial(distance_function, rbf,
            length_parameter_calculator, variables, kernel_matrix);
    } else {
        impl::compute_kernel_matrix_parallel(distance_function, rbf,
            length_parameter_calculator, variables, kernel_matrix);
    }
}

}  // namespace num_collect::rbf
