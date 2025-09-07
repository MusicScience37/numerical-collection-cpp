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
 * \brief Definition of thin_plate_spline_interpolator class type.
 */
#pragma once

#include <algorithm>

#include "num_collect/base/get_compile_time_size.h"
#include "num_collect/rbf/impl/get_default_scalar_type.h"
#include "num_collect/rbf/impl/get_variable_type.h"
#include "num_collect/rbf/rbf_polynomial_interpolator.h"
#include "num_collect/rbf/rbfs/thin_plate_spline_rbf.h"

namespace num_collect::rbf {

/*!
 * \brief Class to interpolate using thin plate spline \cite Ghosh2010.
 *
 * \tparam FunctionSignature Signature of the function to interpolate.
 * (Example: `double(double)`, `double(Eigen::Vector3d)`, ...)
 * \tparam Degree Number of differentiations.
 *
 * Thin plate splines uses a function space defined by the following inner
 * product of \f$ n \f$-th derivatives:
 * \f[
 * \sum_{\alpha_1 + \cdots + \alpha_d = n} \frac{n!}{\alpha_1! \cdots \alpha_d!}
 * \int_{-\infty}^{\infty} \cdots \int_{-\infty}^{\infty}
 * \frac{\partial^n f}{\partial x_1^{\alpha_1} \cdots \partial x_d^{\alpha_d}}
 * \frac{\partial^n g}{\partial x_1^{\alpha_1} \cdots \partial x_d^{\alpha_d}}
 * dx_1 \cdots dx_d
 * \f]
 * where functions \f$ f \f$ and \f$ g \f$
 * are from \f$ \mathbb{R}^d \f$ to \f$ \mathbb{R} \f$.
 *
 * This function space is a reproducing kernel Hilbert space
 * if \f$ 2n > d \f$.
 *
 * This class uses the reproducing kernel to interpolate functions.
 */
template <typename FunctionSignature,
    int Degree = std::max<index_type>(2,
        base::get_compile_time_size<
                impl::get_variable_type_t<FunctionSignature>>() /
                2 +
            1)>
    requires(Degree >= 1 &&
                2 * Degree >
                    base::get_compile_time_size<
                        impl::get_variable_type_t<FunctionSignature>>())
using thin_plate_spline_interpolator =
    rbf_polynomial_interpolator<FunctionSignature,
        rbfs::thin_plate_spline_rbf<
            impl::get_default_scalar_type<FunctionSignature>,
            base::get_compile_time_size<
                impl::get_variable_type_t<FunctionSignature>>(),
            Degree>,
        Degree - 1, kernel_matrix_type::dense,
        distance_functions::euclidean_distance_function<
            impl::get_variable_type_t<FunctionSignature>>,
        length_parameter_calculators::global_length_parameter_calculator<
            distance_functions::euclidean_distance_function<
                impl::get_variable_type_t<FunctionSignature>>>>;

}  // namespace num_collect::rbf
