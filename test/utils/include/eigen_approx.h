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
 * \brief Test of eigen_approx function.
 */
#pragma once

#include <cmath>
#include <sstream>

#include <Eigen/Core>
#include <catch2/matchers/catch_matchers_templated.hpp>

/*!
 * \brief Class of matcher in Catch2 library for Eigen library's vectors and
 * matrices.
 *
 * \tparam Derived Type of derived class from Eigen::DenseBase class.
 */
template <typename Derived>
class eigen_approx_matcher : public Catch::Matchers::MatcherGenericBase {
public:
    //! Type of scalars.
    using Scalar = typename Derived::Scalar;

    /*!
     * \brief Constructor.
     *
     * \param[in] expected Expected matrix.
     * \param[in] precision Precision.
     */
    eigen_approx_matcher(
        const Eigen::DenseBase<Derived>& expected, const Scalar& precision)
        : expected_(expected), precision_(precision) {}

    /*!
     * \brief Check whether two matrices are same.
     *
     * \tparam OtherDerived Type of derived class from Eigen::DenseBase class.
     * \param[in] actual Actual matrix.
     * \return Whether two matrices are same.
     */
    template <typename OtherDerived>
    [[nodiscard]] auto match(const Eigen::DenseBase<OtherDerived>& actual) const
        -> bool {
        if (expected_.cols() != actual.cols()) {
            return false;
        }
        if (expected_.rows() != actual.rows()) {
            return false;
        }
        for (Eigen::Index i = 0; i < expected_.rows(); ++i) {
            for (Eigen::Index j = 0; j < expected_.cols(); ++j) {
                using std::abs;
                if (abs(expected_.coeff(i, j) - actual.coeff(i, j)) >
                    precision_) {
                    return false;
                }
            }
        }
        return true;
    }

    /*!
     * \brief Get description.
     *
     * \return Description.
     */
    [[nodiscard]] auto describe() const -> std::string override {
        std::ostringstream stream;
        stream << "Is approximately (precision: " << precision_
               << ") equal to: " << expected_;
        return stream.str();
    }

private:
    //! Expected matrix.
    const Eigen::DenseBase<Derived>& expected_;

    //! Precision.
    const Scalar& precision_;
};

/*!
 * \brief Create matcher in Catch2 library for Eigen library's vectors and
 * matrices.
 *
 * \tparam Derived Type of derived class from Eigen::DenseBase class.
 * \param[in] expected Expected matrix.
 * \param[in] precision Precision.
 * \return Matcher.
 */
template <typename Derived>
auto eigen_approx(const Eigen::DenseBase<Derived>& expected,
    const typename Derived::Scalar& precision =
        Eigen::NumTraits<typename Derived::Scalar>::dummy_precision()) {
    return eigen_approx_matcher<Derived>(expected, precision);
}
