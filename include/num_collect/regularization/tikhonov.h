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
 * \brief Definition of tikhonov class.
 */
#pragma once

#include <type_traits>

#include <Eigen/Core>
#include <Eigen/SVD>

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/base/index_type.h"
#include "num_collect/regularization/explicit_regularized_solver_base.h"
#include "num_collect/regularization/impl/coeff_param.h"
#include "num_collect/util/impl/warn_fast_math_for_bdcsvc.h"

namespace num_collect::regularization {

/*!
 * \brief Class to perform Tikhonov regularization.
 *
 * \tparam Coeff Type of coefficient matrices.
 * \tparam Data Type of data vectors.
 */
template <base::concepts::dense_matrix Coeff, base::concepts::dense_matrix Data>
class tikhonov
    : public explicit_regularized_solver_base<tikhonov<Coeff, Data>, Data> {
public:
    //! Type of base class.
    using base_type =
        explicit_regularized_solver_base<tikhonov<Coeff, Data>, Data>;

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
    tikhonov() = default;

    /*!
     * \brief Compute internal matrices.
     *
     * \param[in] coeff Coefficient matrix.
     * \param[in] data Data vector.
     */
    void compute(const coeff_type& coeff, const data_type& data) {
        svd_.compute(coeff, Eigen::ComputeThinU | Eigen::ComputeThinV);
        rot_data_ = svd_.matrixU().adjoint() * data;
        const index_type rank = svd_.nonzeroSingularValues();
        min_res_ = (data -
            svd_.matrixU().leftCols(rank) *
                svd_.matrixU().leftCols(rank).adjoint() * data)
                       .squaredNorm();
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::solve
    void solve(const scalar_type& param, data_type& solution) const {
        solution = data_type::Zero(svd_.cols(), rot_data_.cols());
        const index_type rank = svd_.nonzeroSingularValues();
        for (index_type i = 0; i < rank; ++i) {
            const scalar_type singular_value = singular_values()(i);
            const scalar_type factor =
                singular_value / (singular_value * singular_value + param);
            solution += factor * svd_.matrixV().col(i) * rot_data_.row(i);
        }
    }

    /*!
     * \brief Get the singular values.
     *
     * \return Singular values.
     */
    [[nodiscard]] auto singular_values() const -> const
        typename Eigen::BDCSVD<coeff_type>::SingularValuesType& {
        return svd_.singularValues();
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::residual_norm
    [[nodiscard]] auto residual_norm(const scalar_type& param) const
        -> scalar_type {
        auto res = static_cast<scalar_type>(0);
        const index_type rank = svd_.nonzeroSingularValues();
        for (index_type i = 0; i < rank; ++i) {
            const scalar_type singular_value = svd_.singularValues()(i);
            const scalar_type den = singular_value * singular_value + param;
            res +=
                (param * param) / (den * den) * rot_data_.row(i).squaredNorm();
        }
        return res + min_res_;
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::regularization_term
    [[nodiscard]] auto regularization_term(const scalar_type& param) const
        -> scalar_type {
        auto res = static_cast<scalar_type>(0);
        const index_type rank = svd_.nonzeroSingularValues();
        for (index_type i = 0; i < rank; ++i) {
            const scalar_type singular_value = svd_.singularValues()(i);
            const scalar_type den = singular_value * singular_value + param;
            res += (singular_value * singular_value) / (den * den) *
                rot_data_.row(i).squaredNorm();
        }
        return res;
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::first_derivative_of_residual_norm
    [[nodiscard]] auto first_derivative_of_residual_norm(
        const scalar_type& param) const -> scalar_type {
        auto res = static_cast<scalar_type>(0);
        const index_type rank = svd_.nonzeroSingularValues();
        for (index_type i = 0; i < rank; ++i) {
            const scalar_type singular_value = svd_.singularValues()(i);
            const scalar_type den = singular_value * singular_value + param;
            res += (static_cast<scalar_type>(2) * param * singular_value *
                       singular_value) /
                (den * den * den) * rot_data_.row(i).squaredNorm();
        }
        return res;
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::first_derivative_of_regularization_term
    [[nodiscard]] auto first_derivative_of_regularization_term(
        const scalar_type& param) const -> scalar_type {
        auto res = static_cast<scalar_type>(0);
        const index_type rank = svd_.nonzeroSingularValues();
        for (index_type i = 0; i < rank; ++i) {
            const scalar_type singular_value = svd_.singularValues()(i);
            const scalar_type den = singular_value * singular_value + param;
            res += (-static_cast<scalar_type>(2) * singular_value *
                       singular_value) /
                (den * den * den) * rot_data_.row(i).squaredNorm();
        }
        return res;
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::second_derivative_of_residual_norm
    [[nodiscard]] auto second_derivative_of_residual_norm(
        const scalar_type& param) const -> scalar_type {
        auto res = static_cast<scalar_type>(0);
        const index_type rank = svd_.nonzeroSingularValues();
        for (index_type i = 0; i < rank; ++i) {
            const scalar_type singular_value = svd_.singularValues()(i);
            const scalar_type den = singular_value * singular_value + param;
            res += (static_cast<scalar_type>(2) * singular_value *
                           singular_value * singular_value * singular_value -
                       static_cast<scalar_type>(4) * param * singular_value *
                           singular_value) /
                (den * den * den * den) * rot_data_.row(i).squaredNorm();
        }
        return res;
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::second_derivative_of_regularization_term
    [[nodiscard]] auto second_derivative_of_regularization_term(
        const scalar_type& param) const -> scalar_type {
        auto res = static_cast<scalar_type>(0);
        const index_type rank = svd_.nonzeroSingularValues();
        for (index_type i = 0; i < rank; ++i) {
            const scalar_type singular_value = svd_.singularValues()(i);
            const scalar_type den = singular_value * singular_value + param;
            res += (static_cast<scalar_type>(6) * singular_value *  // NOLINT
                       singular_value) /
                (den * den * den * den) * rot_data_.row(i).squaredNorm();
        }
        return res;
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::sum_of_filter_factor
    [[nodiscard]] auto sum_of_filter_factor(const scalar_type& param) const
        -> scalar_type {
        auto res = static_cast<scalar_type>(0);
        const index_type rank = svd_.nonzeroSingularValues();
        for (index_type i = 0; i < rank; ++i) {
            const scalar_type singular_value = svd_.singularValues()(i);
            const scalar_type den = singular_value * singular_value + param;
            res += singular_value * singular_value / den;
        }
        return res;
    }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::data_size
    [[nodiscard]] auto data_size() const -> index_type { return svd_.rows(); }

    //! \copydoc num_collect::regularization::explicit_regularized_solver_base::param_search_region
    [[nodiscard]] auto param_search_region() const
        -> std::pair<scalar_type, scalar_type> {
        const scalar_type max_singular_value = singular_values()(0);
        const scalar_type squared_max_singular_value =
            max_singular_value * max_singular_value;
        return {impl::coeff_min_param<scalar_type> * squared_max_singular_value,
            impl::coeff_max_param<scalar_type> * squared_max_singular_value};
    }

private:
    //! SVD of the coefficient matrix.
    Eigen::BDCSVD<coeff_type> svd_{};

    //! Data in the space of singular values.
    data_type rot_data_{};

    //! Minimum residual.
    scalar_type min_res_{};
};

}  // namespace num_collect::regularization
