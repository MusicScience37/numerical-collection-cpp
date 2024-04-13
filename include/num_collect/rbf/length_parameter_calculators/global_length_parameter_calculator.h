/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of global_length_parameter_calculator class.
 */
#pragma once

#include <cstddef>
#include <limits>
#include <vector>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/zero.h"
#include "num_collect/rbf/concepts/distance_function.h"  // IWYU pragma: keep

namespace num_collect::rbf::length_parameter_calculators {

/*!
 * \brief Class to calculate length parameters for RBF.
 *
 * \tparam DistanceFunction Type of the distance function.
 */
template <concepts::distance_function DistanceFunction>
class global_length_parameter_calculator {
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
    global_length_parameter_calculator() = default;

    /*!
     * \brief Compute the length parameters.
     *
     * \param[in] variables Variables.
     * \param[in] distance_function Distance function.
     */
    void compute(const std::vector<variable_type>& variables,
        const distance_function_type& distance_function) {
        const std::size_t num_samples = variables.size();
        if (num_samples == 0) {
            throw invalid_argument("No sample point is given.");
        }

        auto max_min_distance = constants::zero<scalar_type>;
        for (std::size_t i = 0; i < num_samples; ++i) {
            auto min_distance = std::numeric_limits<scalar_type>::max();
            for (std::size_t j = 0; j < num_samples; ++j) {
                if (i != j) {
                    const auto distance =
                        distance_function(variables[i], variables[j]);
                    if (distance < min_distance) {
                        min_distance = distance;
                    }
                }
            }
            if (min_distance > max_min_distance) {
                max_min_distance = min_distance;
            }
        }

        length_parameter_ = scale_ * max_min_distance;
    }

    /*!
     * \brief Get the length parameter at a point.
     *
     * \param[in] i Index of the point.
     * \return Length parameter.
     */
    [[nodiscard]] auto length_parameter_at(index_type i) const -> scalar_type {
        (void)i;
        return length_parameter_;
    }

    /*!
     * \brief Get the current scale of length parameters.
     *
     * \return Scala of length parameters.
     */
    [[nodiscard]] auto scale() const noexcept -> scalar_type { return scale_; }

    /*!
     * \brief Set the scale of length parameters.
     *
     * \param[in] value Value.
     */
    void scale(scalar_type value) {
        if (value <= constants::zero<scalar_type>) {
            throw invalid_argument(
                "Scale of length parameters must be a positive number.");
        }
        scale_ = value;
    }

private:
    //! Default value of the scalar of length parameters.
    static constexpr auto default_scale = static_cast<scalar_type>(10);

    //! Scale of length parameters.
    scalar_type scale_{default_scale};

    //! Length parameter.
    scalar_type length_parameter_{constants::zero<scalar_type>};
};

}  // namespace num_collect::rbf::length_parameter_calculators
