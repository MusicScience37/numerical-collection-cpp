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
 * \brief Definition of full_gen_tikhonov class.
 */
#pragma once

#include <type_traits>

#include <Eigen/Core>
#include <Eigen/Householder>

#include "num_collect/regularization/explicit_regularized_solver_base.h"
#include "num_collect/regularization/tikhonov.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/exception.h"
#include "num_collect/util/index_type.h"

namespace num_collect::regularization {

/*!
 * \brief Class to perform generalized Tikhonov regularization on the condition
 * that the matrix in the regularization term which have full row rank
 * \cite Elden1982, \cite Hansen1994.
 *
 * \tparam Coeff Type of coefficient matrices.
 * \tparam Data Type of data vectors.
 */
template <typename Coeff, typename Data>
class full_gen_tikhonov
    : public explicit_regularized_solver_base<full_gen_tikhonov<Coeff, Data>,
          Data> {
public:
    //! Type of base class.
    using base_type =
        explicit_regularized_solver_base<full_gen_tikhonov<Coeff, Data>, Data>;

    using typename base_type::data_type;
    using typename base_type::scalar_type;

    //! Type of coefficient matrices.
    using coeff_type = Coeff;

    static_assert(std::is_same_v<typename coeff_type::Scalar, scalar_type>);
    static_assert(std::is_same_v<typename data_type::Scalar, scalar_type>);
    static_assert(data_type::RowsAtCompileTime == Eigen::Dynamic);

    /*!
     * \brief Construct.
     */
    full_gen_tikhonov() = default;

    /*!
     * \brief Compute internal matrices.
     *
     * This generate arranged problem of Tikhonov regularization as in
     * \cite Elden1982, \cite Hansen1994.
     *
     * \param[in] coeff Coefficient matrix.
     * \param[in] data Data vector.
     * \param[in] reg_coeff Coefficient matrix for the regularization term.
     */
    void compute(const coeff_type& coeff, const data_type& data,
        const coeff_type& reg_coeff) {
        NUM_COLLECT_ASSERT(coeff.rows() == data.rows());
        NUM_COLLECT_ASSERT(coeff.cols() == reg_coeff.cols());
        NUM_COLLECT_ASSERT(reg_coeff.rows() < reg_coeff.cols());

        // How can I implement those complex formulas with good variable names.

        const index_type m = coeff.rows();
        const index_type n = coeff.cols();
        const index_type p = reg_coeff.rows();

        Eigen::ColPivHouseholderQR<coeff_type> qr_reg_adj;
        qr_reg_adj.compute(reg_coeff.adjoint());
        if (qr_reg_adj.rank() < qr_reg_adj.cols()) {
            throw assertion_failure(
                fmt::format("reg_coeff must have full row rank (at {})",
                    NUM_COLLECT_FUNCTION));
        }
        const coeff_type v = qr_reg_adj.householderQ();

        Eigen::ColPivHouseholderQR<coeff_type> qr_coeff_v2;
        qr_coeff_v2.compute(coeff * v.rightCols(n - p));
        if (qr_coeff_v2.rank() < qr_coeff_v2.cols()) {
            throw assertion_failure(
                fmt::format("reg_coeff and coeff must not have common elements "
                            "other than zero in their kernel (at {})",
                    NUM_COLLECT_FUNCTION));
        }
        const coeff_type q = qr_coeff_v2.householderQ();

        const coeff_type coeff_arr =
            qr_reg_adj.solve(coeff.adjoint() * q.rightCols(m - n + p))
                .adjoint();
        const data_type data_arr = q.rightCols(m - n + p).adjoint() * data;
        tikhonov_.compute(coeff_arr, data_arr);

        const coeff_type coeff_v2_inv_coeff = qr_coeff_v2.solve(coeff);
        const coeff_type i_minus_v2_coeff_v2_inv_coeff =
            coeff_type::Identity(n, n) -
            v.rightCols(n - p) * coeff_v2_inv_coeff;
        coeff_actual_solution_ =
            qr_reg_adj.solve(i_minus_v2_coeff_v2_inv_coeff.adjoint()).adjoint();

        const data_type coeff_v2_inv_data = qr_coeff_v2.solve(data);
        offset_actual_solution_ = v.rightCols(n - p) * coeff_v2_inv_data;
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::solve
    void solve(const scalar_type& param, data_type& solution) const {
        data_type tikhonov_solution;
        tikhonov_.solve(param, tikhonov_solution);
        solution = coeff_actual_solution_ * tikhonov_solution +
            offset_actual_solution_;
    }

    /*!
     * \brief Get the singular values.
     *
     * \return Singular values.
     */
    [[nodiscard]] auto singular_values() const -> const
        typename Eigen::BDCSVD<coeff_type>::SingularValuesType& {
        return tikhonov_.singular_values();
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::residual_norm
    [[nodiscard]] auto residual_norm(const scalar_type& param) const
        -> scalar_type {
        return tikhonov_.residual_norm(param);
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::regularization_term
    [[nodiscard]] auto regularization_term(const scalar_type& param) const
        -> scalar_type {
        return tikhonov_.regularization_term(param);
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::first_derivative_of_residual_norm
    [[nodiscard]] auto first_derivative_of_residual_norm(
        const scalar_type& param) const -> scalar_type {
        return tikhonov_.first_derivative_of_residual_norm(param);
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::first_derivative_of_regularization_term
    [[nodiscard]] auto first_derivative_of_regularization_term(
        const scalar_type& param) const -> scalar_type {
        return tikhonov_.first_derivative_of_regularization_term(param);
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::second_derivative_of_residual_norm
    [[nodiscard]] auto second_derivative_of_residual_norm(
        const scalar_type& param) const -> scalar_type {
        return tikhonov_.second_derivative_of_residual_norm(param);
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::second_derivative_of_regularization_term
    [[nodiscard]] auto second_derivative_of_regularization_term(
        const scalar_type& param) const -> scalar_type {
        return tikhonov_.second_derivative_of_regularization_term(param);
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::sum_of_filter_factor
    [[nodiscard]] auto sum_of_filter_factor(const scalar_type& param) const
        -> scalar_type {
        return tikhonov_.sum_of_filter_factor(param);
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::data_size
    [[nodiscard]] auto data_size() const -> index_type {
        return tikhonov_.data_size();
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::param_search_region
    [[nodiscard]] auto param_search_region() const
        -> std::pair<scalar_type, scalar_type> {
        return tikhonov_.param_search_region();
    }

    /*!
     * \brief Access to the internal solver (for debug).
     *
     * \return Internal solver.
     */
    [[nodiscard]] auto internal_solver() const
        -> const tikhonov<coeff_type, data_type>& {
        return tikhonov_;
    }

private:
    //! Object to perform Tikhonov regularization.
    tikhonov<coeff_type, data_type> tikhonov_{};

    //! Coefficient matrix to calculate actual solution.
    Coeff coeff_actual_solution_{};

    //! Offset vector to calculate actual solution.
    Data offset_actual_solution_{};
};

}  // namespace num_collect::regularization
