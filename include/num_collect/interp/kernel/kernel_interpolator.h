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
 * \brief Definition of kernel_interpolator class
 */
#pragma once

#include <vector>

#include "num_collect/interp/kernel/impl/auto_regularizer_wrapper.h"
#include "num_collect/interp/kernel/impl/kernel_parameter_optimizer.h"

namespace num_collect::interp::kernel {

/*!
 * \brief Class to interpolate data using kernels.
 *
 * \tparam Kernel Type of the kernel.
 */
template <typename Kernel>
class kernel_interpolator {
public:
    //! Type of the kernel.
    using kernel_type = Kernel;

    //! Type of variables.
    using variable_type = typename kernel_type::variable_type;

    //! Type of values in data.
    using value_type = typename kernel_type::value_type;

    //! Type of kernel parameters.
    using kernel_param_type = typename kernel_type::kernel_param_type;

    /*!
     * \brief Construct.
     *
     * \param[in] kernel Kernel.
     */
    explicit kernel_interpolator(const kernel_type& kernel = kernel_type())
        : kernel_(kernel) {
        search_kernel_param_auto();
    }

    /*!
     * \brief Set the regularization parameter.
     *
     * \param[in] reg_param Regularization parameter.
     * \return This.
     */
    auto regularize_with(const value_type& reg_param) -> kernel_interpolator& {
        interpolator_.regularize_with(reg_param);
        return *this;
    }

    /*!
     * \brief Set to determine the regularization parameter automatically.
     *
     * \return This.
     */
    auto regularize_automatically() -> kernel_interpolator& {
        interpolator_.regularize_automatically();
        return *this;
    }

    /*!
     * \brief Set to interpolate without regularization.
     *
     * \return This.
     */
    auto disable_regularization() -> kernel_interpolator& {
        regularize_with(static_cast<value_type>(0));
        return *this;
    }

    /*!
     * \brief Get the regularization parameter.
     *
     * \return Regularization parameter.
     */
    [[nodiscard]] auto reg_param() const -> value_type {
        return interpolator_.reg_param();
    }

    /*!
     * \brief Set kernel parameters to a fixed values.
     *
     * \param[in] kernel_param Kernel parameters.
     * \return This.
     */
    auto fix_kernel_param(const kernel_param_type& kernel_param)
        -> kernel_interpolator& {
        kernel_.kernel_param(kernel_param);
        optimizer_.reset();
        return *this;
    }

    /*!
     * \brief Set to determine kernel parameters automatically.
     *
     * \return This.
     */
    auto search_kernel_param_auto() -> kernel_interpolator& {
        if (!optimizer_) {
            optimizer_ =
                std::make_unique<impl::kernel_parameter_optimizer<kernel_type>>(
                    interpolator_, kernel_);
        }
        return *this;
    }

    /*!
     * \brief Get the kernel.
     *
     * \return Kernel.
     */
    [[nodiscard]] auto kernel() const -> const kernel_type& { return kernel_; }

    /*!
     * \brief Compute internal matrices.
     *
     * \param[in] variable_list List of variables.
     * \param[in] data Data vector.
     */
    template <typename Data>
    void compute(const std::vector<variable_type>& variable_list,
        const Eigen::MatrixBase<Data>& data) {
        NUM_COLLECT_ASSERT(
            static_cast<std::size_t>(data.rows()) == variable_list.size());
        NUM_COLLECT_ASSERT(data.cols() == 1);

        if (optimizer_) {
            optimizer_->compute(variable_list, data);
            kernel_.kernel_param(optimizer_->opt_param());
        }
        interpolator_.compute(kernel_, variable_list, data);
        interpolator_.solve(coeff_);

        variable_list_ = variable_list;
    }

    /*!
     * \brief Get the value of the MLE objective function.
     *
     * \return Value of the MLE objective function.
     */
    [[nodiscard]] auto mle_objective_function_value() const -> value_type {
        return interpolator_.mle_objective_function_value();
    }

    /*!
     * \brief Calculate the coefficient of the kernel common in variables.
     *
     * \return Value.
     */
    [[nodiscard]] auto common_coeff() const -> value_type {
        return interpolator_.common_coeff();
    }

    /*!
     * \brief Interpolate a value for a variable.
     *
     * \param[in] variable Variable.
     * \return Value.
     */
    [[nodiscard]] auto interpolate_on(const variable_type& variable) const
        -> value_type {
        auto value = static_cast<value_type>(0);
        for (std::size_t i = 0; i < variable_list_.size(); ++i) {
            value += kernel_(variable, variable_list_[i]) *
                coeff_(static_cast<index_type>(i));
        }
        return value;
    }

    /*!
     * \brief Interpolate a value for a variable.
     *
     * \param[in] variable Variable.
     * \return Value.
     */
    [[nodiscard]] auto operator()(const variable_type& variable) const {
        return interpolate_on(variable);
    }

private:
    //! Kernel.
    kernel_type kernel_;

    //! List of variables.
    std::vector<variable_type> variable_list_;

    //! Coefficients of kernels for variables.
    Eigen::VectorX<value_type> coeff_{};

    //! Internal interpolator.
    impl::auto_regularizer_wrapper<value_type> interpolator_{};

    //! Optimizer of kernel parameters.
    std::unique_ptr<impl::kernel_parameter_optimizer<kernel_type>> optimizer_{};
};

}  // namespace num_collect::interp::kernel
