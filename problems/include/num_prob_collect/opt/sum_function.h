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
 * \brief Definition of sum_function class.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/util/format_dense_vector.h"

namespace num_prob_collect::opt {

/*!
 * \brief Class of a test function of optimization algorithms
 * which calculates sum of variables.
 *
 * This function is used for testing box-constrained optimization algorithms
 * to check whether they can handle optimal solutions on the boundary correctly.
 */
template <num_collect::concepts::real_scalar_dense_vector Variable>
class sum_function {
public:
    //! Type of variables.
    using variable_type = Variable;

    //! Type of function values.
    using value_type = typename variable_type::Scalar;

    /*!
     * \brief Constructor.
     */
    sum_function() = default;

    /*!
     * \brief Evaluate function value on variable.
     *
     * \param[in] x Variable.
     */
    void evaluate_on(const variable_type& x) {
        num_collect::logging::logger logger;
        NUM_COLLECT_LOG_TRACE(logger, "Evaluating sum function at {}",
            num_collect::util::format_dense_vector(x));
        value_ = x.sum();
    }

    /*!
     * \brief Get function value.
     *
     * \return Function value.
     */
    [[nodiscard]] auto value() const -> const value_type& { return value_; }

private:
    //! Function value.
    value_type value_{};
};

}  // namespace num_prob_collect::opt
