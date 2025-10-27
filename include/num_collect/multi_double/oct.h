/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of oct class.
 */
#pragma once

#include <array>

#include "num_collect/base/concepts/implicitly_convertible_to.h"
#include "num_collect/multi_double/impl/basic_operations.h"
#include "num_collect/multi_double/impl/nine_sums.h"
#include "num_collect/multi_double/impl/oct_renormalize.h"
#include "num_collect/multi_double/impl/six_sums.h"
#include "num_collect/multi_double/impl/three_sums.h"
#include "num_collect/multi_double/quad.h"

namespace num_collect::multi_double {

/*!
 * \brief Class of octuple-precision floating-point numbers
 * using four double-precision floating-point numbers
 * \cite Hida2000.
 */
class oct {
public:
    /*!
     * \brief Constructor.
     *
     * This initializes the number to zero.
     */
    constexpr oct() noexcept = default;

    /*!
     * \brief Constructor.
     *
     * \param[in] terms Terms.
     *
     * This constructor assumes that the terms satisfy
     * \f$ |term_{i+1}| \leq 1/2 \mathrm{ulp}(term_i) \f$ for
     * \f$ i = 0, 1, 2 \f$.
     * If such condition is not satisfied,
     * the results of any operations are not accurate.
     */
    constexpr explicit oct(const std::array<double, 4>& terms) noexcept
        : terms_(terms) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] term0 0th order term.
     * \param[in] term1 1st order term.
     * \param[in] term2 2nd order term.
     * \param[in] term3 3rd order term.
     *
     * This constructor assumes that the terms satisfy
     * \f$ |term_{i+1}| \leq 1/2 \mathrm{ulp}(term_i) \f$ for
     * \f$ i = 0, 1, 2 \f$.
     * If such condition is not satisfied,
     * the results of any operations are not accurate.
     */
    constexpr oct(
        double term0, double term1, double term2 = 0.0, double term3 = 0.0)
        : terms_{term0, term1, term2, term3} {}

    /*!
     * \brief Constructor.
     *
     * \tparam Scalar Type of scalar value.
     * \param[in] value Value.
     *
     * This constructor implicitly converts integers and floating-point
     * numbers to quad numbers.
     */
    template <concepts::implicitly_convertible_to<double> Scalar>
    constexpr oct(Scalar value) noexcept  // NOLINT(*-explicit-*)
        : terms_{static_cast<double>(value), 0.0, 0.0, 0.0} {}

    /*!
     * \brief Constructor.
     *
     * \param[in] value Quadruple-precision floating-point number.
     */
    constexpr oct(const quad& value) noexcept  // NOLINT(*-explicit-*)
        : terms_{value.high(), value.low(), 0.0, 0.0} {}

    /*!
     * \brief Get a term.
     *
     * \param[in] index Index of the term.
     * \return Term at the specified index.
     */
    [[nodiscard]] constexpr auto term(std::size_t index) const noexcept
        -> double {
        return terms_[index];
    }

    /*!
     * \brief Negate this number.
     *
     * \return Negated number.
     */
    constexpr auto operator-() const noexcept -> oct {
        return oct(-terms_[0], -terms_[1], -terms_[2], -terms_[3]);
    }

    /*!
     * \brief Add a number.
     *
     * \param[in] right Right-hand-side number.
     * \return This number after calculation.
     */
    constexpr auto operator+=(const oct& right) noexcept -> oct& {
        // Last digit of the variable names indicates the order.

        // First, calculate sums for each order.
        const auto [s0_0, e0_1] = impl::two_sum(terms_[0], right.terms_[0]);
        const auto [s1_1, e1_2] = impl::two_sum(terms_[1], right.terms_[1]);
        const auto [s2_2, e2_3] = impl::two_sum(terms_[2], right.terms_[2]);
        const auto [s3_3, e3_4] = impl::two_sum(terms_[3], right.terms_[3]);

        // Second, collect terms of each order to get unnormalized results.
        const auto [u1_1, u1_2] = impl::two_sum(e0_1, s1_1);
        const auto [u2_2, u2_3, u2_4] =
            impl::three_to_three_sum(e1_2, s2_2, u1_2);
        const auto [u3_3, u3_4] = impl::three_to_two_sum(e2_3, s3_3, u2_3);
        const double u4_4 = e3_4 + u2_4 + u3_4;

        // Finally renormalize the results.
        terms_ = impl::oct_renormalize({s0_0, u1_1, u2_2, u3_3, u4_4});

        return *this;
    }

