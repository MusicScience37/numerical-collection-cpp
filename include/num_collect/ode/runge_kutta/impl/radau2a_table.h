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

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <Eigen/Core>
#include <Eigen/LU>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/polynomials/compute_polynomial_zeros.h"
#include "num_collect/polynomials/polynomial.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/format_dense_matrix.h"
#include "num_collect/util/format_dense_vector.h"
#include "num_collect/util/vector.h"

namespace num_collect::ode::runge_kutta::impl {

/*!
 * \brief Class to hold coefficients in Butcher tableau of Radau IIA method
 * \cite Hairer1991.
 *
 * \tparam Stages Number of stages.
 */
template <num_collect::index_type Stages>
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
     * \param[in] slope_coeffs Coefficients of intermidiate slopes in the
     * formula.
     * \param[in] time_coeffs Coefficients of time in the formula.
     */
    radau2a_table(
        const matrix_type& slope_coeffs, const vector_type& time_coeffs)
        : slope_coeffs_(slope_coeffs), time_coeffs_(time_coeffs) {}

    /*!
     * \brief Get the coefficients of intermidiate slopes in the formula in the
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
    //! Coefficients of intermidiate slopes in the formula.
    matrix_type slope_coeffs_;

    //! Coefficients of time in the formula.
    vector_type time_coeffs_;
};

//! Log tag of radau2a_table_generator.
constexpr auto radau2a_table_generator_log_tag = logging::log_tag_view(
    "num_collect::ode::runge_kutta::impl::radau2a_table_generator");

/*!
 * \brief Class to compute coefficients in Butcher tableau of Radau IIA method.
 *
 * \tparam Stages Number of stages.
 */
template <num_collect::index_type Stages>
class radau2a_table_generator {
public:
    // This class does not have non-static members,
    // but it is not a namespace because it has template parameters.

    //! Type of coefficients in this class.
    using scalar_type = long double;

    //! Type of matrices.
    using matrix_type = Eigen::Matrix<scalar_type, Stages, Stages>;

    //! Type of vectors.
    using vector_type = Eigen::Vector<scalar_type, Stages>;

    /*!
     * \brief Compute coefficients of Butcher tableau.
     *
     * \return Coefficients.
     */
    [[nodiscard]] static auto compute() -> radau2a_table<Stages> {
        const auto poly = generate_radau_polynomial();
        const auto time_coeffs = compute_zeros(poly);
        const auto slope_coeffs = compute_slope_coeffs(time_coeffs);

        logging::logger logger(radau2a_table_generator_log_tag);
        NUM_COLLECT_LOG_TRACE(logger,
            "Generated coefficients of Butcher tableau of Radau IIA method "
            "with {} stages",
            Stages);
        NUM_COLLECT_LOG_TRACE(logger, "Time coefficients: {:.15e}",
            util::format_dense_vector(time_coeffs));
        NUM_COLLECT_LOG_TRACE(logger, "Slope coefficients: {:.15e}",
            util::format_dense_matrix(slope_coeffs));

        return radau2a_table<Stages>(slope_coeffs, time_coeffs);
    }

private:
    /*!
     * \brief Generate Radau polynomial.
     *
     * \return Radau polynomial.
     */
    [[nodiscard]] static auto generate_radau_polynomial()
        -> num_collect::polynomials::polynomial<scalar_type> {
        const auto x_minus_one =
            num_collect::polynomials::polynomial<scalar_type>({-1.0, 1.0});
        num_collect::util::vector<scalar_type> starting_coefficients(Stages);
        starting_coefficients.back() = 1.0;
        auto result = num_collect::polynomials::polynomial<scalar_type>(
            std::move(starting_coefficients));
        for (num_collect::index_type i = 0; i < Stages; ++i) {
            result *= x_minus_one;
        }
        for (num_collect::index_type i = 0; i < Stages - 1; ++i) {
            num_collect::polynomials::differentiate(result);
        }
        return result;
    }

    /*!
     * \brief Compute real zeros of the given polynomial.
     *
     * \param[in] poly Polynomial.
     * \return Real zeros of the polynomial.
     */
    [[nodiscard]] static auto compute_zeros(
        const num_collect::polynomials::polynomial<scalar_type>& poly)
        -> Eigen::VectorX<scalar_type> {
        const auto complex_zeros =
            num_collect::polynomials::compute_zeros(poly);
        Eigen::VectorX<scalar_type> zeros(complex_zeros.size());
        for (num_collect::index_type i = 0; i < complex_zeros.size(); ++i) {
            NUM_COLLECT_DEBUG_ASSERT(std::abs(complex_zeros[i].imag()) < 1e-10);
            zeros(i) = complex_zeros[i].real();
        }
        std::sort(zeros.data(), zeros.data() + zeros.size());
        return zeros;
    }

    /*!
     * \brief Compute coefficients of intermidiate slopes in the formula from
     * the given coefficients of time.
     *
     * \param[in] time_coeffs Coefficients of time in the formula.
     * \return Coefficients of intermidiate slopes in the formula.
     */
    [[nodiscard]] static auto compute_slope_coeffs(
        const Eigen::VectorX<scalar_type>& time_coeffs)
        -> Eigen::MatrixX<scalar_type> {
        const num_collect::index_type stages = time_coeffs.size();

        Eigen::MatrixX<scalar_type> slope_coeffs(stages, stages);

        Eigen::MatrixX<scalar_type> equation_coeff(stages, stages);
        Eigen::VectorX<scalar_type> equation_rhs(stages);
        Eigen::VectorX<scalar_type> equation_solution(stages);
        Eigen::FullPivLU<Eigen::MatrixX<scalar_type>> lu;
        using std::pow;
        for (num_collect::index_type i = 0; i < stages; ++i) {
            for (num_collect::index_type q = 0; q < stages; ++q) {
                for (num_collect::index_type j = 0; j < stages; ++j) {
                    equation_coeff(q, j) = pow(time_coeffs(j), q);
                }
            }
            for (num_collect::index_type q = 0; q < stages; ++q) {
                equation_rhs(q) = pow(time_coeffs(i), q + 1) /
                    static_cast<scalar_type>(q + 1);
            }
            lu.compute(equation_coeff);
            equation_solution = lu.solve(equation_rhs);
            slope_coeffs.row(i) = equation_solution.transpose();
        }
        return slope_coeffs;
    }
};

/*!
 * \brief Get coefficients of Butcher tableau of Radau IIA method.
 *
 * \tparam Stages Number of stages.
 * \return Coefficients.
 *
 * \note This function caches the generated coefficients.
 */
template <num_collect::index_type Stages>
[[nodiscard]] auto get_radau2a_table() -> const radau2a_table<Stages>& {
    static const auto table = radau2a_table_generator<Stages>::compute();
    return table;
}

}  // namespace num_collect::ode::runge_kutta::impl
