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
 * \brief Definition of kernel_coeff_solver class
 */
#pragma once

#include <cstddef>
#include <memory>
#include <string_view>

#include <Eigen/Core>

#include "num_collect/interp/kernel/concepts/kernel.h"  // IWYU pragma: keep
#include "num_collect/interp/kernel/impl/auto_regularizer_wrapper.h"
#include "num_collect/interp/kernel/impl/kernel_parameter_optimizer.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/util/assert.h"

namespace num_collect::interp::kernel {

//! Log tag of kernel_interpolator.
constexpr auto kernel_interpolator_tag =
    logging::log_tag_view("num_collect::interp::kernel::kernel_interpolator");

}  // namespace num_collect::interp::kernel

namespace num_collect::interp::kernel::impl {

/*!
 * \brief Class to calculate coefficients of kernels for variables.
 *
 * \tparam Kernel Type of the kernel.
 */
template <concepts::kernel Kernel>
class kernel_coeff_solver : public logging::logging_mixin {
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
    explicit kernel_coeff_solver(const kernel_type& kernel = kernel_type())
        : logging::logging_mixin(kernel_interpolator_tag), kernel_(kernel) {
        search_kernel_param_auto();
    }

    /*!
     * \brief Set the regularization parameter.
     *
     * \param[in] reg_param Regularization parameter.
     * \return This.
     */
    auto regularize_with(const value_type& reg_param) -> kernel_coeff_solver& {
        interpolator_.regularize_with(reg_param);
        return *this;
    }

    /*!
     * \brief Set to determine the regularization parameter automatically.
     *
     * \return This.
     */
    auto regularize_automatically() -> kernel_coeff_solver& {
        interpolator_.regularize_automatically();
        update_logger_state();
        return *this;
    }

    /*!
     * \brief Set to interpolate without regularization.
     *
     * \return This.
     */
    auto disable_regularization() -> kernel_coeff_solver& {
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
    auto fix_kernel_param(
        const kernel_param_type& kernel_param) -> kernel_coeff_solver& {
        kernel_.kernel_param(kernel_param);
        optimizer_.reset();
        return *this;
    }

    /*!
     * \brief Set to determine kernel parameters automatically.
     *
     * \return This.
     */
    auto search_kernel_param_auto() -> kernel_coeff_solver& {
        if (!optimizer_) {
            optimizer_ =
                std::make_unique<impl::kernel_parameter_optimizer<kernel_type>>(
                    interpolator_, kernel_);
        }
        update_logger_state();
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
     * \tparam Container Type of the list of variables.
     * \tparam InputData Type of the input data vector.
     * \param[in] variable_list List of variables.
     * \param[in] data Data vector.
     */
    template <typename Container, typename InputData>
    void compute(const Container& variable_list,
        const Eigen::MatrixBase<InputData>& data) {
        NUM_COLLECT_ASSERT(
            static_cast<std::size_t>(data.rows()) == variable_list.size());
        NUM_COLLECT_ASSERT(data.cols() == 1);

        if (optimizer_) {
            optimizer_->compute(variable_list, data);
            kernel_.kernel_param(optimizer_->opt_param());
        }
        interpolator_.compute(kernel_, variable_list, data);
    }

    /*!
     * \brief Calculate coefficients of the kernel for variables.
     *
     * \tparam Coeff Type of coefficients.
     * \param[out] coeff Coefficients.
     */
    template <typename Coeff>
    void solve(Eigen::MatrixBase<Coeff>& coeff) const {
        interpolator_.solve(coeff);
    }

    /*!
     * \brief Get the value of the MLE objective function
     * \cite Scheuerer2011.
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
     * \brief Calculate the regularization term for a vector.
     *
     * \tparam InputData Type of the input data.
     * \param[in] data Data vector.
     * \return Value.
     */
    template <typename InputData>
    [[nodiscard]] auto calc_reg_term(
        const Eigen::MatrixBase<InputData>& data) const -> value_type {
        return interpolator_.calc_reg_term(data);
    }

private:
    /*!
     * \brief Update states of loggers.
     */
    void update_logger_state() {
        if (optimizer_) {
            logging::logger& parent_logger = optimizer_->logger();
            this->logger().initialize_child_algorithm_logger(parent_logger);
            logging::logger* child_logger = interpolator_.logger();
            if (child_logger != nullptr) {
                parent_logger.initialize_child_algorithm_logger(*child_logger);
            }
        } else {
            logging::logger* child_logger = interpolator_.logger();
            if (child_logger != nullptr) {
                this->logger().initialize_child_algorithm_logger(*child_logger);
            }
        }
    }

    //! Kernel.
    kernel_type kernel_;

    //! Internal interpolator.
    impl::auto_regularizer_wrapper<value_type> interpolator_{};

    //! Optimizer of kernel parameters.
    std::unique_ptr<impl::kernel_parameter_optimizer<kernel_type>> optimizer_{};
};

}  // namespace num_collect::interp::kernel::impl
