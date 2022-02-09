/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of functions objects to compare values.
 */
#pragma once

#include "num_collect/util/concepts/rhs_comparable.h"

namespace num_collect::util {

/*!
 * \brief Class to compare two values with `operator<`.
 *
 * \tparam Left Type of left-hand-side values.
 * \tparam Right Type of right-hand-side values.
 */
template <typename Left, concepts::rhs_less_than_comparable<Left> Right = Left>
class less {
public:
    /*!
     * \brief Compare.
     *
     * \param[in] left Left-hand-side value.
     * \param[in] right Right-hand-side value.
     * \return Whether left is less than right.
     */
    [[nodiscard]] constexpr auto operator()(
        const Left& left, const Right& right) const -> bool {
        return left < right;
    }
};

/*!
 * \brief Class to compare two values with `operator<=`.
 *
 * \tparam Left Type of left-hand-side values.
 * \tparam Right Type of right-hand-side values.
 */
template <typename Left,
    concepts::rhs_less_than_or_equal_to_comparable<Left> Right = Left>
class less_equal {
public:
    /*!
     * \brief Compare.
     *
     * \param[in] left Left-hand-side value.
     * \param[in] right Right-hand-side value.
     * \return Whether left is less than or equal to right.
     */
    [[nodiscard]] constexpr auto operator()(
        const Left& left, const Right& right) const -> bool {
        return left <= right;
    }
};

/*!
 * \brief Class to compare two values with `operator>`.
 *
 * \tparam Left Type of left-hand-side values.
 * \tparam Right Type of right-hand-side values.
 */
template <typename Left,
    concepts::rhs_greater_than_comparable<Left> Right = Left>
class greater {
public:
    /*!
     * \brief Compare.
     *
     * \param[in] left Left-hand-side value.
     * \param[in] right Right-hand-side value.
     * \return Whether left is greater than right.
     */
    [[nodiscard]] constexpr auto operator()(
        const Left& left, const Right& right) const -> bool {
        return left > right;
    }
};

/*!
 * \brief Class to compare two values with `operator>=`.
 *
 * \tparam Left Type of left-hand-side values.
 * \tparam Right Type of right-hand-side values.
 */
template <typename Left,
    concepts::rhs_greater_than_or_equal_to_comparable<Left> Right = Left>
class greater_equal {
public:
    /*!
     * \brief Compare.
     *
     * \param[in] left Left-hand-side value.
     * \param[in] right Right-hand-side value.
     * \return Whether left is greater than or equal to right.
     */
    [[nodiscard]] constexpr auto operator()(
        const Left& left, const Right& right) const -> bool {
        return left >= right;
    }
};

/*!
 * \brief Class to compare two values with `operator==`.
 *
 * \tparam Left Type of left-hand-side values.
 * \tparam Right Type of right-hand-side values.
 */
template <typename Left, concepts::rhs_equal_to_comparable<Left> Right = Left>
class equal {
public:
    /*!
     * \brief Compare.
     *
     * \param[in] left Left-hand-side value.
     * \param[in] right Right-hand-side value.
     * \return Whether left is equal to right.
     */
    [[nodiscard]] constexpr auto operator()(
        const Left& left, const Right& right) const -> bool {
        return left == right;
    }
};

/*!
 * \brief Class to compare two values with `operator!=`.
 *
 * \tparam Left Type of left-hand-side values.
 * \tparam Right Type of right-hand-side values.
 */
template <typename Left,
    concepts::rhs_not_equal_to_comparable<Left> Right = Left>
class not_equal {
public:
    /*!
     * \brief Compare.
     *
     * \param[in] left Left-hand-side value.
     * \param[in] right Right-hand-side value.
     * \return Whether left is not equal to right.
     */
    [[nodiscard]] constexpr auto operator()(
        const Left& left, const Right& right) const -> bool {
        return left != right;
    }
};

}  // namespace num_collect::util
