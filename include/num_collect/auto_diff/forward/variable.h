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
 * \brief Definition of variable class.
 */
#pragma once

#include <limits>
#include <optional>
#include <type_traits>

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/is_eigen_matrix.h"
#include "num_collect/util/is_eigen_vector.h"

namespace num_collect::auto_diff::forward {

/*!
 * \brief Class of variables in forward-mode automatic differentiation
 *        \cite Kubota1998.
 *
 * \tparam Value Type of values.
 * \tparam Diff Type of differential coefficients
 *              (scalars, vectors, or matrices).
 */
template <base::concepts::real_scalar Value, typename Diff = Value>
class variable {
public:
    //! Type of values.
    using value_type = Value;

    //! Type of differential coefficients.
    using diff_type = Diff;

    /*!
     * \brief Construct.
     *
     * \param[in] value Value.
     * \param[in] diff Differential coefficients.
     */
    template <typename DiffArg>
    variable(const value_type& value, DiffArg&& diff)
        : value_(value), diff_(std::forward<DiffArg>(diff)) {}

    /*!
     * \brief Construct.
     *
     * \param[in] value Value.
     */
    variable(const value_type& value)  // NOLINT: implicit convertion required
        : value_(value) {}

    /*!
     * \brief Construct.
     */
    variable() : variable(static_cast<value_type>(0)) {}

    /*!
     * \brief Get the value.
     *
     * \return Value.
     */
    [[nodiscard]] auto value() const noexcept -> const value_type& {
        return value_;
    }

    /*!
     * \brief Get whether this variable has differential coefficients.
     *
     * \return Whether this variable has differential coefficients.
     */
    [[nodiscard]] auto has_diff() const noexcept -> bool {
        return diff_.has_value();
    }

    /*!
     * \brief Get the differential coefficients.
     *
     * \return Differential coefficients.
     */
    [[nodiscard]] auto diff() const -> const diff_type& {
        NUM_COLLECT_ASSERT(diff_);
        return *diff_;
    }

    /*!
     * \brief Negate this variable.
     *
     * \return Negated variable.
     */
    auto operator-() const -> variable {
        if (diff_) {
            return variable(-value_, -*diff_);
        }
        return variable(-value_);
    }

    /*!
     * \brief Add a variable.
     *
     * \param[in] right Right-hand-side variable.
     * \return This.
     */
    auto operator+=(const variable& right) -> variable& {
        value_ += right.value_;
        if (right.diff_) {
            if (diff_) {
                *diff_ += *right.diff_;
            } else {
                diff_ = *right.diff_;
            }
        }
        return *this;
    }

    /*!
     * \brief Subtract a variable.
     *
     * \param[in] right Right-hand-side variable.
     * \return This.
     */
    auto operator-=(const variable& right) -> variable& {
        value_ -= right.value_;
        if (right.diff_) {
            if (diff_) {
                *diff_ -= *right.diff_;
            } else {
                diff_ = -*right.diff_;
            }
        }
        return *this;
    }

    /*!
     * \brief Multiply a variable.
     *
     * \param[in] right Right-hand-side variable.
     * \return This.
     */
    auto operator*=(const variable& right) -> variable& {
        if (right.diff_) {
            if (diff_) {
                *diff_ *= right.value_;
                *diff_ += value_ * (*right.diff_);
            } else {
                diff_ = value_ * (*right.diff_);
            }
        } else {
            if (diff_) {
                *diff_ *= right.value_;
            }
        }
        value_ *= right.value_;
        return *this;
    }

    /*!
     * \brief Divide by a variable.
     *
     * \param[in] right Right-hand-side variable.
     * \return This.
     */
    auto operator/=(const variable& right) -> variable& {
        value_ /= right.value_;
        if (right.diff_) {
            if (diff_) {
                *diff_ = ((*diff_) - (*right.diff_) * value_) / right.value_;
            } else {
                diff_ = -(*right.diff_) * value_ / right.value_;
            }
        } else {
            if (diff_) {
                *diff_ = (*diff_) / right.value_;
            }
        }
        return *this;
    }

private:
    //! Value.
    value_type value_;

