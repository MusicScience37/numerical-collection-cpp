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
 * \brief Definition of node class.
 */
#pragma once

#include <utility>  // IWYU pragma: keep

#include <Eigen/Core>

#include "num_collect/auto_diff/backward/graph/node.h"
#include "num_collect/base/concepts/real_scalar.h"

namespace num_collect::auto_diff::backward {

/*!
 * \brief Tag class to specify constants.
 */
struct constant_tag {};

/*!
 * \brief Tag class to specify variables.
 */
struct variable_tag {};

/*!
 * \brief Class of variables in backward-mode automatic differentiation
 *        \cite Kubota1998.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class variable {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Constructor.
     *
     * \param[in] value Value.
     * \param[in] node Node.
     */
    variable(const scalar_type& value, graph::node_ptr<scalar_type> node)
        : value_(value), node_(std::move(node)) {}

    /*!
     * \brief Construct constants.
     *
     * \param[in] value Value.
     */
    variable(const scalar_type& value, const constant_tag& /*tag*/)
        : variable(value, nullptr) {}

    /*!
     * \brief Construct variables.
     *
     * \param[in] value Value.
     */
    variable(const scalar_type& value, const variable_tag& /*tag*/)
        : variable(value, graph::create_node<scalar_type>()) {}

    /*!
     * \brief Construct constants.
     *
     * \param[in] value Value.
     */
    variable(const scalar_type& value)  // NOLINT: implicit conversion required
        : variable(value, constant_tag()) {}

    /*!
     * \brief Constructor.
     */
    variable() : variable(static_cast<scalar_type>(0)) {}

    /*!
     * \brief Get the value.
     *
     * \return Value.
     */
    [[nodiscard]] auto value() const noexcept -> const scalar_type& {
        return value_;
    }

    /*!
     * \brief Get the node.
     *
     * \return Node.
     */
    [[nodiscard]] auto node() const noexcept
        -> const graph::node_ptr<scalar_type>& {
        return node_;
    }

    /*!
     * \brief Negate this variable.
     *
     * \return Negative variable.
     */
    auto operator-() const -> variable {
        if (node_) {
            return variable(-value_,
                graph::create_node<scalar_type>(
                    node_, static_cast<scalar_type>(-1)));
        }
        return variable(-value_, nullptr);
    }

    /*!
     * \brief Add a variable to this variable.
     *
     * \param[in] right Right-hand-side variable.
     * \return This.
     */
    auto operator+=(const variable& right) -> variable& {
        if (this == &right) {
            node_ = graph::create_node<scalar_type>(
                node_, static_cast<scalar_type>(2));
            value_ += right.value_;
            return *this;
        }
        if (node_) {
            if (right.node_) {
                node_ = graph::create_node<scalar_type>(node_,
                    static_cast<scalar_type>(1), right.node_,
                    static_cast<scalar_type>(1));
            }
        } else {
            node_ = right.node_;
        }
        value_ += right.value_;
        return *this;
    }

    /*!
     * \brief Subtract a variable from this variable.
     *
     * \param[in] right Right-hand-side variable.
     * \return This.
     */
    auto operator-=(const variable& right) -> variable& {
        if (this == &right) {
            value_ -= right.value_;
            node_.reset();
            return *this;
        }
        if (node_) {
            if (right.node_) {
                node_ = graph::create_node<scalar_type>(node_,
                    static_cast<scalar_type>(1), right.node_,
                    static_cast<scalar_type>(-1));
            }
        } else {
            if (right.node_) {
                node_ = graph::create_node<scalar_type>(
                    right.node_, static_cast<scalar_type>(-1));
            }
        }
        value_ -= right.value_;
        return *this;
    }

