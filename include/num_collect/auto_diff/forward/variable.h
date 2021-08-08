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

#include <type_traits>

#include "num_collect/util/index_type.h"
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
template <typename Value, typename Diff = Value>
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
    variable(const value_type& value, const diff_type& diff)
        : value_(value), diff_(diff) {}

    /*!
     * \brief Construct.
     *
     * \note This constructor can be used when diff_type is a type of scalars.
     *
     * \param[in] value Value.
     */
    explicit variable(const value_type& value)
        : variable(value, static_cast<diff_type>(0)) {}

    /*!
     * \brief Construct.
     *
     * \note This constructor can be used when diff_type is a type of scalars.
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
     * \brief Get the differential coefficients.
     *
     * \return Differential coefficients.
     */
    [[nodiscard]] auto diff() const noexcept -> const diff_type& {
        return diff_;
    }

    /*!
     * \brief Add a variable.
     *
     * \param[in] right Right-hand-side variable.
     * \return This.
     */
    auto operator+=(const variable& right) -> variable& {
        value_ += right.value_;
        diff_ += right.diff_;
        return *this;
    }

    /*!
     * \brief Add a value.
     *
     * \param[in] right Right-hand-side value.
     * \return This.
     */
    auto operator+=(const value_type& right) -> variable& {
        value_ += right;
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
        diff_ -= right.diff_;
        return *this;
    }

    /*!
     * \brief Subtract a value.
     *
     * \param[in] right Right-hand-side value.
     * \return This.
     */
    auto operator-=(const value_type& right) -> variable& {
        value_ -= right;
        return *this;
    }

    /*!
     * \brief Multiply a variable.
     *
     * \param[in] right Right-hand-side variable.
     * \return This.
     */
    auto operator*=(const variable& right) -> variable& {
        diff_ = right.value_ * diff_ + value_ * right.diff_;
        value_ *= right.value_;
        return *this;
    }

    /*!
     * \brief Multiply a value.
     *
     * \param[in] right Right-hand-side value.
     * \return This.
     */
    auto operator*=(const value_type& right) -> variable& {
        diff_ *= right;
        value_ *= right;
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
        diff_ = (diff_ - right.diff_ * value_) / right.value_;
        return *this;
    }

    /*!
     * \brief Divide by a value.
     *
     * \param[in] right Right-hand-side value.
     * \return This.
     */
    auto operator/=(const value_type& right) -> variable& {
        value_ /= right;
        diff_ /= right;
        return *this;
    }

private:
    //! Value.
    value_type value_;

    //! Differential coefficients.
    diff_type diff_;
};

/*!
 * \brief Create a variable by which functions will be differentiated
 *        (for scalar differential coefficients).
 *
 * This will set the differential coefficient to one.
 *
 * \tparam Value Type of the value.
 * \tparam Diff Type of the differential coefficient.
 * \param[in] value Value.
 * \return Variable.
 */
template <typename Value>
[[nodiscard]] inline auto create_diff_variable(const Value& value)
    -> variable<Value> {
    return variable<Value>(value, static_cast<Value>(1));
}

/*!
 * \brief Create a variable by which functions will be differentiated
 *        (for vector differential coefficients).
 *
 * \tparam Value Type of the value.
 * \tparam Diff Type of the differential coefficients.
 * \param[in] value Value.
 * \param[in] size Size of Diff.
 * \param[in] index Index of the variable.
 * \return Variable.
 */
template <typename Value, typename Diff,
    std::enable_if_t<is_eigen_vector_v<Diff>, void*> = nullptr>
[[nodiscard]] inline auto create_diff_variable(const Value& value,
    index_type size, index_type index) -> variable<Value, Diff> {
    Diff diff = Diff::Zero(size);
    diff(index) = static_cast<typename Diff::Scalar>(1);
    return variable<Value, Diff>(value, diff);
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
    const variable<Value, Diff>& right) -> variable<Value, Diff> {
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

}  // namespace num_collect::auto_diff::forward
