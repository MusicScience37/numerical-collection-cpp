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
 * \brief Definition of auto_regularizer_wrapper class.
 */
#pragma once

#include <memory>

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/exception.h"
#include "num_collect/interp/kernel/calc_kernel_mat.h"
#include "num_collect/interp/kernel/impl/auto_regularizer.h"
#include "num_collect/interp/kernel/impl/self_adjoint_kernel_solver.h"
#include "num_collect/logging/logger.h"

namespace num_collect::interp::kernel::impl {

/*!
 * \brief Helper class to use auto_regularizer class only if needed.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class auto_regularizer_wrapper {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    //! Type of kernel matrices.
    using kernel_mat_type = Eigen::MatrixX<scalar_type>;

    //! Type of data vector.
    using data_type = Eigen::VectorX<scalar_type>;

    //! Type of the solver of kernel matrices.
    using solver_type = self_adjoint_kernel_solver<kernel_mat_type, data_type>;

    //! Type of object to determine regularization parameter.
    using regularizer_type = auto_regularizer<solver_type>;

    /*!
     * \brief Set the regularization parameter.
     *
     * \param[in] reg_param Regularization parameter.
     */
    void regularize_with(const scalar_type& reg_param) {
        if (reg_param < static_cast<scalar_type>(0)) {
            throw invalid_argument(
                "Regularization parameter must be a a non-negative value.");
        }
        reg_param_ = reg_param;
        regularizer_.reset();
    }

    /*!
     * \brief Set to determine the regularization parameter automatically.
     */
    void regularize_automatically() {
        if (!regularizer_) {
            regularizer_ = std::make_unique<regularizer_type>(solver_);
        }
    }

    /*!
     * \brief Get the regularization parameter.
     *
     * \return Regularization parameter.
     */
    [[nodiscard]] auto reg_param() const -> scalar_type { return reg_param_; }

    /*!
     * \brief Compute internal matrices.
     *
     * \tparam Kernel Type of the kernel.
     * \tparam Container Type of the list of variables.
     * \tparam Data Type of the data vector.
     * \param[in] kernel Kernel.
     * \param[in] variable_list List of variables.
     * \param[in] data Data vector.
     */
    template <typename Kernel, typename Container, typename Data>
    void compute(const Kernel& kernel, const Container& variable_list,
        const Eigen::MatrixBase<Data>& data) {
        kernel_mat_ = calc_kernel_mat(kernel, variable_list);
        solver_.compute(kernel_mat_, data);

        if (regularizer_) {
            regularizer_->optimize();
            reg_param_ = regularizer_->opt_param();
        }
    }

    /*!
     * \brief Calculate coefficients of the kernel for variables.
     *
     * \tparam Coeff Type of coefficients.
     * \param[out] coeff Coefficients.
     */
    template <typename Coeff>
    void solve(Eigen::MatrixBase<Coeff>& coeff) const {
        solver_.solve(reg_param_, coeff);
    }

    /*!
     * \brief Get the value of the MLE objective function
     * \cite Scheuerer2011.
     *
     * \return Value of the MLE objective function.
     */
    [[nodiscard]] auto mle_objective_function_value() const -> scalar_type {
        return solver_.calc_mle_objective(reg_param_);
    }

    /*!
     * \brief Calculate the coefficient of the kernel common in variables.
     *
     * \return Value.
     */
    [[nodiscard]] auto common_coeff() const -> scalar_type {
        return solver_.calc_common_coeff(reg_param_);
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
        const Eigen::MatrixBase<InputData>& data) const -> scalar_type {
        return solver_.calc_reg_term(reg_param_, data);
    }

    /*!
     * \brief Access to the logger.
     *
     * \return Logger.
     */
    [[nodiscard]] auto logger() noexcept -> logging::logger* {
        if (!regularizer_) {
            return nullptr;
        }
        return &regularizer_->logger();
    }

private:
    //! Kernel matrix.
    kernel_mat_type kernel_mat_{};

    //! Solver of kernel matrices.
    solver_type solver_{};

    //! Regularization parameter.
    scalar_type reg_param_{static_cast<scalar_type>(0)};

    //! Object to determine regularization parameter.
    std::unique_ptr<regularizer_type> regularizer_{};
};

}  // namespace num_collect::interp::kernel::impl
