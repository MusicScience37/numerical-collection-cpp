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
 * \brief Definition of adc_ternary_vector class.
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <utility>

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/util/assert.h"

namespace num_collect::opt::impl {

/*!
 * \brief Class of vectors of ternary floating-point numbers in \ref
 * num_collect::opt::adaptive_diagonal_curves.
 *
 * \tparam VariableType Type of variables.
 * \tparam MaxDigits Maximum number of digits per dimension at compile time.
 */
template <base::concepts::real_scalar_dense_vector VariableType,
    index_type MaxDigits>
class adc_ternary_vector {
public:
    static_assert(MaxDigits > 0, "MaxDigits must be a positive integer.");

    //! Type of a digit.
    using digit_type = std::int8_t;

    //! Dimensions at compile time.
    static constexpr index_type dimensions_at_compile_time =
        VariableType::RowsAtCompileTime;

    //! Maximum number of dimensions at compile time.
    static constexpr index_type max_dimensions_at_compile_time =
        VariableType::MaxRowsAtCompileTime;

    //! Maximum number of digits per dimension at compile time.
    static constexpr index_type max_digits_at_compile_time = MaxDigits;

    /*!
     * \brief Constructor.
     */
    adc_ternary_vector() = default;

    /*!
     * \brief Constructor.
     *
     * \param[in] dim Number of digits.
     */
    explicit adc_ternary_vector(index_type dim)
        : data_(data_type::Zero(dim, MaxDigits)) {}

    /*!
     * \brief Get the number of dimensions.
     *
     * \return Number of dimensions.
     */
    [[nodiscard]] auto dim() const -> index_type { return data_.rows(); }

    /*!
     * \brief Get the current maximum number of digits.
     *
     * \return Current maximum number of digits.
     */
    [[nodiscard]] auto current_max_digits() const -> index_type {
        return current_max_digits_;
    }

    /*!
     * \brief Get the next dimension index to add a digit.
     *
     * \return Next dimension index.
     */
    [[nodiscard]] auto next_divided_dimension_index() const -> index_type {
        return next_divided_dimension_index_;
    }

    /*!
     * \brief Check whether this vector is full.
     *
     * \retval true This vector is full.
     * \retval false This vector is not full.
     */
    [[nodiscard]] auto is_full() const -> bool {
        return current_max_digits_ >= max_digits_at_compile_time &&
            next_divided_dimension_index_ == 0;
    }

    /*!
     * \brief Get the number of digits in a dimension.
     *
     * \param[in] dim Dimension index.
     * \return Number of digits.
     */
    [[nodiscard]] auto digits(index_type dim) const -> index_type {
        if (next_divided_dimension_index_ == 0 ||
            dim < next_divided_dimension_index_) {
            return current_max_digits_;
        }
        return current_max_digits_ - 1;
    }

    /*!
     * \brief Access a digit.
     *
     * \param[in] dim Dimension index.
     * \param[in] digit Digit index.
     * \return Reference to the digit.
     */
    [[nodiscard]] auto operator()(index_type dim, index_type digit) const
        -> digit_type {
        NUM_COLLECT_DEBUG_ASSERT(dim < data_.rows());
        NUM_COLLECT_DEBUG_ASSERT(digit < max_digits_at_compile_time);
        return data_.coeff(dim, digit);
    }

    /*!
     * \brief Access a digit.
     *
     * \param[in] dim Dimension index.
     * \param[in] digit Digit index.
     * \return Reference to the digit.
     */
    [[nodiscard]] auto operator()(index_type dim, index_type digit)
        -> digit_type& {
        NUM_COLLECT_DEBUG_ASSERT(dim < data_.rows());
        NUM_COLLECT_DEBUG_ASSERT(digit < max_digits_at_compile_time);
        return data_.coeffRef(dim, digit);
    }

    /*!
     * \brief Add a digit to a dimension specified by
     * next_divided_dimension_index().
     *
     * \param[in] digit Digit.
     * \return Pair of indices of the dimension and the digit to which the digit
     * is added.
     */
    auto push_back(digit_type digit) -> std::pair<index_type, index_type> {
        if (next_divided_dimension_index_ == 0) {
            if (current_max_digits_ >= max_digits_at_compile_time) {
                NUM_COLLECT_LOG_AND_THROW(precondition_not_satisfied,
                    "Tried to add a digit to a full adc_ternary_vector.");
            }
            current_max_digits_ += 1;
        }
        NUM_COLLECT_DEBUG_ASSERT(current_max_digits_ - 1 >= 0);
        NUM_COLLECT_DEBUG_ASSERT(current_max_digits_ - 1 < data_.cols());
        NUM_COLLECT_DEBUG_ASSERT(next_divided_dimension_index_ < data_.rows());
        data_.coeffRef(next_divided_dimension_index_, current_max_digits_ - 1) =
            digit;
        const index_type divided_dimension_index =
            next_divided_dimension_index_;
        next_divided_dimension_index_ =
            (next_divided_dimension_index_ + 1) % data_.rows();
        return {divided_dimension_index, current_max_digits_ - 1};
    }

