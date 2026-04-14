/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Eigen support for num_collect::multi_double::quad class.
 */
#pragma once

#include <limits>

#include <Eigen/Core>

#include "num_collect/multi_double/quad.h"

namespace Eigen {

/*!
 * \brief Specialization of Eigen::NumTraits for
 * num_collect::multi_double::quad.
 *
 * \note Names in this class is defined by Eigen.
 */
template <>
struct NumTraits<num_collect::multi_double::quad> {
    //! Type of the real scalars.
    using Real = num_collect::multi_double::quad;

    //! Type of non-integer scalars.
    using NonInteger = num_collect::multi_double::quad;

    //! Type of numeric literals.
    using Literal = double;

    //! Type to use to nest a value.
    using Nested = num_collect::multi_double::quad;

    // NOLINTBEGIN : API of an external library.
    enum {
        //! Whether this type is a complex number.
        IsComplex = 0,
        //! Whether this type is an integer.
        IsInteger = 0,
        //! Cost to read this value.
        ReadCost = 2,
        //! Whether this type is signed.
        IsSigned = 1,
        //! Whether this type requires initialization.
        RequireInitialization = 0,
        //! Cost to add this value.
        AddCost = 10,
        //! Cost to multiply this value.
        MulCost = 10
    };
    // NOLINTEND

    /*!
     * \brief Get machine epsilon.
     *
     * \return Machine epsilon.
     */
    static constexpr auto epsilon() -> num_collect::multi_double::quad {
        return num_collect::multi_double::quad(
            std::numeric_limits<double>::epsilon() *
            std::numeric_limits<double>::epsilon());
    }

    /*!
     * \brief Get dummy precision.
     *
     * \return Dummy precision.
     */
    static constexpr auto dummy_precision() -> num_collect::multi_double::quad {
        static constexpr auto value = epsilon() * 1000.0;
        return value;
    }

    /*!
     * \brief Get the highest value.
     *
     * \return Highest value.
     */
    static constexpr auto highest() -> num_collect::multi_double::quad {
        return num_collect::multi_double::quad(
            std::numeric_limits<double>::max());
    }

    /*!
     * \brief Get the lowest value.
     *
     * \return Lowest value.
     */
    static constexpr auto lowest() -> num_collect::multi_double::quad {
        return num_collect::multi_double::quad(
            std::numeric_limits<double>::lowest());
    }

    /*!
     * \brief Get the number of bits in the mantissa.
     *
     * \return Number of bits in the mantissa.
     */
    static constexpr auto digits() -> int {
        return std::numeric_limits<double>::digits * 2;
    }

    /*!
     * \brief Get the number of digits.
     *
     * \return Number of digits.
     */
    static constexpr auto digits10() -> int {
        return std::numeric_limits<double>::digits10 * 2;
    }

    /*!
     * \brief Get the maximum number of digits needed to represent the value.
     *
     * \return Maximum number of digits needed to represent the value.
     */
    static constexpr auto max_digits10() -> int {
        return std::numeric_limits<double>::max_digits10 * 2;
    }

    /*!
     * \brief Get the minimum exponent.
     *
     * \return Minimum exponent.
     */
    static constexpr auto min_exponent() -> int {
        return std::numeric_limits<double>::min_exponent;
    }

    /*!
     * \brief Get the maximum exponent.
     *
     * \return Maximum exponent.
     */
    static constexpr auto max_exponent() -> int {
        return std::numeric_limits<double>::max_exponent;
    }
};

}  // namespace Eigen
