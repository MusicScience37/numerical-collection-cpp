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
 * \brief Experiment to calculate Radau IIA coefficients.
 */
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>
#include <Eigen/LU>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include "num_collect/base/index_type.h"
#include "num_collect/multi_double/quad.h"
#include "num_collect/multi_double/quad_eigen.h"  // IWYU pragma: keep
#include "num_collect/multi_double/quad_math.h"
#include "num_collect/polynomials/polynomial.h"
#include "num_collect/util/vector.h"

using scalar_type = num_collect::multi_double::quad;

[[nodiscard]] static auto generate_radau_polynomial(
    num_collect::index_type degree)
    -> num_collect::polynomials::polynomial<scalar_type> {
    const auto x_minus_one =
        num_collect::polynomials::polynomial<scalar_type>({-1.0, 1.0});
    num_collect::util::vector<scalar_type> starting_coefficients(degree);
    starting_coefficients.back() = 1.0;
    auto result = num_collect::polynomials::polynomial<scalar_type>(
        std::move(starting_coefficients));
    for (num_collect::index_type i = 0; i < degree; ++i) {
        result *= x_minus_one;
    }
    for (num_collect::index_type i = 0; i < degree - 1; ++i) {
        num_collect::polynomials::differentiate(result);
    }
    return result;
}

[[nodiscard]] static auto compute_zeros(
    const num_collect::polynomials::polynomial<scalar_type>& poly)
    -> Eigen::VectorX<scalar_type> {
    const auto& coeffs = poly.coeffs();
    Eigen::MatrixX<scalar_type> matrix =
        Eigen::MatrixX<scalar_type>::Zero(coeffs.size() - 1, coeffs.size() - 1);
    for (num_collect::index_type i = 0; i < coeffs.size() - 1; ++i) {
        matrix(0, coeffs.size() - 2 - i) = -coeffs[i] / coeffs.back();
    }
    for (num_collect::index_type i = 1; i < coeffs.size() - 1; ++i) {
        matrix(i, i - 1) = 1.0;
    }
    Eigen::EigenSolver<Eigen::MatrixX<scalar_type>> solver(matrix);
    Eigen::VectorX<scalar_type> zeros =
        solver.pseudoEigenvalueMatrix().diagonal();
    std::sort(zeros.data(), zeros.data() + zeros.size());
    return zeros;
}

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
            equation_rhs(q) =
                pow(time_coeffs(i), q + 1) / static_cast<scalar_type>(q + 1);
        }
        lu.compute(equation_coeff);
        equation_solution = lu.solve(equation_rhs);
        slope_coeffs.row(i) = equation_solution.transpose();
    }
    return slope_coeffs;
}

auto main(int argc, char** argv) -> int {
    const char* degree_str = "9";
    if (argc > 1) {
        degree_str = argv[1];
    }
    const num_collect::index_type degree = std::stoi(degree_str);

    const auto poly = generate_radau_polynomial(degree);
    const auto time_coeffs = compute_zeros(poly);
    const auto slope_coeffs = compute_slope_coeffs(time_coeffs);
    const Eigen::VectorX<scalar_type> update_coeffs =
        slope_coeffs.row(slope_coeffs.rows() - 1).transpose();
    fmt::print("Time coefficients: [{:.15e}]\n",
        fmt::join(time_coeffs.cast<double>(), ", "));
    fmt::print("Slope coefficients:\n");
    for (num_collect::index_type i = 0; i < slope_coeffs.rows(); ++i) {
        fmt::print(
            "[{:.15e}]\n", fmt::join(slope_coeffs.row(i).cast<double>(), ", "));
    }
    fmt::print("Update coefficients: [{:.15e}]\n",
        fmt::join(update_coeffs.cast<double>(), ", "));

    return 0;
}
