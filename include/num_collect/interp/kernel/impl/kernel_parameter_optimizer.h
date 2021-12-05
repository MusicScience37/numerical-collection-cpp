/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of kernel_parameter_optimizer class.
 */
#pragma once

#include <Eigen/Core>
#include <functional>
#include <memory>

#include "num_collect/interp/kernel/impl/auto_regularizer_wrapper.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/function_object_wrapper.h"

namespace num_collect::interp::kernel::impl {

/*!
 * \brief Class to determine kernel parameters.
 *
 * \tparam Kernel Type of the kernel.
 */
template <typename Kernel>
class kernel_parameter_optimizer {
public:
    //! Type of the kernel.
    using kernel_type = Kernel;

    //! Type of values in data.
    using value_type = typename kernel_type::value_type;

    //! Type of kernel parameters.
    using kernel_param_type = typename kernel_type::kernel_param_type;

    //! Signature of objective function.
    using objective_function_signature = value_type(kernel_param_type);

    /*!
     * \brief Construct.
     *
     * \param[in] interpolator Interpolator.
     * \param[in] kernel Kernel.
     */
    kernel_parameter_optimizer(
        auto_regularizer_wrapper<value_type>& interpolator, kernel_type& kernel)
        : interpolator_(&interpolator), kernel_(&kernel) {
        constexpr index_type default_max_evaluations = 10;
        optimizer_.max_evaluations(default_max_evaluations);
    }

    /*!
     * \brief Compute internal matrices.
     *
     * \tparam Container Type of the list of variables.
     * \tparam Data Type of the data vector.
     * \param[in] variable_list List of variables.
     * \param[in] data Data vector.
     */
    template <typename Container, typename Data>
    void compute(
        const Container& variable_list, const Eigen::MatrixBase<Data>& data) {
        optimizer_ = opt::dividing_rectangles<
            opt::function_object_wrapper<objective_function_signature,
                std::function<objective_function_signature>>>(
            opt::make_function_object_wrapper<objective_function_signature>(
                std::function<objective_function_signature>(
                    [this, &variable_list, &data](
                        const kernel_param_type& param) {
                        kernel_->kernel_param(param);
                        interpolator_->compute(*kernel_, variable_list, data);
                        return interpolator_->mle_objective_function_value();
                    })));

        const auto [lower, upper] =
            kernel_->kernel_param_search_region(variable_list);
        optimizer_.init(lower, upper);

        optimizer_.solve();
    }

    /*!
     * \brief Get the optimal parameter.
     *
     * \return Optimal parameter.
     */
    [[nodiscard]] auto opt_param() const -> const kernel_param_type& {
        return optimizer_.opt_variable();
    }

private:
    //! Interpolator.
    auto_regularizer_wrapper<value_type>* interpolator_;

    //! Kernel.
    kernel_type* kernel_;

    //! Optimizer.
    opt::dividing_rectangles<
        opt::function_object_wrapper<objective_function_signature,
            std::function<objective_function_signature>>>
        optimizer_{
            opt::make_function_object_wrapper<objective_function_signature>(
                std::function<objective_function_signature>())};
};

}  // namespace num_collect::interp::kernel::impl
