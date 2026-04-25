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
 * \brief Implementation of generate_radau2a_coeffs_impl function.
 */
#include "num_collect/ode/runge_kutta/impl/generate_radau2a_coeffs_impl.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <Eigen/Core>
#include <Eigen/LU>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logger.h"
#include "num_collect/multi_double/quad.h"
#include "num_collect/multi_double/quad_math.h"  // IWYU pragma: keep
#include "num_collect/polynomials/compute_polynomial_zeros.h"
#include "num_collect/polynomials/polynomial.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/format_dense_matrix.h"
#include "num_collect/util/format_dense_vector.h"
#include "num_collect/util/vector.h"

namespace num_collect::ode::runge_kutta::impl {

//! Namespace for internal implementation of Radau IIA method.
namespace radau2a_impl {

//! Type of scalars.
using scalar_type = long double;

/*!
 * \brief Generate Radau polynomial.
 *
 * \param[in] stages Number of stages.
 * \return Radau polynomial.
 */
static auto generate_radau_polynomial(index_type stages)
    -> num_collect::polynomials::polynomial<scalar_type> {
    const auto x_minus_one =
        num_collect::polynomials::polynomial<scalar_type>({-1.0, 1.0});
    num_collect::util::vector<scalar_type> starting_coefficients(stages);
    starting_coefficients.back() = 1.0;
    auto result = num_collect::polynomials::polynomial<scalar_type>(
        std::move(starting_coefficients));
    for (index_type i = 0; i < stages; ++i) {
        result *= x_minus_one;
    }
    for (index_type i = 0; i < stages - 1; ++i) {
        num_collect::polynomials::differentiate(result);
    }
    return result;
}

/*!
 * \brief Make a zero of the polynomial more precise by Newton's method.
 *
 * \param[in] poly Polynomial.
 * \param[in,out] zero Zero of the polynomial to be made more precise.
 */
static void make_zero_preciser(
    const num_collect::polynomials::polynomial<scalar_type>& poly,
    scalar_type& zero) {
    using num_collect::multi_double::quad;
    quad solution = zero;
    constexpr num_collect::index_type max_iterations = 10;
    for (num_collect::index_type i = 0; i < max_iterations; ++i) {
        quad value = quad(0.0);
        quad derivative = quad(0.0);
        const auto& coeffs = poly.coeffs();
        for (num_collect::index_type j = 0; j < coeffs.size(); ++j) {
            value += coeffs[j] * pow(solution, j);
            if (j > 0) {
                // Here uses double for compatibility with quad class.
                derivative +=
                    static_cast<double>(j) * coeffs[j] * pow(solution, j - 1);
            }
        }
        quad update = -value / derivative;
        solution += update;
        // Solution is in the range [0, 1], so relative error is not used.
        if (static_cast<scalar_type>(std::abs(update.high())) <
            std::numeric_limits<scalar_type>::epsilon()) {
            break;
        }
    }
    zero = static_cast<scalar_type>(solution.high()) +
        static_cast<scalar_type>(solution.low());
}

/*!
 * \brief Compute real zeros of the given polynomial.
 *
 * \param[in] poly Polynomial.
 * \return Real zeros of the polynomial.
 */
static auto compute_zeros(
    const num_collect::polynomials::polynomial<scalar_type>& poly)
    -> Eigen::VectorX<scalar_type> {
    const auto complex_zeros = num_collect::polynomials::compute_zeros(poly);
    Eigen::VectorX<scalar_type> zeros(complex_zeros.size());
    for (index_type i = 0; i < complex_zeros.size(); ++i) {
        NUM_COLLECT_DEBUG_ASSERT(std::abs(complex_zeros[i].imag()) < 1e-10);
        zeros(i) = complex_zeros[i].real();
    }
    std::sort(zeros.data(), zeros.data() + zeros.size());
    for (scalar_type& zero : zeros) {
        make_zero_preciser(poly, zero);
    }
    return zeros;
}

/*!
 * \brief Compute coefficients of intermediate slopes in the formula from
 * the given coefficients of time.
 *
 * \param[in] time_coeffs Coefficients of time in the formula.
 * \return Coefficients of intermediate slopes in the formula.
 */
static auto compute_slope_coeffs(const Eigen::VectorX<scalar_type>& time_coeffs)
    -> Eigen::MatrixX<scalar_type> {
    const index_type stages = time_coeffs.size();

    Eigen::MatrixX<scalar_type> slope_coeffs(stages, stages);

    Eigen::MatrixX<scalar_type> equation_coeff(stages, stages);
    Eigen::VectorX<scalar_type> equation_rhs(stages);
    Eigen::VectorX<scalar_type> equation_solution(stages);
    Eigen::FullPivLU<Eigen::MatrixX<scalar_type>> lu;
    using std::pow;
    for (index_type i = 0; i < stages; ++i) {
        for (index_type q = 0; q < stages; ++q) {
            for (index_type j = 0; j < stages; ++j) {
                equation_coeff(q, j) = pow(time_coeffs(j), q);
            }
        }
        for (index_type q = 0; q < stages; ++q) {
            equation_rhs(q) =
                pow(time_coeffs(i), q + 1) / static_cast<scalar_type>(q + 1);
        }
        lu.compute(equation_coeff);
        equation_solution = lu.solve(equation_rhs);
        slope_coeffs.row(i) = equation_solution.transpose();
    }
    return slope_coeffs;
}

}  // namespace radau2a_impl

void generate_radau2a_coeffs_impl(
    index_type stages, long double* slope_coeffs, long double* time_coeffs) {
    const auto poly = radau2a_impl::generate_radau_polynomial(stages);
    const auto time_coeffs_vector = radau2a_impl::compute_zeros(poly);
    const auto slope_coeffs_matrix =
        radau2a_impl::compute_slope_coeffs(time_coeffs_vector);

    logging::logger logger(radau2a_table_generator_log_tag);
    NUM_COLLECT_LOG_TRACE(logger,
        "Generated coefficients of Butcher tableau of Radau IIA method "
        "with {} stages",
        stages);
    NUM_COLLECT_LOG_TRACE(logger, "Time coefficients: {:.15e}",
        util::format_dense_vector(time_coeffs_vector));
    NUM_COLLECT_LOG_TRACE(logger, "Slope coefficients: {:.15e}",
        util::format_dense_matrix(slope_coeffs_matrix));

    Eigen::Map<Eigen::Matrix<long double, Eigen::Dynamic, Eigen::Dynamic,
        Eigen::ColMajor>>
        slope_coeffs_map(slope_coeffs, stages, stages);
    Eigen::Map<Eigen::VectorX<long double>> time_coeffs_map(
        time_coeffs, stages);
    slope_coeffs_map = slope_coeffs_matrix;
    time_coeffs_map = time_coeffs_vector;
}

}  // namespace num_collect::ode::runge_kutta::impl