    /*!
     * \brief Multiply this variable by a variable.
     *
     * \param[in] right Right-hand-side variable.
     * \return This.
     */
    auto operator*=(const variable& right) -> variable& {
        if (this == &right) {
            node_ = graph::create_node<scalar_type>(
                node_, static_cast<scalar_type>(2) * value_);
            value_ *= right.value_;
            return *this;
        }
        if (node_) {
            if (right.node_) {
                node_ = graph::create_node<scalar_type>(
                    node_, right.value_, right.node_, value_);
            } else {
                node_ = graph::create_node<scalar_type>(node_, right.value_);
            }
        } else {
            if (right.node_) {
                node_ = graph::create_node<scalar_type>(right.node_, value_);
            }
        }
        value_ *= right.value_;
        return *this;
    }

    /*!
     * \brief Divide this variable by a variable.
     *
     * \param[in] right Right-hand-side variable.
     * \return This.
     */
    auto operator/=(const variable& right) -> variable& {
        if (this == &right) {
            value_ /= right.value_;
            node_.reset();
            return *this;
        }
        value_ /= right.value_;
        if (node_) {
            if (right.node_) {
                node_ = graph::create_node<scalar_type>(node_,
                    static_cast<scalar_type>(1) / right.value_, right.node_,
                    -value_ / right.value_);
            } else {
                node_ = graph::create_node<scalar_type>(
                    node_, static_cast<scalar_type>(1) / right.value_);
            }
        } else {
            if (right.node_) {
                node_ = graph::create_node<scalar_type>(
                    right.node_, -value_ / right.value_);
            }
        }
        return *this;
    }

private:
    //! Value.
    scalar_type value_;

    //! Node.
    graph::node_ptr<scalar_type> node_;
};

/*!
 * \brief Add two variables.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Sum.
 */
template <typename Scalar>
[[nodiscard]] inline auto operator+(const variable<Scalar>& left,
    const variable<Scalar>& right) -> variable<Scalar> {
    return variable<Scalar>(left) += right;
}

/*!
 * \brief Add two variables.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Sum.
 */
template <typename Scalar>
[[nodiscard]] inline auto operator+(
    const Scalar& left, const variable<Scalar>& right) -> variable<Scalar> {
    return variable<Scalar>(left) += right;
}

/*!
 * \brief Add two variables.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Sum.
 */
template <typename Scalar>
[[nodiscard]] inline auto operator+(
    const variable<Scalar>& left, const Scalar& right) -> variable<Scalar> {
    return variable<Scalar>(left) += right;
}

/*!
 * \brief Subtract a variable from another variable.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Difference.
 */
template <typename Scalar>
[[nodiscard]] inline auto operator-(const variable<Scalar>& left,
    const variable<Scalar>& right) -> variable<Scalar> {
    return variable<Scalar>(left) -= right;
}

/*!
 * \brief Subtract a variable from another variable.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Difference.
 */
template <typename Scalar>
[[nodiscard]] inline auto operator-(
    const Scalar& left, const variable<Scalar>& right) -> variable<Scalar> {
    return variable<Scalar>(left) -= right;
}

/*!
 * \brief Subtract a variable from another variable.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Difference.
 */
template <typename Scalar>
[[nodiscard]] inline auto operator-(
    const variable<Scalar>& left, const Scalar& right) -> variable<Scalar> {
    return variable<Scalar>(left) -= right;
}

/*!
 * \brief Multiply two variables.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Product.
 */
template <typename Scalar>
[[nodiscard]] inline auto operator*(const variable<Scalar>& left,
    const variable<Scalar>& right) -> variable<Scalar> {
    return variable<Scalar>(left) *= right;
}

/*!
 * \brief Multiply two variables.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Product.
 */
template <typename Scalar>
[[nodiscard]] inline auto operator*(
    const Scalar& left, const variable<Scalar>& right) -> variable<Scalar> {
    return variable<Scalar>(left) *= right;
}

/*!
 * \brief Multiply two variables.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Product.
 */
