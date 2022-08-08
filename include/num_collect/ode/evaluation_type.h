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
 * \brief Definition of evaluation_type enumeration.
 */
#pragma once

namespace num_collect::ode {

/*!
 * \brief Struct to specify types of evaluations.
 */
struct evaluation_type {
    //! Differential coefficient.
    bool diff_coeff{false};

    //! Jacobian.
    bool jacobian{false};

    //! Partial derivative with respect to time.
    bool time_derivative{false};

    //! Mass matrix.
    bool mass{false};

    /*!
     * \brief Check whether this evaluations allows the requested evaluations.
     *
     * \param[in] request Requested evaluations.
     * \retval true All evaluations in request is allowed by this evaluations.
     * \retval false Otherwise.
     */
    [[nodiscard]] constexpr auto allows(evaluation_type request) const -> bool {
        return !(!this->diff_coeff && request.diff_coeff) &&
            !(!this->jacobian && request.jacobian) &&
            !(!this->time_derivative && request.time_derivative) &&
            !(!this->mass && request.mass);
    }
};

}  // namespace num_collect::ode
