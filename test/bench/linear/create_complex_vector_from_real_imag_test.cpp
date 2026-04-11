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
 * \brief Benchmark for creating a complex vector from real and imaginary parts.
 */
#include <Eigen/Core>
#include <stat_bench/benchmark_macros.h>

STAT_BENCH_CASE("create_complex_vector_from_real_imag", "add_two_parts") {
    const Eigen::VectorXd real_part = Eigen::VectorXd::Random(1000);
    const Eigen::VectorXd imag_part = Eigen::VectorXd::Random(1000);
    Eigen::VectorXcd complex_vector;
    complex_vector.resize(real_part.size());

    STAT_BENCH_MEASURE() {
        complex_vector = real_part.cast<std::complex<double>>() +
            std::complex<double>(0.0, 1.0) *
                imag_part.cast<std::complex<double>>();
    };
}

STAT_BENCH_CASE("create_complex_vector_from_real_imag", "coeff_wise") {
    const Eigen::VectorXd real_part = Eigen::VectorXd::Random(1000);
    const Eigen::VectorXd imag_part = Eigen::VectorXd::Random(1000);
    Eigen::VectorXcd complex_vector;
    complex_vector.resize(real_part.size());

    STAT_BENCH_MEASURE() {
        for (Eigen::Index i = 0; i < real_part.size(); ++i) {
            complex_vector(i) =
                std::complex<double>(real_part(i), imag_part(i));
        }
    };
}

STAT_BENCH_CASE("create_complex_vector_from_real_imag", "assign_each_part") {
    const Eigen::VectorXd real_part = Eigen::VectorXd::Random(1000);
    const Eigen::VectorXd imag_part = Eigen::VectorXd::Random(1000);
    Eigen::VectorXcd complex_vector;
    complex_vector.resize(real_part.size());

    STAT_BENCH_MEASURE() {
        complex_vector.real() = real_part;
        complex_vector.imag() = imag_part;
    };
}

STAT_BENCH_MAIN
