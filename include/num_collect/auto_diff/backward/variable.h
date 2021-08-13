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

#include "num_collect/auto_diff/backward/graph/node.h"

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
template <typename Scalar>
class variable {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Construct.
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
    variable(const scalar_type& value)  // NOLINT: implicit convertion required
        : variable(value, constant_tag()) {}

    /*!
     * \brief Construct.
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
     * \brief Add a variable to this variable.
     *
     * \param[in] right Right-hand-side variable.
     * \return This.
     */
    auto operator+=(const variable& right) -> variable& {
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

}  // namespace num_collect::auto_diff::backward
