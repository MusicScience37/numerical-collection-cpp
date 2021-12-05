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

#include <type_traits>

#include "num_collect/multi_double/impl/basic_operations.h"

namespace num_collect::multi_double {

/*!
 * \brief class of quadruple precision floating-point numbers
 */
class quad {
public:
    /*!
     * \brief construct zero
     */
    constexpr quad() noexcept = default;

    /*!
     * \brief construct
     *
     * \param[in] high higher digits
     * \param[in] low lower digits
     */
    constexpr quad(double high, double low) noexcept : high_(high), low_(low) {}

    /*!
     * \brief convert implicitly
     *
     * \tparam Scalar type of scalar value
     * \param[in] value value
     */
    template <typename Scalar,
        std::enable_if_t<(std::is_integral_v<Scalar> ||
            std::is_floating_point_v<Scalar>)&&(!std::is_same_v<Scalar,
                                                quad>)>* = nullptr>
    constexpr quad(Scalar value) noexcept  // NOLINT
        : high_(static_cast<double>(value)) {}

    /*!
     * \brief get higher digits
     *
     * \return higher digits
     */
    [[nodiscard]] auto high() const noexcept -> double { return high_; }

    /*!
     * \brief get lower digits
     *
     * \return lower digits
     */
    [[nodiscard]] auto low() const noexcept -> double { return low_; }

    /*!
     * \brief negate this number
     *
     * \return negated number
     */
    auto operator-() const noexcept -> quad { return quad(-high_, -low_); }

    /*!
     * \brief add another number
     *
     * \param[in] right another number
     * \return this
     */
    auto operator+=(const quad& right) noexcept -> quad& {
        auto [x_h, x_l] = impl::two_sum(high_, right.high_);
        x_l += low_;
        x_l += right.low_;
        std::tie(high_, low_) = impl::quick_two_sum(x_h, x_l);
        return *this;
    }

    /*!
     * \brief subtract another number
     *
     * \param[in] right another number
     * \return this
     */
    auto operator-=(const quad& right) noexcept -> quad& {
        return operator+=(-right);
    }

    /*!
     * \brief multiply with another number
     *
     * \param[in] right another number
     * \return this
     */
    auto operator*=(const quad& right) noexcept -> quad& {
        auto [x_h, x_l] = impl::two_prod(high_, right.high_);
        x_l += high_ * right.low_;
        x_l += low_ * right.high_;
        std::tie(high_, low_) = impl::quick_two_sum(x_h, x_l);
        return *this;
    }

    /*!
     * \brief divide by another number
     *
     * \param[in] right another number
     * \return this
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
    //! higher digits
    double high_{0.0};

    //! lower digits
    double low_{0.0};
};

/*!
 * \brief add two number
 *
 * \param[in] left left-hand-side number
 * \param[in] right right-hand-side number
 * \return result
 */
inline auto operator+(const quad& left, const quad& right) -> quad {
    return quad(left) += right;
}

/*!
 * \brief substruct a number from a number
 *
 * \param[in] left left-hand-side number
 * \param[in] right right-hand-side number
 * \return result
 */
inline auto operator-(const quad& left, const quad& right) -> quad {
    return quad(left) -= right;
}

/*!
 * \brief multiply a number by a number
 *
 * \param[in] left left-hand-side number
 * \param[in] right right-hand-side number
 * \return result
 */
inline auto operator*(const quad& left, const quad& right) -> quad {
    return quad(left) *= right;
}

/*!
 * \brief divide a number by a number
 *
 * \param[in] left left-hand-side number
 * \param[in] right right-hand-side number
 * \return result
 */
inline auto operator/(const quad& left, const quad& right) -> quad {
    return quad(left) /= right;
}

}  // namespace num_collect::multi_double
