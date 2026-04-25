/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of radau2a_table class.
 */
#pragma once

#include <array>
#include <cstddef>

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/ode/runge_kutta/impl/generate_radau2a_coeffs_impl.h"

namespace num_collect::ode::runge_kutta::impl {

/*!
 * \brief Class to hold coefficients in Butcher tableau of Radau IIA method
 * \cite Hairer1991.
 *
 * \tparam Stages Number of stages.
 */
template <index_type Stages>
class radau2a_table {
public:
    //! Type of coefficients in this class.
    using scalar_type = long double;

    //! Type of matrices.
    using matrix_type = Eigen::Matrix<scalar_type, Stages, Stages>;

    //! Type of vectors.
    using vector_type = Eigen::Vector<scalar_type, Stages>;

    /*!
     * \brief Constructor.
     *
     * \param[in] slope_coeffs Coefficients of intermediate slopes in the
     * formula.
     * \param[in] time_coeffs Coefficients of time in the formula.
     */
    radau2a_table(
        const matrix_type& slope_coeffs, const vector_type& time_coeffs)
        : slope_coeffs_(slope_coeffs), time_coeffs_(time_coeffs) {}

    /*!
     * \brief Get the coefficients of intermediate slopes in the formula in the
     * specified type.
     *
     * \tparam ResultScalar Type of scalars in the result.
     * \return Coefficients.
     */
    template <base::concepts::real_scalar ResultScalar>
    [[nodiscard]] auto slope_coeffs() const
        -> Eigen::Matrix<ResultScalar, Stages, Stages> {
        return slope_coeffs_.template cast<ResultScalar>();
    }

    /*!
     * \brief Get the coefficients of time in the formula in the specified type.
     *
     * \tparam ResultScalar Type of scalars in the result.
     * \return Coefficients.
     */
    template <base::concepts::real_scalar ResultScalar>
    [[nodiscard]] auto time_coeffs() const
        -> Eigen::Vector<ResultScalar, Stages> {
        return time_coeffs_.template cast<ResultScalar>();
    }

private:
    //! Coefficients of intermediate slopes in the formula.
    matrix_type slope_coeffs_;

    //! Coefficients of time in the formula.
    vector_type time_coeffs_;
};

/*!
 * \brief Compute coefficients of Butcher tableau.
 *
 * \tparam Stages Number of stages.
 * \return Coefficients.
 */
template <index_type Stages>
[[nodiscard]] static auto compute_radau2a_table() -> radau2a_table<Stages> {
    using scalar_type = long double;

    std::array<scalar_type, static_cast<std::size_t>(Stages * Stages)>
        slope_coeffs_buffer{};
    std::array<scalar_type, static_cast<std::size_t>(Stages)>
        time_coeffs_buffer{};
    generate_radau2a_coeffs_impl(
        Stages, slope_coeffs_buffer.data(), time_coeffs_buffer.data());
    Eigen::Map<Eigen::Matrix<scalar_type, Stages, Stages, Eigen::ColMajor>>
        slope_coeffs_map(slope_coeffs_buffer.data(), Stages, Stages);
    Eigen::Map<Eigen::Vector<scalar_type, Stages>> time_coeffs_map(
        time_coeffs_buffer.data(), Stages);

    return radau2a_table<Stages>(slope_coeffs_map, time_coeffs_map);
}

/*!
 * \brief Get coefficients of Butcher tableau of Radau IIA method.
 *
 * \tparam Stages Number of stages.
 * \return Coefficients.
 *
 * \note This function caches the generated coefficients.
 */
template <index_type Stages>
[[nodiscard]] auto get_radau2a_table() -> const radau2a_table<Stages>& {
    static const auto table = compute_radau2a_table<Stages>();
    return table;
}

}  // namespace num_collect::ode::runge_kutta::impl
