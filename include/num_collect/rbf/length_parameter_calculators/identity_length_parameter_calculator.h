/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of identity_length_parameter_calculator class.
 */
#pragma once

#include "num_collect/base/index_type.h"
#include "num_collect/rbf/concepts/distance_function.h"
#include "num_collect/util/vector_view.h"

namespace num_collect::rbf::length_parameter_calculators {

/*!
 * \brief Class of length parameter calculator returning one.
 *
 * \tparam DistanceFunction Type of the distance function.
 */
template <concepts::distance_function DistanceFunction>
class identity_length_parameter_calculator {
public:
    //! Type of the distance function.
    using distance_function_type = DistanceFunction;

    //! Type of variables.
    using variable_type = typename DistanceFunction::variable_type;

    //! Type of scalars.
    using scalar_type = typename DistanceFunction::value_type;

    //! Whether this calculator uses the globally fixed length parameters.
    static constexpr bool uses_global_length_parameter = true;

    /*!
     * \brief Constructor.
     */
    identity_length_parameter_calculator() = default;

    /*!
     * \brief Compute the length parameters.
     *
     * \param[in] variables Variables.
     * \param[in] distance_function Distance function.
     *
     * This function does nothing in this class.
     */
    void compute(util::vector_view<const variable_type> variables,
        const distance_function_type& distance_function) {
        // No operation.
        (void)variables;
        (void)distance_function;
    }

    /*!
     * \brief Get the length parameter at a point.
     *
     * \param[in] i Index of the point.
     * \return Length parameter.
     *
     * This function always returns one in this class.
     */
    [[nodiscard]] auto length_parameter_at(index_type i) const -> scalar_type {
        (void)i;
        return static_cast<scalar_type>(1);
    }

    /*!
     * \brief Get the current scale of length parameters.
     *
     * \return Scale of length parameters.
     *
     * This function always returns one in this class.
     */
    [[nodiscard]] auto scale() const noexcept -> scalar_type {
        return static_cast<scalar_type>(1);
    }

    /*!
     * \brief Set the scale of length parameters.
     *
     * \param[in] value Value.
     *
     * This function does nothing in this class.
     */
    void scale(scalar_type value) {
        // No operation.
        (void)value;
    }
};

}  // namespace num_collect::rbf::length_parameter_calculators