template <typename Scalar>
[[nodiscard]] inline auto operator*(
    const variable<Scalar>& left, const Scalar& right) -> variable<Scalar> {
    return variable<Scalar>(left) *= right;
}

/*!
 * \brief Divide a variable from another variable.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Quotient.
 */
template <typename Scalar>
[[nodiscard]] inline auto operator/(const variable<Scalar>& left,
    const variable<Scalar>& right) -> variable<Scalar> {
    return variable<Scalar>(left) /= right;
}

/*!
 * \brief Divide a variable from another variable.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Quotient.
 */
template <typename Scalar>
[[nodiscard]] inline auto operator/(
    const Scalar& left, const variable<Scalar>& right) -> variable<Scalar> {
    return variable<Scalar>(left) /= right;
}

/*!
 * \brief Divide a variable from another variable.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] left Left-hand-side variable.
 * \param[in] right Right-hand-side variable.
 * \return Quotient.
 */
template <typename Scalar>
[[nodiscard]] inline auto operator/(
    const variable<Scalar>& left, const Scalar& right) -> variable<Scalar> {
    return variable<Scalar>(left) /= right;
}

}  // namespace num_collect::auto_diff::backward

namespace Eigen {

/*!
 * \brief Specialization of Eigen::NumTraits for
 * num_collect::auto_diff::forward::variable.
 *
 * See
 * [Reference](https://eigen.tuxfamily.org/dox/structEigen_1_1NumTraits.html)
 * for description.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Diff Type of differential coefficients.
 */
template <typename Scalar>
struct NumTraits<num_collect::auto_diff::backward::variable<Scalar>> {
    //! Type of the variable.
    using Real = num_collect::auto_diff::backward::variable<Scalar>;

    //! Type of the variable.
    using NonInteger = Real;

    //! Type of the variable.
    using Literal = Real;

    //! Type of the variable.
    using Nested = Real;

    enum {  // NOLINT(performance-enum-size): Preserve the same implementation as Eigen library.
        //! Whether this type is an integer.
        IsInteger = 0,  // NOLINT

        //! Whether this type is signed.
        IsSigned = 1,  // NOLINT

        //! Whether this type is a complex number.
        IsComplex = 0,  // NOLINT

        //! Whether this type requires initialization.
        RequireInitialization = 1,  // NOLINT

        //! Cost to read this value.
        ReadCost = 1,  // NOLINT

        //! Cost to add this value.
        AddCost = 2,  // NOLINT

        //! Cost to multiply this value.
        MulCost = 4  // NOLINT
    };

    /*!
     * \brief Get machine epsilon.
     *
     * \return Machine epsilon.
     */
    static constexpr auto epsilon() -> Real {
        return NumTraits<Scalar>::epsilon();
    }

    /*!
     * \brief Get dummy precision.
     *
     * \return Dummy precision.
     */
    static constexpr auto dummy_precision() -> Real {
        return NumTraits<Scalar>::dummy_precision();
    }

    /*!
     * \brief Get the highest value.
     *
     * \return Highest value.
     */
    static constexpr auto highest() -> Real {
        return NumTraits<Scalar>::highest();
    }

    /*!
     * \brief Get the lowest value.
     *
     * \return Lowest value.
     */
    static constexpr auto lowest() -> Real {
        return NumTraits<Scalar>::lowest();
    }

    /*!
     * \brief Get the number of digits.
     *
     * \return Number of digits.
     */
    static constexpr auto digits10() -> int {
        return NumTraits<Scalar>::digits10();
    }

    /*!
     * \brief Get the infinity.
     *
     * \return Infinity.
     */
    static constexpr auto infinity() -> Real {
        return NumTraits<Scalar>::infinity();
    }

    /*!
     * \brief Get the quiet NaN value.
     *
     * \return Quiet NaN value.
     */
    static constexpr auto quiet_NaN() -> Real {  // NOLINT
        return NumTraits<Scalar>::quiet_NaN();
    }
};

}  // namespace Eigen