    //! Differential coefficients.
    std::optional<diff_type> diff_{};
};

/*!
 * \brief Add two variables.
 *
 * \tparam Value Type of value.
 * \tparam Diff Type of differential coefficients.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Sum.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto operator+(const variable<Value, Diff>& left,
    const variable<Value, Diff>& right) -> variable<Value, Diff> {
    return variable<Value, Diff>(left) += right;
}

/*!
 * \brief Add two variables.
 *
 * \tparam Value Type of value.
 * \tparam Diff Type of differential coefficients.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Sum.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto operator+(const Value& left,
    const variable<Value, Diff>& right) -> variable<Value, Diff> {
    return variable<Value, Diff>(left) += right;
}

/*!
 * \brief Add two variables.
 *
 * \tparam Value Type of value.
 * \tparam Diff Type of differential coefficients.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Sum.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto operator+(const variable<Value, Diff>& left,
    const Value& right) -> variable<Value, Diff> {
    return variable<Value, Diff>(left) += right;
}

/*!
 * \brief Subtract a variable from another variable.
 *
 * \tparam Value Type of value.
 * \tparam Diff Type of differential coefficients.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Difference.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto operator-(const variable<Value, Diff>& left,
    const variable<Value, Diff>& right) -> variable<Value, Diff> {
    return variable<Value, Diff>(left) -= right;
}

/*!
 * \brief Subtract a variable from another variable.
 *
 * \tparam Value Type of value.
 * \tparam Diff Type of differential coefficients.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Difference.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto operator-(const Value& left,
    const variable<Value, Diff>& right) -> variable<Value, Diff> {
    return variable<Value, Diff>(left) -= right;
}

/*!
 * \brief Subtract a variable from another variable.
 *
 * \tparam Value Type of value.
 * \tparam Diff Type of differential coefficients.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Difference.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto operator-(const variable<Value, Diff>& left,
    const Value& right) -> variable<Value, Diff> {
    return variable<Value, Diff>(left) -= right;
}

/*!
 * \brief Multiply two variables.
 *
 * \tparam Value Type of value.
 * \tparam Diff Type of differential coefficients.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Product.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto operator*(const variable<Value, Diff>& left,
    const variable<Value, Diff>& right) -> variable<Value, Diff> {
    return variable<Value, Diff>(left) *= right;
}

/*!
 * \brief Multiply two variables.
 *
 * \tparam Value Type of value.
 * \tparam Diff Type of differential coefficients.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Product.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto operator*(const Value& left,
    const variable<Value, Diff>& right) -> variable<Value, Diff> {
    return variable<Value, Diff>(left) *= right;
}

/*!
 * \brief Multiply two variables.
 *
 * \tparam Value Type of value.
 * \tparam Diff Type of differential coefficients.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Product.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto operator*(const variable<Value, Diff>& left,
    const Value& right) -> variable<Value, Diff> {
    return variable<Value, Diff>(left) *= right;
}

/*!
 * \brief Divide a variable by another variable.
 *
 * \tparam Value Type of value.
 * \tparam Diff Type of differential coefficients.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Quotient.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto operator/(const variable<Value, Diff>& left,
    const variable<Value, Diff>& right) -> variable<Value, Diff> {
    return variable<Value, Diff>(left) /= right;
}

/*!
 * \brief Divide a variable by another variable.
 *
 * \tparam Value Type of value.
 * \tparam Diff Type of differential coefficients.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Quotient.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto operator/(const Value& left,
    const variable<Value, Diff>& right) -> variable<Value, Diff> {
    return variable<Value, Diff>(left) /= right;
}

/*!
 * \brief Divide a variable by another variable.
 *
 * \tparam Value Type of value.
 * \tparam Diff Type of differential coefficients.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Quotient.
 */
template <typename Value, typename Diff>
[[nodiscard]] inline auto operator/(const variable<Value, Diff>& left,
    const Value& right) -> variable<Value, Diff> {
    return variable<Value, Diff>(left) /= right;
}

}  // namespace num_collect::auto_diff::forward

namespace Eigen {

/*!
 * \brief Specialization of Eigen::NumTraits for
 * num_collect::auto_diff::forward::variable.
 *
 * See
 * [Reference](https://eigen.tuxfamily.org/dox/structEigen_1_1NumTraits.html)
 * for description.
 *
 * \tparam Value Type of values.
 * \tparam Diff Type of differential coefficients.
 */
template <typename Value, typename Diff>
struct NumTraits<num_collect::auto_diff::forward::variable<Value, Diff>> {
    //! Type of the variable.
    using Real = num_collect::auto_diff::forward::variable<Value, Diff>;

    //! Type of the variable.
    using NonInteger = Real;

    //! Type of the variable.
    using Literal = Real;

    //! Type of the variable.
    using Nested = Real;

    enum {
        IsInteger = 0,              // NOLINT
        IsSigned = 1,               // NOLINT
        IsComplex = 0,              // NOLINT
        RequireInitialization = 1,  // NOLINT
        ReadCost = 1,               // NOLINT
        AddCost = 2,                // NOLINT
        MulCost = 4                 // NOLINT
    };

    /*!
     * \brief Get machine epsilon.
     *
     * \return Machine epsilon.
     */
    static constexpr auto epsilon() -> Real {
        return NumTraits<Value>::epsilon();
    }

    /*!
     * \brief Get dummy precision.
     *
     * \return Dummy precision.
     */
    static constexpr auto dummy_precision() -> Real {
        return NumTraits<Value>::dummy_precision();
    }

    /*!
     * \brief Get the highest value.
     *
     * \return Highest value.
     */
    static constexpr auto highest() -> Real {
        return NumTraits<Value>::highest();
    }

    /*!
     * \brief Get the lowest value.
     *
     * \return Lowest value.
     */
    static constexpr auto lowest() -> Real {
        return NumTraits<Value>::lowest();
    }

    /*!
     * \brief Get the number of digits.
     *
     * \return Number of digits.
     */
    static constexpr auto digits10() -> int {
        return NumTraits<Value>::digits10();
    }

    /*!
     * \brief Get the infinity.
     *
     * \return Infinity.
     */
    static constexpr auto infinity() -> Real {
        return NumTraits<Value>::infinity();
    }

    /*!
     * \brief Get the quiet NaN value.
     *
     * \return Quiet NaN value.
     */
    static constexpr auto quiet_NaN() -> Real {  // NOLINT
        return NumTraits<Value>::quiet_NaN();
    }
};

}  // namespace Eigen
