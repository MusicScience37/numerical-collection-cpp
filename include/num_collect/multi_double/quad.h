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
 * \brief Definition of quad class
 */
#pragma once

#include <tuple>

#include "num_collect/base/concepts/implicitly_convertible_to.h"
#include "num_collect/multi_double/impl/basic_operations.h"

namespace num_collect::multi_double {

/*!
 * \brief Class of quadruple-precision floating-point numbers
 * using two double-precision floating-point numbers
 * \cite Yamanaka2012, \cite Hida2000, \cite Yamanaka2012d.
 */
class quad {
public:
    /*!
     * \brief Constructor.
     *
     * This initializes the number to zero.
     */
    constexpr quad() noexcept = default;

    /*!
     * \brief Constructor.
     *
     * \param[in] high Higher digits.
     * \param[in] low Lower digits.
     *
     * This constructor assumes that two arguments satisfy
     * \f$ |low| \leq 1/2 \mathrm{ulp}(high) \f$.
     * If such condition is not satisfied,
     * the results of any operations are not accurate.
     * If you want to create number with two arbitrary numbers,
     * use `quad(a) + quad(b)`.
     */
    constexpr quad(double high, double low) noexcept : high_(high), low_(low) {}

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
    constexpr quad(Scalar value) noexcept  // NOLINT
        : high_(static_cast<double>(value)) {}

    /*!
     * \brief Get higher digits.
     *
     * \return Higher digits.
     */
    [[nodiscard]] auto high() const noexcept -> double { return high_; }

    /*!
     * \brief Get lower digits.
     *
     * \return Lower digits.
     */
    [[nodiscard]] auto low() const noexcept -> double { return low_; }

    /*!
     * \brief Negate this number.
     *
     * \return Negated number.
     */
    auto operator-() const noexcept -> quad { return quad(-high_, -low_); }

    /*!
     * \brief Add a number.
     *
     * \param[in] right Right-hand-side number.
     * \return This number after calculation.
     */
    auto operator+=(const quad& right) noexcept -> quad& {
        auto [x_h, x_l] = impl::two_sum(high_, right.high_);
        x_l += low_;
        x_l += right.low_;
        std::tie(high_, low_) = impl::quick_two_sum(x_h, x_l);
        return *this;
    }

    /*!
     * \brief Add a number.
     *
     * \tparam Scalar Type of the right-hand-side number.
     * \param[in] right Right-hand-side number.
     * \return This number after calculation.
     */
    template <concepts::implicitly_convertible_to<double> Scalar>
    auto operator+=(Scalar right) noexcept -> quad& {
        auto [x_h, x_l] = impl::two_sum(high_, static_cast<double>(right));
        x_l += low_;
        std::tie(high_, low_) = impl::quick_two_sum(x_h, x_l);
        return *this;
    }

    /*!
     * \brief Subtract a number.
     *
     * \param[in] right Right-hand-side number.
     * \return This number after calculation.
     */
    auto operator-=(const quad& right) noexcept -> quad& {
        return operator+=(-right);
    }

    /*!
     * \brief Subtract a number.
     *
     * \tparam Scalar Type of the right-hand-side number.
     * \param[in] right Right-hand-side number.
     * \return This number after calculation.
     */
    template <concepts::implicitly_convertible_to<double> Scalar>
    auto operator-=(Scalar right) noexcept -> quad& {
        return operator+=(-right);
    }

    /*!
     * \brief Multiply with another number.
     *
     * \param[in] right Right-hand-side number.
     * \return This number after calculation.
     */
    auto operator*=(const quad& right) noexcept -> quad& {
        auto [x_h, x_l] = impl::two_prod(high_, right.high_);
        // Calculate the sum of the cross terms first,
        // then add the sum to the lower digits.
        // This solved accuracy issues in some cases.
        x_l += high_ * right.low_ + low_ * right.high_;
        std::tie(high_, low_) = impl::quick_two_sum(x_h, x_l);
        return *this;
    }

    /*!
     * \brief Divide by another number.
     *
     * \param[in] right Right-hand-side number.
     * \return This number after calculation.
     *
     * This function does not check whether the right-hand-side number is zero.
     * If it is zero, the result can be infinity or NaN.
     */
    auto operator/=(const quad& right) noexcept -> quad& {
        const double inv_right_h = 1.0 / right.high_;
        const double rate_right = right.low_ * inv_right_h;
        const double x_h = high_ * inv_right_h;
        const auto [r_1, r_2] = impl::two_prod(x_h, right.high_);
        double x_l = ((high_ - r_1) - r_2) * inv_right_h;
        x_l += x_h * ((low_ / high_) - rate_right);
        std::tie(high_, low_) = impl::quick_two_sum(x_h, x_l);
        return *this;
    }

private:
    //! Higher digits.
    double high_{0.0};

    //! Lower digits.
    double low_{0.0};
};

/*!
 * \brief Add two numbers.
 *
 * \param[in] left Left-hand-side number.
 * \param[in] right Right-hand-side number.
 * \return Result.
 */
inline auto operator+(const quad& left, const quad& right) -> quad {
    return quad(left) += right;
}

/*!
 * \brief Add two numbers.
 *
 * \tparam Scalar Type of the right-hand-side number.
 * \param[in] left Left-hand-side number.
 * \param[in] right Right-hand-side number.
 * \return Result.
 */
template <concepts::implicitly_convertible_to<double> Scalar>
inline auto operator+(const quad& left, Scalar right) -> quad {
    return quad(left) += right;
}

/*!
 * \brief Add two numbers.
 *
 * \tparam Scalar Type of the left-hand-side number.
 * \param[in] left Left-hand-side number.
 * \param[in] right Right-hand-side number.
 * \return Result.
 */
template <concepts::implicitly_convertible_to<double> Scalar>
inline auto operator+(Scalar left, const quad& right) -> quad {
    return quad(right) += left;
}

/*!
 * \brief Subtract a number from a number.
 *
 * \param[in] left Left-hand-side number.
 * \param[in] right Right-hand-side number.
 * \return Result.
 */
inline auto operator-(const quad& left, const quad& right) -> quad {
    return quad(left) -= right;
}

/*!
 * \brief Subtract a number from a number.
 *
 * \tparam Scalar Type of the right-hand-side number.
 * \param[in] left Left-hand-side number.
 * \param[in] right Right-hand-side number.
 * \return Result.
 */
template <concepts::implicitly_convertible_to<double> Scalar>
inline auto operator-(const quad& left, Scalar right) -> quad {
    return quad(left) -= right;
}

/*!
 * \brief Subtract a number from a number.
 *
 * \tparam Scalar Type of the left-hand-side number.
 * \param[in] left Left-hand-side number.
 * \param[in] right Right-hand-side number.
 * \return Result.
 */
template <concepts::implicitly_convertible_to<double> Scalar>
inline auto operator-(Scalar left, const quad& right) -> quad {
    return -quad(right) += left;
}

/*!
 * \brief Multiply a number by a number.
 *
 * \param[in] left Left-hand-side number.
 * \param[in] right Right-hand-side number.
 * \return Result.
 */
inline auto operator*(const quad& left, const quad& right) -> quad {
    return quad(left) *= right;
}

/*!
 * \brief Divide a number by a number.
 *
 * \param[in] left Left-hand-side number.
 * \param[in] right Right-hand-side number.
 * \return Result.
 *
 * This function does not check whether the right-hand-side number is zero.
 * If it is zero, the result can be infinity or NaN.
 */
inline auto operator/(const quad& left, const quad& right) -> quad {
    return quad(left) /= right;
}

}  // namespace num_collect::multi_double
