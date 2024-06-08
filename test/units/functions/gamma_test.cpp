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
 * \brief Test of gamma function.
 */
#include "num_collect/functions/gamma.h"

#include <complex>

#include <Eigen/Core>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include "comparison_approvals.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/pi.h"  // IWYU pragma: keep
#include "num_collect/constants/sqrt.h"

TEMPLATE_TEST_CASE("num_collect::functions::gamma", "", float, double) {
    using num_collect::functions::gamma;

    SECTION("for real numbers") {
        const Eigen::VectorX<TestType> x{{// integers
            static_cast<TestType>(1.0), static_cast<TestType>(2.0),
            static_cast<TestType>(3.0), static_cast<TestType>(4.0),
            static_cast<TestType>(5.0),
            // real numbers
            static_cast<TestType>(0.5), static_cast<TestType>(-0.5),
            static_cast<TestType>(1.0) / static_cast<TestType>(3.0),
            static_cast<TestType>(12.3), static_cast<TestType>(-12.3)}};
        const Eigen::VectorX<TestType> reference{{// integers
            static_cast<TestType>(1.0), static_cast<TestType>(1.0),
            static_cast<TestType>(2.0), static_cast<TestType>(6.0),
            static_cast<TestType>(24.0),
            // real numbers
            num_collect::constants::sqrt(num_collect::constants::pi<TestType>),
            static_cast<TestType>(-2.0) *
                num_collect::constants::sqrt(
                    num_collect::constants::pi<TestType>),
            static_cast<TestType>(2.6789385347077476337),
            static_cast<TestType>(83385367.89997001),
            static_cast<TestType>(-3.786145218746391e-09)}};

        Eigen::VectorX<TestType> actual =
            Eigen::VectorX<TestType>::Zero(x.size());
        for (num_collect::index_type i = 0; i < x.size(); ++i) {
            actual(i) = gamma(x(i));
        }

        comparison_approvals::verify_with_reference(actual, reference);
    }

    SECTION("for complex numbers") {
        const Eigen::VectorX<std::complex<TestType>> x{
            {std::complex<TestType>(1.0, 1.0),
                std::complex<TestType>(0.5, -0.5)}};
        const Eigen::VectorX<std::complex<TestType>> reference{
            {std::complex<TestType>(
                 static_cast<TestType>(0.498015668118356042713691117462198),
                 static_cast<TestType>(-0.15494982830181068512495513048)),
                std::complex<TestType>(static_cast<TestType>(0.8181639995),
                    static_cast<TestType>(0.7633138287))}};

        Eigen::VectorX<std::complex<TestType>> actual =
            Eigen::VectorX<std::complex<TestType>>::Zero(x.size());
        for (num_collect::index_type i = 0; i < x.size(); ++i) {
            actual(i) = gamma(x(i));
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

TEMPLATE_TEST_CASE("num_collect::functions::log_gamma", "", float, double) {
    using num_collect::functions::log_gamma;

    SECTION("for real numbers") {
        const Eigen::VectorX<TestType> x{{// integers
            static_cast<TestType>(1.0), static_cast<TestType>(2.0),
            static_cast<TestType>(3.0), static_cast<TestType>(4.0),
            static_cast<TestType>(5.0),
            // real numbers
            static_cast<TestType>(0.5),
            static_cast<TestType>(1.0) / static_cast<TestType>(3.0),
            static_cast<TestType>(12.3)}};
        const Eigen::VectorX<TestType> reference{{
            // integers
            static_cast<TestType>(1.0),
            static_cast<TestType>(1.0),
            static_cast<TestType>(2.0),
            static_cast<TestType>(6.0),
            static_cast<TestType>(24.0),
            // real numbers
            num_collect::constants::sqrt(num_collect::constants::pi<TestType>),
            static_cast<TestType>(2.6789385347077476337),
            static_cast<TestType>(83385367.89997001),
        }};
        const Eigen::VectorX<TestType> log_reference = reference.array().log();

        Eigen::VectorX<TestType> actual =
            Eigen::VectorX<TestType>::Zero(x.size());
        for (num_collect::index_type i = 0; i < x.size(); ++i) {
            actual(i) = log_gamma(x(i));
        }

        comparison_approvals::verify_with_reference(actual, log_reference);
    }
}
