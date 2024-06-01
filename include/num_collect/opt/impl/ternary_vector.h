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
 * \brief Definition of ternary_vector class.
 */
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <utility>

#include <Eigen/Core>

#include "num_collect/base/index_type.h"
#include "num_collect/util/assert.h"

namespace num_collect::opt::impl {

/*!
 * \brief Class of vectors of ternary floating-point numbers.
 */
class ternary_vector {
public:
    //! Type of a digit.
    using digit_type = std::int8_t;

    //! Maximum number of digits per dimension.
    static constexpr index_type max_digits =
        static_cast<index_type>(std::numeric_limits<digit_type>::max());

    /*!
     * \brief Constructor.
     */
    ternary_vector() = default;

    /*!
     * \brief Constructor.
     *
     * \param[in] dim Number of digits.
     */
    explicit ternary_vector(index_type dim)
        : data_(data_type::Zero(dim, init_digits_per_dimensions + 1)) {}

    /*!
     * \brief Get the number of dimensions.
     *
     * \return Number of dimensions.
     */
    [[nodiscard]] auto dim() const -> index_type { return data_.rows(); }

    /*!
     * \brief Change the number of dimensions.
     *
     * \warning This won't preserve the digits already inserted.
     *
     * \param[in] dim Number of dimensions.
     */
    void change_dim(index_type dim) {
        data_ = data_type::Zero(dim, init_digits_per_dimensions + 1);
    }

    /*!
     * \brief Get the number of digits of a dimension.
     *
     * \param[in] dim Dimension index.
     * \return Number of digits.
     */
    [[nodiscard]] auto digits(index_type dim) const -> index_type {
        NUM_COLLECT_DEBUG_ASSERT(dim < data_.rows());
        return static_cast<index_type>(data_(dim, 0));
    }

    /*!
     * \brief Access a digit.
     *
     * \param[in] dim Dimension index.
     * \param[in] digit Digit index.
     * \return Reference to the digit.
     */
    [[nodiscard]] auto operator()(
        index_type dim, index_type digit) -> digit_type& {
        NUM_COLLECT_DEBUG_ASSERT(dim < data_.rows());
        NUM_COLLECT_DEBUG_ASSERT(digit < data_(dim, 0));
        return data_(dim, digit + 1);
    }

    /*!
     * \brief Access a digit.
     *
     * \param[in] dim Dimension index.
     * \param[in] digit Digit index.
     * \return Reference to the digit.
     */
    [[nodiscard]] auto operator()(
        index_type dim, index_type digit) const -> digit_type {
        NUM_COLLECT_DEBUG_ASSERT(dim < data_.rows());
        NUM_COLLECT_DEBUG_ASSERT(digit < data_(dim, 0));
        return data_(dim, digit + 1);
    }

    /*!
     * \brief Add a digit to a dimension.
     *
     * \param[in] dim Dimension index.
     * \param[in] digit Digit.
     */
    void push_back(index_type dim, digit_type digit) {
        const auto next_digits = digits(dim) + 1;
        if (next_digits >= data_.cols()) {
            NUM_COLLECT_ASSERT(next_digits <= max_digits);
            auto new_data = data_type(data_.rows(), next_digits + 1);
            new_data.leftCols(data_.cols()) = data_;
            data_ = std::move(new_data);
        }
        data_(dim, next_digits) = digit;
        data_(dim, 0) = static_cast<digit_type>(next_digits);
    }

    /*!
     * \brief Compare with another object.
     *
     * \param[in] right Right-hand-side object.
     * \return Whether the two object are same.
     */
    [[nodiscard]] auto operator==(const ternary_vector& right) const -> bool {
        NUM_COLLECT_ASSERT(data_.rows() == right.data_.rows());
        for (index_type i = 0; i < data_.rows(); ++i) {
            // NOLINTNEXTLINE: false positive
            const auto left_digits = static_cast<index_type>(data_(i, 0));
            const auto right_digits =  // NOLINTNEXTLINE: false positive
                static_cast<index_type>(right.data_(i, 0));
            const auto min_digits = std::min(left_digits, right_digits);
            for (index_type j = 0; j < min_digits; ++j) {
                if (data_(i, j + 1) != right.data_(i, j + 1)) {
                    return false;
                }
            }
            for (index_type j = min_digits; j < left_digits; ++j) {
                if (data_(i, j + 1) != static_cast<digit_type>(0)) {
                    return false;
                }
            }
            for (index_type j = min_digits; j < right_digits; ++j) {
                if (right.data_(i, j + 1) != static_cast<digit_type>(0)) {
                    return false;
                }
            }
        }
        return true;
    }

    /*!
     * \brief Compare with another object.
     *
     * \param[in] right Right-hand-side object.
     * \return Whether the two object are different.
     */
    [[nodiscard]] auto operator!=(const ternary_vector& right) const -> bool {
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
        const auto ndigits = digits(dim);
        auto num = static_cast<Scalar>(0);
        auto coeff = static_cast<Scalar>(1);
        static const Scalar inv_base =
            static_cast<Scalar>(1) / static_cast<Scalar>(3);
        for (index_type i = 0; i < ndigits; ++i) {
            num += coeff * static_cast<Scalar>(data_(dim, i + 1));
            coeff *= inv_base;
        }
        return num;
    }

private:
    //! Type of data matrix.
    using data_type = Eigen::Matrix<digit_type, Eigen::Dynamic, Eigen::Dynamic,
        Eigen::RowMajor>;

    //! Initial digits per dimensions.
    static constexpr index_type init_digits_per_dimensions = 4;

    /*!
     * \brief Data matrix.
     *
     * This matrix has the number of digits for each dimension and the buffer of
     * digits.
     */
    data_type data_{};
};

}  // namespace num_collect::opt::impl

namespace std {

/*!
 * \brief Implementation of std::hash for
 * num_collect::opt::impl::ternary_vector.
 */
template <>
class hash<num_collect::opt::impl::ternary_vector> {
public:
    //! Type of argument.
    using argument_type = num_collect::opt::impl::ternary_vector;

    //! Type of result.
    using result_type = std::size_t;

    /*!
     * \brief Calculate hash.
     *
     * \param[in] vec Vector.
     * \return Hash.
     */
    [[nodiscard]] auto operator()(
        const argument_type& vec) const -> result_type {
        std::size_t res = 0;
        for (num_collect::index_type i = 0; i < vec.dim(); ++i) {
            std::size_t temp = 0;
            num_collect::index_type non_zero_digits = 0;
            for (num_collect::index_type j = 0; j < vec.digits(i); ++j) {
                if (vec(i, j) != 0) {
                    non_zero_digits = j + 1;
                }
            }
            for (num_collect::index_type j = 0; j < non_zero_digits; ++j) {
                constexpr std::size_t coeff = 3;
                temp *= coeff;
                temp += static_cast<std::size_t>(
                    static_cast<std::uint8_t>(vec(i, j)));
            }
            constexpr std::size_t coeff = 79865413;  // a prime number
            res *= coeff;
            res += temp;
        }
        return res;
    }
};

}  // namespace std
