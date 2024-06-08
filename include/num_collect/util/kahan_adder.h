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
 * \brief Definition of kahan_adder class.
 */
#pragma once

#include "num_collect/util/concepts/kahan_addable.h"

namespace num_collect::util {

/*!
 * \brief Class to add numbers using Kahan summation \cite Kahan1965.
 *
 * \tparam T Type of numbers.
 */
template <concepts::kahan_addable T>
class kahan_adder {
public:
    /*!
     * \brief Constructor.
     *
     * \note This won't work for Eigen's vectors and matrices.
     * Use another constructor for them.
     */
    kahan_adder() : kahan_adder(static_cast<T>(0)) {}

    /*!
     * \brief Construct with zero number.
     *
     * This constructor is useful for Eigen's vectors and matrices.
     *
     * \param[in] zero Zero.
     */
    explicit kahan_adder(const T& zero)
        : sum_(zero), rem_(zero), prev_sum_(zero) {}

    /*!
     * \brief Add a number.
     *
     * \param[in] value Added value.
     * \return This.
     */
    auto add(const T& value) -> kahan_adder& {
        prev_sum_ = sum_;
        rem_ += value;
        sum_ += rem_;
        rem_ -= sum_ - prev_sum_;
        return *this;
    }

    /*!
     * \brief Subtract a number.
     *
     * \param[in] value Added value.
     * \return This.
     */
    auto sub(const T& value) -> kahan_adder& {
        prev_sum_ = sum_;
        rem_ += -value;
        sum_ += rem_;
        rem_ -= sum_ - prev_sum_;
        return *this;
    }

    /*!
     * \brief Add a number.
     *
     * \param[in] value Added value.
     * \return This.
     */
    auto operator+=(const T& value) -> kahan_adder& {
        add(value);
        return *this;
    }

    /*!
     * \brief Subtract a number.
     *
     * \param[in] value Added value.
     * \return This.
     */
    auto operator-=(const T& value) -> kahan_adder& {
        sub(value);
        return *this;
    }

    /*!
     * \brief Get sum.
     *
     * \return Sum.
     */
    [[nodiscard]] auto sum() const noexcept -> const T& { return sum_; }

    /*!
     * \brief Get sum.
     *
     * \return Sum.
     */
    operator T() const {  // NOLINT: Allow implicit conversion.
        return sum_;
    }

private:
    //! Sum.
    T sum_;

    //! Remaining.
    T rem_;

    //! Previous sum.
    T prev_sum_;
};

}  // namespace num_collect::util
