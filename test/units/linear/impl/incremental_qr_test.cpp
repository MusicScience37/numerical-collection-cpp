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
 * \brief Test of incremental_qr class.
 */
#include "num_collect/linear/impl/incremental_qr.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "eigen_approx.h"
#include "num_collect/base/index_type.h"

TEST_CASE("num_collect::linear::impl::compute_givens_rotation_coefficients") {
    using num_collect::linear::impl::compute_givens_rotation_coefficients;

    SECTION("a=0, b=0") {
        const auto [c, s] = compute_givens_rotation_coefficients(0.0, 0.0);
        CHECK(c == 1.0);
        CHECK(s == 0.0);
    }

    SECTION("b=0") {
        const auto [c, s] = compute_givens_rotation_coefficients(1.23, 0.0);
        CHECK(c == 1.0);
        CHECK(s == 0.0);
    }

    SECTION("a=0") {
        const auto [c, s] = compute_givens_rotation_coefficients(0.0, 1.23);
        CHECK(c == 0.0);
        CHECK(s == 1.0);
    }

    SECTION("general case with |a| < |b|") {
        const Eigen::Vector2d input_vector{1.23, 4.56};
        const auto [c, s] = compute_givens_rotation_coefficients(
            input_vector(0), input_vector(1));

        const Eigen::Matrix2d rotation_matrix{{c, s}, {-s, c}};
        const Eigen::Vector2d rotated_vector =
            rotation_matrix.transpose() * input_vector;
        INFO("rotated_vector = " << rotated_vector.transpose());
        CHECK_THAT(rotated_vector(1), Catch::Matchers::WithinAbs(0.0, 1e-10));
    }

    SECTION("general case with |a| >= |b|") {
        const Eigen::Vector2d input_vector{4.56, 1.23};
        const auto [c, s] = compute_givens_rotation_coefficients(
            input_vector(0), input_vector(1));

        const Eigen::Matrix2d rotation_matrix{{c, s}, {-s, c}};
        const Eigen::Vector2d rotated_vector =
            rotation_matrix.transpose() * input_vector;
        INFO("rotated_vector = " << rotated_vector.transpose());
        CHECK_THAT(rotated_vector(1), Catch::Matchers::WithinAbs(0.0, 1e-10));
    }
}

TEST_CASE("num_collect::linear::impl::incremental_qr") {
    using num_collect::linear::impl::incremental_qr;

    SECTION("compute the first column") {
        incremental_qr<double> qr;

        const Eigen::MatrixXd input_matrix{{1.0}, {1.0}};
        qr.initialize(3, 2);
        qr.append_column(input_matrix.col(0));

        const Eigen::MatrixXd q = qr.q();
        const Eigen::MatrixXd r = qr.r();
        CHECK(q.rows() == 2);
        CHECK(q.cols() == 2);
        CHECK(r.rows() == 2);
        CHECK(r.cols() == 1);

        // Check that q is orthogonal.
        const Eigen::MatrixXd qtq = q.transpose() * q;
        const Eigen::MatrixXd expected_qtq = Eigen::MatrixXd::Identity(2, 2);
        CHECK_THAT(qtq, eigen_approx(expected_qtq));
        const Eigen::MatrixXd qqt = q * q.transpose();
        const Eigen::MatrixXd expected_qqt = Eigen::MatrixXd::Identity(2, 2);
        CHECK_THAT(qqt, eigen_approx(expected_qqt));

        // Check that r is upper triangular.
        for (num_collect::index_type i = 0; i < r.rows(); ++i) {
            for (num_collect::index_type j = 0; j < r.cols(); ++j) {
                if (i > j) {
                    INFO("r(" << i << ", " << j << ") = " << r(i, j));
                    CHECK_THAT(r(i, j), Catch::Matchers::WithinAbs(0.0, 1e-10));
                }
            }
        }

        // Check that q * r equals the input matrix.
        const Eigen::MatrixXd qr_product = q * r;
        CHECK_THAT(qr_product, eigen_approx(input_matrix));
    }

    SECTION("compute two columns") {
        incremental_qr<double> qr;

        const Eigen::MatrixXd input_matrix{{1.0, 1.0}, {2.0, 2.0}, {0.0, 3.0}};
        qr.initialize(3, 2);
        qr.append_column(input_matrix.col(0).head(2));
        qr.append_column(input_matrix.col(1).head(3));

        const Eigen::MatrixXd q = qr.q();
        const Eigen::MatrixXd r = qr.r();
        CHECK(q.rows() == 3);
        CHECK(q.cols() == 3);
        CHECK(r.rows() == 3);
        CHECK(r.cols() == 2);

        // Check that q is orthogonal.
        const Eigen::MatrixXd qtq = q.transpose() * q;
        const Eigen::MatrixXd expected_qtq = Eigen::MatrixXd::Identity(3, 3);
        CHECK_THAT(qtq, eigen_approx(expected_qtq));
        const Eigen::MatrixXd qqt = q * q.transpose();
        const Eigen::MatrixXd expected_qqt = Eigen::MatrixXd::Identity(3, 3);
        CHECK_THAT(qqt, eigen_approx(expected_qqt));

        // Check that r is upper triangular.
        for (num_collect::index_type i = 0; i < r.rows(); ++i) {
            for (num_collect::index_type j = 0; j < r.cols(); ++j) {
                if (i > j) {
                    INFO("r(" << i << ", " << j << ") = " << r(i, j));
                    CHECK_THAT(r(i, j), Catch::Matchers::WithinAbs(0.0, 1e-10));
                }
            }
        }

        // Check that q * r equals the input matrix.
        const Eigen::MatrixXd qr_product = q * r;
        CHECK_THAT(qr_product, eigen_approx(input_matrix));
    }

    SECTION("compute three columns") {
        incremental_qr<double> qr;

        const Eigen::MatrixXd input_matrix{
            {1.0, 1.0, 1.0}, {2.0, 2.0, 2.0}, {0.0, 3.0, 3.0}, {0.0, 0.0, 4.0}};
        qr.initialize(4, 3);
        qr.append_column(input_matrix.col(0).head(2));
        qr.append_column(input_matrix.col(1).head(3));
        qr.append_column(input_matrix.col(2).head(4));

        const Eigen::MatrixXd q = qr.q();
        const Eigen::MatrixXd r = qr.r();
        CHECK(q.rows() == 4);
        CHECK(q.cols() == 4);
        CHECK(r.rows() == 4);
        CHECK(r.cols() == 3);

        // Check that q is orthogonal.
        const Eigen::MatrixXd qtq = q.transpose() * q;
        const Eigen::MatrixXd expected_qtq = Eigen::MatrixXd::Identity(4, 4);
        CHECK_THAT(qtq, eigen_approx(expected_qtq));
        const Eigen::MatrixXd qqt = q * q.transpose();
        const Eigen::MatrixXd expected_qqt = Eigen::MatrixXd::Identity(4, 4);
        CHECK_THAT(qqt, eigen_approx(expected_qqt));

        // Check that r is upper triangular.
        for (num_collect::index_type i = 0; i < r.rows(); ++i) {
            for (num_collect::index_type j = 0; j < r.cols(); ++j) {
                if (i > j) {
                    INFO("r(" << i << ", " << j << ") = " << r(i, j));
                    CHECK_THAT(r(i, j), Catch::Matchers::WithinAbs(0.0, 1e-10));
                }
            }
        }

        // Check that q * r equals the input matrix.
        const Eigen::MatrixXd qr_product = q * r;
        CHECK_THAT(qr_product, eigen_approx(input_matrix));
    }
}