    /*!
     * \brief Compare with another object.
     *
     * \param[in] right Right-hand-side object.
     * \return Whether the two object are same.
     */
    [[nodiscard]] auto operator==(const adc_ternary_vector& right) const
        -> bool {
        return data_ == right.data_;
    }

    /*!
     * \brief Compare with another object.
     *
     * \param[in] right Right-hand-side object.
     * \return Whether the two object are different.
     */
    [[nodiscard]] auto operator!=(const adc_ternary_vector& right) const
        -> bool {
        return !operator==(right);
    }

    /*!
     * \brief Get the element of this vector as a floating-point number.
     *
     * \tparam Scalar Type of resulting floating-point number.
     * \param[in] dim Dimension index.
     * \return Calculated floating-point number.
     */
    template <typename Scalar>
    [[nodiscard]] auto elem_as(index_type dim) const -> Scalar {
        NUM_COLLECT_DEBUG_ASSERT(dim < data_.rows());
        auto num = static_cast<Scalar>(0);
        auto coeff = static_cast<Scalar>(1);
        static const Scalar inv_base =
            static_cast<Scalar>(1) / static_cast<Scalar>(3);
        for (index_type i = 0; i < current_max_digits_; ++i) {
            num += coeff * static_cast<Scalar>(data_.coeff(dim, i));
            coeff *= inv_base;
        }
        return num;
    }

    /*!
     * \brief Convert this vector to a variable.
     *
     * \param[in] lower_bound Element-wise lower bound of the search range.
     * \param[in] width Element-wise width of the search range.
     * \return Variable.
     */
    [[nodiscard]] auto as_variable(const VariableType& lower_bound,
        const VariableType& width) const -> VariableType {
        VariableType res(dim());
        for (index_type i = 0; i < dim(); ++i) {
            res(i) = lower_bound(i) +
                width(i) * elem_as<typename VariableType::Scalar>(i);
        }
        return res;
    }

    /*!
     * \brief Calculate the hash of this vector.
     *
     * \return Hash.
     */
    [[nodiscard]] auto hash() const -> std::size_t {
        std::size_t res = 0;
        for (index_type i = 0; i < data_.rows(); ++i) {
            for (index_type j = 0; j < data_.cols(); ++j) {
                res += static_cast<std::size_t>(data_.coeff(i, j)) +
                    (res << 1U) + (res >> 2U);
            }
        }
        return res;
    }

private:
    //! Type of data matrix.
    using data_type = Eigen::Matrix<digit_type, dimensions_at_compile_time,
        MaxDigits, Eigen::RowMajor, max_dimensions_at_compile_time, MaxDigits>;

    //! Data matrix.
    data_type data_;

    //! Current maximum number of digits.
    index_type current_max_digits_{0};

    //! Next dimension index to add a digit.
    index_type next_divided_dimension_index_{0};
};

}  // namespace num_collect::opt::impl

namespace std {

/*!
 * \brief Implementation of std::hash for
 * num_collect::opt::impl::adc_ternary_vector.
 *
 * \tparam VariableType Type of variables.
 * \tparam MaxDigits Maximum number of digits per dimension at compile time.
 */
template <num_collect::base::concepts::real_scalar_dense_vector VariableType,
    num_collect::index_type MaxDigits>
class hash<
    num_collect::opt::impl::adc_ternary_vector<VariableType, MaxDigits>> {
public:
    //! Type of argument.
    using argument_type =
        num_collect::opt::impl::adc_ternary_vector<VariableType, MaxDigits>;

    //! Type of result.
    using result_type = std::size_t;

    /*!
     * \brief Calculate hash.
     *
     * \param[in] vec Vector.
     * \return Hash.
     */
    [[nodiscard]] auto operator()(const argument_type& vec) const
        -> result_type {
        return vec.hash();
    }
};

}  // namespace std
