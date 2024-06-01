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

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include <Eigen/Core>

#include "num_collect/base/index_type.h"
#include "num_collect/interp/kernel/concepts/kernel.h"  // IWYU pragma: keep
#include "num_collect/interp/kernel/impl/kernel_coeff_solver.h"
#include "num_collect/logging/logger.h"

namespace num_collect::interp::kernel {

/*!
 * \brief Class to interpolate data using kernels.
 *
 * This class determines parameters automatically if set to do so
 * using maximum likelihood estimation (MLE) \cite Scheuerer2011.
 * And this class can evaluate mean and variance in Gaussian process as in
 * \cite Brochu2010.
 *
 * \tparam Kernel Type of the kernel.
 */
template <concepts::kernel Kernel>
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
     * \brief Constructor.
     *
     * \param[in] kernel Kernel.
     */
    explicit kernel_interpolator(const kernel_type& kernel = kernel_type())
        : solver_(kernel) {
        search_kernel_param_auto();
    }

    /*!
     * \brief Set the regularization parameter.
     *
     * \param[in] reg_param Regularization parameter.
     * \return This.
     */
    auto regularize_with(const value_type& reg_param) -> kernel_interpolator& {
        solver_.regularize_with(reg_param);
        return *this;
    }

    /*!
     * \brief Set to determine the regularization parameter automatically.
     *
     * \return This.
     */
    auto regularize_automatically() -> kernel_interpolator& {
        solver_.regularize_automatically();
        return *this;
    }

    /*!
     * \brief Set to interpolate without regularization.
     *
     * \return This.
     */
    auto disable_regularization() -> kernel_interpolator& {
        solver_.regularize_with(static_cast<value_type>(0));
        return *this;
    }

    /*!
     * \brief Get the regularization parameter.
     *
     * \return Regularization parameter.
     */
    [[nodiscard]] auto reg_param() const -> value_type {
        return solver_.reg_param();
    }

    /*!
     * \brief Set kernel parameters to a fixed values.
     *
     * \param[in] kernel_param Kernel parameters.
     * \return This.
     */
    auto fix_kernel_param(
        const kernel_param_type& kernel_param) -> kernel_interpolator& {
        solver_.fix_kernel_param(kernel_param);
        return *this;
    }

    /*!
     * \brief Set to determine kernel parameters automatically.
     *
     * \return This.
     */
    auto search_kernel_param_auto() -> kernel_interpolator& {
        solver_.search_kernel_param_auto();
        return *this;
    }

    /*!
     * \brief Get the kernel.
     *
     * \return Kernel.
     */
    [[nodiscard]] auto kernel() const -> const kernel_type& {
        return solver_.kernel();
    }

    /*!
     * \brief Compute internal matrices.
     *
     * \param[in] variable_list List of variables.
     * \param[in] data Data vector.
     */
    template <typename Data>
    void compute(std::vector<variable_type> variable_list,
        const Eigen::MatrixBase<Data>& data) {
        solver_.compute(variable_list, data);
        solver_.solve(coeff_);
        variable_list_ = std::move(variable_list);
        common_coeff_ = solver_.common_coeff();
    }

    /*!
     * \brief Interpolate a value for a variable.
     *
     * \param[in] variable Variable.
     * \return Value.
     */
    [[nodiscard]] auto interpolate_on(
        const variable_type& variable) const -> value_type {
        auto value = static_cast<value_type>(0);
        for (std::size_t i = 0; i < variable_list_.size(); ++i) {
            value += kernel()(variable, variable_list_[i]) *
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
    [[nodiscard]] auto operator()(
        const variable_type& variable) const -> value_type {
        return interpolate_on(variable);
    }

    /*!
     * \brief Evaluate mean and variance for a variable.
     *
     * \param[in] variable Variable.
     * \return Mean and variance.
     */
    [[nodiscard]] auto evaluate_mean_and_variance_on(
        const variable_type& variable) const
        -> std::pair<value_type, value_type> {
        Eigen::VectorXd kernel_vec;
        kernel_vec.resize(static_cast<index_type>(variable_list_.size()));
        for (std::size_t i = 0; i < variable_list_.size(); ++i) {
            kernel_vec(static_cast<index_type>(i)) =
                kernel()(variable, variable_list_[i]);
        }

        const value_type mean = kernel_vec.dot(coeff_);
        const value_type variance = common_coeff_ *
            std::max(kernel()(variable, variable) - calc_reg_term(kernel_vec),
                static_cast<value_type>(0));
        return {mean, variance};
    }

    /*!
     * \brief Get the value of the MLE objective function
     * \cite Scheuerer2011.
     *
     * \return Value of the MLE objective function.
     */
    [[nodiscard]] auto mle_objective_function_value() const -> value_type {
        return solver_.mle_objective_function_value();
    }

    /*!
     * \brief Calculate the coefficient of the kernel common in variables.
     *
     * \return Value.
     */
    [[nodiscard]] auto common_coeff() const -> value_type {
        return common_coeff_;
    }

    /*!
     * \brief Calculate the regularization term for a vector.
     *
     * \tparam InputData Type of the input data.
     * \param[in] data Data vector.
     * \return Value.
     */
    template <typename InputData>
    [[nodiscard]] auto calc_reg_term(
        const Eigen::MatrixBase<InputData>& data) const -> value_type {
        return solver_.calc_reg_term(data);
    }

    /*!
     * \brief Access to the logger.
     *
     * \return Logger.
     */
    [[nodiscard]] auto logger() const noexcept -> const logging::logger& {
        return solver_.logger();
    }

    /*!
     * \brief Access to the logger.
     *
     * \return Logger.
     */
    [[nodiscard]] auto logger() noexcept -> logging::logger& {
        return solver_.logger();
    }

private:
    //! Solver.
    impl::kernel_coeff_solver<kernel_type> solver_;

    //! List of variables.
    std::vector<variable_type> variable_list_;

    //! Coefficients of kernels for variables.
    Eigen::VectorX<value_type> coeff_{};

    //! Coefficient of the kernel common in variables.
    value_type common_coeff_{};
};

}  // namespace num_collect::interp::kernel
