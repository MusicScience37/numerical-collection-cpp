/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Test of gamma_lanczos class.
 */
#include "num_collect/functions/impl/gamma_lanczos.h"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

#include "comparison_approvals.h"
#include "num_collect/base/constants.h"
#include "num_collect/base/index_type.h"
#include "num_collect/functions/sqrt.h"

TEST_CASE("num_collect::functions::impl::gamma_lanczos") {
    using num_collect::functions::impl::gamma_lanczos;

    SECTION("for real numbers") {
        const Eigen::VectorXd x{{// integers
            1.0, 2.0, 3.0, 4.0, 5.0,
            // real numbers
            0.5, 1.0 / 3.0, 12.3}};
        const Eigen::VectorXd reference{{// integers
            1.0, 1.0, 2.0, 6.0, 24.0,
            // real numbers
            num_collect::functions::sqrt(num_collect::pi<double>),
            2.6789385347077476337, 83385367.89997001}};

        SECTION("calculate gamma(x)") {
            Eigen::VectorXd actual = Eigen::VectorXd::Zero(x.size());
            for (num_collect::index_type i = 0; i < x.size(); ++i) {
                actual(i) = gamma_lanczos<double>::gamma(x(i));
            }

            comparison_approvals::verify_with_reference(actual, reference);
        }

        SECTION("calculate log(gamma(x))") {
            const Eigen::VectorXd log_reference = reference.array().log();

            Eigen::VectorXd actual = Eigen::VectorXd::Zero(x.size());
            for (num_collect::index_type i = 0; i < x.size(); ++i) {
                actual(i) = gamma_lanczos<double>::log_gamma(x(i));
            }

            comparison_approvals::verify_with_reference(actual, log_reference);
        }
    }

    SECTION("for complex numbers") {
        const Eigen::VectorXcd x{
            {std::complex<double>(1.0, 1.0), std::complex<double>(0.5, -0.5)}};
        const Eigen::VectorXcd reference{
            {std::complex<double>(0.498015668118356042713691117462198,
                 -0.15494982830181068512495513048),
                std::complex<double>(0.8181639995, 0.7633138287)}};

        SECTION("calculate gamma(x)") {
            Eigen::VectorXcd actual = Eigen::VectorXcd::Zero(x.size());
            for (num_collect::index_type i = 0; i < x.size(); ++i) {
                actual(i) = gamma_lanczos<double>::gamma(x(i));
            }

            SECTION("real part") {
                comparison_approvals::verify_with_reference(
                    actual.real().eval(), reference.real().eval());
            }

            SECTION("imaginary part") {
                comparison_approvals::verify_with_reference(
                    actual.imag().eval(), reference.imag().eval());
            }
        }
    }
}
