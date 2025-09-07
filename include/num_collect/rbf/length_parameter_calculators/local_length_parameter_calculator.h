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
 * \brief Definition of local_length_parameter_calculator class.
 */
#pragma once

#include <cstddef>
#include <limits>

#include <Eigen/Core>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/constants/zero.h"  // IWYU pragma: keep
#include "num_collect/logging/logging_macros.h"
#include "num_collect/rbf/concepts/distance_function.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/vector_view.h"

namespace num_collect::rbf::length_parameter_calculators {

/*!
 * \brief Class to calculate length parameters for RBF using length parameters
 * localized for each sample point.
 *
 * \tparam DistanceFunction Type of the distance function.
 */
template <concepts::distance_function DistanceFunction>
class local_length_parameter_calculator {
public:
    //! Type of the distance function.
    using distance_function_type = DistanceFunction;

    //! Type of variables.
    using variable_type = typename DistanceFunction::variable_type;

    //! Type of scalars.
    using scalar_type = typename DistanceFunction::value_type;

    //! Whether this calculator uses the globally fixed length parameters.
    static constexpr bool uses_global_length_parameter = false;

    /*!
     * \brief Constructor.
     */
    local_length_parameter_calculator() = default;

    /*!
     * \brief Compute the length parameters.
     *
     * \param[in] variables Variables.
     * \param[in] distance_function Distance function.
     */
    void compute(util::vector_view<const variable_type> variables,
        const distance_function_type& distance_function) {
        const index_type num_samples = variables.size();
        NUM_COLLECT_PRECONDITION(
            num_samples > 0, "Sample points must be given.");

        length_parameters_.resize(num_samples);
        // TODO parallelization for many points
        for (index_type i = 0; i < num_samples; ++i) {
            auto min_distance = std::numeric_limits<scalar_type>::max();
            for (index_type j = 0; j < num_samples; ++j) {
                if (i != j) {
                    const auto distance =
                        distance_function(variables[i], variables[j]);
                    if (distance < min_distance) {
                        min_distance = distance;
                    }
                }
            }
            length_parameters_(i) = scale_ * min_distance;
        }
    }

    /*!
     * \brief Get the length parameter at a point.
     *
     * \param[in] i Index of the point.
     * \return Length parameter.
     */
    [[nodiscard]] auto length_parameter_at(index_type i) const -> scalar_type {
        NUM_COLLECT_DEBUG_ASSERT(0 <= i);
        NUM_COLLECT_DEBUG_ASSERT(i < length_parameters_.size());
        return length_parameters_(i);
    }

    /*!
     * \brief Get the current scale of length parameters.
     *
     * \return Scale of length parameters.
     */
    [[nodiscard]] auto scale() const noexcept -> scalar_type { return scale_; }

    /*!
     * \brief Set the scale of length parameters.
     *
     * \param[in] value Value.
     */
    void scale(scalar_type value) {
        NUM_COLLECT_PRECONDITION(value > constants::zero<scalar_type>,
            "Scale of length parameters must be a positive number.");
        scale_ = value;
    }

private:
    //! Default value of the scalar of length parameters.
    static constexpr auto default_scale = static_cast<scalar_type>(10);

    //! Scale of length parameters.
    scalar_type scale_{default_scale};

    //! Length parameters.
    Eigen::VectorX<scalar_type> length_parameters_{};
};

}  // namespace num_collect::rbf::length_parameter_calculators
