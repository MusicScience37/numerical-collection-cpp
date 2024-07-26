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
 * \brief Definition of step_size_limits class.
 */
#pragma once

#include <limits>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/exception.h"
#include "num_collect/constants/sqrt.h"
#include "num_collect/logging/logging_macros.h"

namespace num_collect::ode {

namespace impl {

/*!
 * \brief Default upper limit of the step size.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
constexpr auto default_step_size_upper_limit = static_cast<Scalar>(1);

/*!
 * \brief Default lower limit of the step size.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
constexpr Scalar default_step_size_lower_limit =
    constants::sqrt(std::numeric_limits<Scalar>::epsilon());

}  // namespace impl

/*!
 * \brief Class of limits of step sizes.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class step_size_limits {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Constructor.
     */
    constexpr step_size_limits() = default;

    /*!
     * \brief Apply the limit of this object.
     *
     * \param[in] val Value to apply the limit.
     * \return Resulting value.
     */
    [[nodiscard]] auto apply(scalar_type val) const -> scalar_type {
        if (val < lower_limit_) {
            return lower_limit_;
        }
        if (val > upper_limit_) {
            return upper_limit_;
        }
        return val;
    }

    /*!
     * \brief Get the upper limit.
     *
     * \return Value.
     */
    [[nodiscard]] auto upper_limit() const -> const scalar_type& {
        return upper_limit_;
    }

    /*!
     * \brief Get the lower limit.
     *
     * \return Value.
     */
    [[nodiscard]] auto lower_limit() const -> const scalar_type& {
        return lower_limit_;
    }

    /*!
     * \brief Set the upper limit.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto upper_limit(const scalar_type& val) -> step_size_limits& {
        if (val <= lower_limit_) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "0 < lower_limit < upper_limit must be satisfied.");
        }
        upper_limit_ = val;
        return *this;
    }

    /*!
     * \brief Set the lower limit.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto lower_limit(const scalar_type& val) -> step_size_limits& {
        if (val <= static_cast<scalar_type>(0) || upper_limit_ <= val) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "0 < lower_limit < upper_limit must be satisfied.");
        }
        lower_limit_ = val;
        return *this;
    }

private:
    //! Upper limit.
    scalar_type upper_limit_{impl::default_step_size_upper_limit<scalar_type>};

    //! Lower limit.
    scalar_type lower_limit_{impl::default_step_size_lower_limit<scalar_type>};
};

}  // namespace num_collect::ode