    /*!
     * \brief Subtract a number.
     *
     * \param[in] right Right-hand-side number.
     * \return This number after calculation.
     */
    constexpr auto operator-=(const oct& right) noexcept -> oct& {
        return operator+=(-right);
    }

    /*!
     * \brief Multiply with another number.
     *
     * \param[in] right Right-hand-side number.
     * \return This number after calculation.
     */
    constexpr auto operator*=(const oct& right) noexcept -> oct& {
        // Last digit of the variable names indicates the order.

        // First calculates products of terms up to 4th order.
        const auto [p00_0, p00_1] = impl::two_prod(terms_[0], right.terms_[0]);
        const auto [p01_1, p01_2] = impl::two_prod(terms_[0], right.terms_[1]);
        const auto [p02_2, p02_3] = impl::two_prod(terms_[0], right.terms_[2]);
        const auto [p03_3, p03_4] = impl::two_prod(terms_[0], right.terms_[3]);
        const auto [p10_1, p10_2] = impl::two_prod(terms_[1], right.terms_[0]);
        const auto [p11_2, p11_3] = impl::two_prod(terms_[1], right.terms_[1]);
        const auto [p12_3, p12_4] = impl::two_prod(terms_[1], right.terms_[2]);
        const double p13_4 = terms_[1] * right.terms_[3];
        const auto [p20_2, p20_3] = impl::two_prod(terms_[2], right.terms_[0]);
        const auto [p21_3, p21_4] = impl::two_prod(terms_[2], right.terms_[1]);
        const double p22_4 = terms_[2] * right.terms_[2];
        const auto [p30_3, p30_4] = impl::two_prod(terms_[3], right.terms_[0]);
        const double p31_4 = terms_[3] * right.terms_[1];

        // Second, collect terms of each order to get unnormalized results.
        const auto [u1_1, u1_2, u1_3] =
            impl::three_to_three_sum(p00_1, p01_1, p10_1);
        const auto [u2_2, u2_3, u2_4] =
            impl::six_to_three_sum(p01_2, p02_2, p10_2, p11_2, p20_2, u1_2);
        const auto [u3_3, u3_4] = impl::nine_to_two_sum(
            p02_3, p03_3, p11_3, p12_3, p20_3, p21_3, p30_3, u1_3, u2_3);
        const double u4_4 =
            p03_4 + p12_4 + p13_4 + p21_4 + p22_4 + p30_4 + p31_4 + u2_4 + u3_4;

        // Finally renormalize the results.
        terms_ = impl::oct_renormalize({p00_0, u1_1, u2_2, u3_3, u4_4});

        return *this;
    }

private:
    //! Terms.
    std::array<double, 4> terms_{0.0, 0.0, 0.0, 0.0};
};

/*!
 * \brief Add two numbers.
 *
 * \param[in] left Left-hand-side number.
 * \param[in] right Right-hand-side number.
 * \return Result.
 */
constexpr auto operator+(const oct& left, const oct& right) noexcept -> oct {
    return oct(left) += right;
}

/*!
 * \brief Subtract a number from a number.
 *
 * \param[in] left Left-hand-side number.
 * \param[in] right Right-hand-side number.
 * \return Result.
 */
constexpr auto operator-(const oct& left, const oct& right) noexcept -> oct {
    return oct(left) -= right;
}

/*!
 * \brief Multiply two numbers.
 *
 * \param[in] left Left-hand-side number.
 * \param[in] right Right-hand-side number.
 * \return Result.
 */
constexpr auto operator*(const oct& left, const oct& right) noexcept -> oct {
    return oct(left) *= right;
}

}  // namespace num_collect::multi_double
