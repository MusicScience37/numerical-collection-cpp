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

#include <Eigen/Core>
#include <Eigen/SVD>
#include <type_traits>

#include "num_collect/regularization/explicit_regularized_solver_base.h"
#include "num_collect/util/index_type.h"

namespace num_collect::regularization {

/*!
 * \brief Class to perform Tikhonov regularization.
 *
 * \tparam Coeff Type of coefficient matrices.
 * \tparam Data Type of data vectors.
 */
template <typename Coeff, typename Data>
class tikhonov : public explicit_regularized_solver_base<tikhonov<Coeff, Data>,
                     typename Coeff::Scalar> {
public:
    //! Type of base class.
    using base_type = explicit_regularized_solver_base<tikhonov<Coeff, Data>,
        typename Coeff::Scalar>;

    using typename base_type::scalar_type;

    //! Type of coefficient matrices.
    using coeff_type = Coeff;

    //! Type of data vectors.
    using data_type = Data;

    static_assert(std::is_same_v<typename coeff_type::Scalar, scalar_type>);
    static_assert(std::is_same_v<typename data_type::Scalar, scalar_type>);
    static_assert(data_type::RowsAtCompileTime == Eigen::Dynamic);

    /*!
     * \brief Construct.
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
    }

    /*!
     * \brief Solve for a regularization parameter.
     *
     * \param[in] param Regularization parameter.
     * \param[out] solution Solution.
     */
    void solve(const scalar_type& param, data_type& solution) const {
        solution = data_type::Zero(svd_.cols(), rot_data_.cols());
        const index_type rank = svd_.nonzeroSingularValues();
        for (index_type i = 0; i < rank; ++i) {
            const scalar_type singular_value = svd_.singularValues()(i);
            const scalar_type factor =
                singular_value / (singular_value * singular_value + param);
            solution += factor * svd_.matrixV().col(i) * rot_data_.row(i);
        }
    }

private:
    //! SVD of the coefficient matrix.
    Eigen::BDCSVD<coeff_type> svd_{};

    //! Data in the space of singular values.
    data_type rot_data_{};
};

}  // namespace num_collect::regularization
