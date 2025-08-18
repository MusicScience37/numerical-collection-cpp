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

#include <optional>
#include <type_traits>

#include <Eigen/Core>
#include <Eigen/Householder>

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/regularization/explicit_regularized_solver_base.h"
#include "num_collect/regularization/tikhonov.h"
#include "num_collect/util/assert.h"

namespace num_collect::regularization {

//! Tag of fista.
constexpr auto full_gen_tikhonov_tag =
    logging::log_tag_view("num_collect::regularization::full_gen_tikhonov");

/*!
 * \brief Class to perform generalized Tikhonov regularization on the condition
 * that the matrix in the regularization term which have full row rank
 * \cite Elden1982, \cite Hansen1994.
 *
 * \tparam Coeff Type of coefficient matrices.
 * \tparam Data Type of data vectors.
 *
 * This class minimizes the following function:
 *
 * \f[
 *     \| A \boldsymbol{x} - \boldsymbol{y} \|_2^2
 *     + \lambda \| L \boldsymbol{x} \|_2^2
 * \f]
 *
 * where \f$ A \f$ is a coefficient matrix,
 * \f$ L \f$ is a coefficient matrix for the regularization term,
 * \f$ \boldsymbol{x} \f$ is a solution vector,
 * \f$ \boldsymbol{y} \f$ is a data vector,
 * \f$ \lambda \f$ is a regularization parameter.
 *
 * In this class, following conditions must be satisfied:
 *
 * - \f$ L \f$ must have full row rank because of the formula used in this
 * class.
 * - \f$ A \f$ and \f$ L \f$ must have only the zero vector in the intersection
 * of their null spaces. This is a requirement for uniqueness of the solution.
 */
template <base::concepts::dense_matrix Coeff, base::concepts::dense_matrix Data>
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

    static_assert(std::is_same_v<typename coeff_type::Scalar,
        typename data_type::Scalar>);
    static_assert(data_type::RowsAtCompileTime == Eigen::Dynamic);

    /*!
     * \brief Constructor.
     */
    full_gen_tikhonov() : base_type(full_gen_tikhonov_tag) {}

    /*!
     * \brief Compute internal matrices.
     *
     * This generate arranged problem of Tikhonov regularization as in
     * \cite Elden1982, \cite Hansen1994.
     *
     * \param[in] coeff Coefficient matrix.
     * \param[in] data Data vector.
     * \param[in] reg_coeff Coefficient matrix for the regularization term.
     *
     * \note Pointers to the arguments are saved in this object, so don't
     * destruct those arguments.
     */
    void compute(const coeff_type& coeff, const data_type& data,
        const coeff_type& reg_coeff) {
        NUM_COLLECT_PRECONDITION(coeff.rows() == data.rows(), this->logger(),
            "The number of rows in the coefficient matrix must match the "
            "number of rows in data.");
        NUM_COLLECT_PRECONDITION(coeff.cols() == reg_coeff.cols(),
            this->logger(),
            "The number of columns in the coefficient matrix must match the "
            "number of columns in the coefficient matrix of the regularization "
            "term.");
        NUM_COLLECT_PRECONDITION(reg_coeff.rows() < reg_coeff.cols(),
            this->logger(),
            "Coefficient matrix for the regularization term must have rows "
            "less than columns.");

        coeff_.emplace(coeff);
        reg_coeff_.emplace(reg_coeff);

        compute_impl(coeff, data, reg_coeff);
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::solve
    void solve(const scalar_type& param, data_type& solution) const {
        data_type tikhonov_solution;
        tikhonov_.solve(param, tikhonov_solution);
        solution = coeff_actual_solution_ * tikhonov_solution +
            offset_actual_solution_;
    }

    //! \copydoc num_collect::regularization::regularized_solver_base::change_data
    void change_data(const data_type& data) {
        NUM_COLLECT_ASSERT(coeff_.has_value());
        NUM_COLLECT_ASSERT(reg_coeff_.has_value());
        compute_impl(*coeff_, data, *reg_coeff_);
    }

    //! \copydoc num_collect::regularization::regularized_solver_base::calculate_data_for
    void calculate_data_for(const data_type& solution, data_type& data) const {
        NUM_COLLECT_ASSERT(coeff_.has_value());
        data = (*coeff_) * solution;
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

    //! \copydoc num_collect::regularization::regularized_solver_base::residual_norm
    [[nodiscard]] auto residual_norm(const data_type& solution) const
        -> scalar_type {
        NUM_COLLECT_ASSERT(reg_coeff_.has_value());
        return tikhonov_.residual_norm((*reg_coeff_) * solution);
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::regularization_term
    [[nodiscard]] auto regularization_term(const scalar_type& param) const
        -> scalar_type {
        return tikhonov_.regularization_term(param);
    }

    //! \copydoc num_collect::regularization::regularized_solver_base::regularization_term
    [[nodiscard]] auto regularization_term(const data_type& solution) const
        -> scalar_type {
        NUM_COLLECT_ASSERT(reg_coeff_.has_value());
        return ((*reg_coeff_) * solution).squaredNorm();
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

    //! \copydoc num_collect::regularization::regularized_solver_base::data_size
    [[nodiscard]] auto data_size() const -> index_type {
        return tikhonov_.data_size();
    }

    //! \copydoc num_collect::regularization::regularized_solver_base::param_search_region
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
    template <typename InputMatrix>
    void compute_impl(const Eigen::MatrixBase<InputMatrix>& coeff,
        const data_type& data,
        const Eigen::MatrixBase<InputMatrix>& reg_coeff) {
        // TODO How can I implement those complex formulas with good variable
        // names?

        const index_type m = coeff.rows();
        const index_type n = coeff.cols();
        const index_type p = reg_coeff.rows();

        Eigen::ColPivHouseholderQR<coeff_type> qr_reg_adj;
        qr_reg_adj.compute(reg_coeff.adjoint());
        NUM_COLLECT_PRECONDITION(qr_reg_adj.rank() >= qr_reg_adj.cols(),
            this->logger(), "reg_coeff must have full row rank.");
        const coeff_type v = qr_reg_adj.householderQ();

        Eigen::ColPivHouseholderQR<coeff_type> qr_coeff_v2;
        qr_coeff_v2.compute(coeff * v.rightCols(n - p));
        NUM_COLLECT_PRECONDITION(qr_coeff_v2.rank() >= qr_coeff_v2.cols(),
            this->logger(),
            "reg_coeff and coeff must have only the zero vector in the "
            "intersection of their null spaces.");
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

    //! Object to perform Tikhonov regularization.
    tikhonov<coeff_type, data_type> tikhonov_{};

    //! Coefficient matrix to calculate actual solution.
    Coeff coeff_actual_solution_{};

    //! Offset vector to calculate actual solution.
    Data offset_actual_solution_{};

    //! Coefficient matrix.
    std::optional<Eigen::Ref<const coeff_type>> coeff_;

    //! Coefficient matrix for the regularization term.
    std::optional<Eigen::Ref<const coeff_type>> reg_coeff_;
};

}  // namespace num_collect::regularization
