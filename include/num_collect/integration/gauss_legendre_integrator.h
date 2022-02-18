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
 * \brief Definition of legendre_roots function.
 */
#pragma once

#include <type_traits>

#include <Eigen/Core>

#include "num_collect/base/concepts/invocable_as.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/half.h"
#include "num_collect/constants/one.h"
#include "num_collect/constants/two.h"
#include "num_collect/functions/legendre.h"
#include "num_collect/functions/legendre_roots.h"
#include "num_collect/util/assert.h"

namespace num_collect::integration {

/*!
 * \brief Class to perform numerical integration with Gauss-Legendre formula.
 *
 * \tparam Signature Function signature.
 */
template <typename Signature>
class gauss_legendre_integrator;

/*!
 * \brief Class to perform numerical integration with Gauss-Legendre formula.
 *
 * \tparam Result Type of results.
 * \tparam Variable Type of variables.
 */
template <typename Result, base::concepts::real_scalar Variable>
class gauss_legendre_integrator<Result(Variable)> {
public:
    //! Type of variables.
    using variable_type = std::decay_t<Variable>;

    //! Type of results.
    using result_type = std::decay_t<Result>;

    //! Default order.
    static constexpr index_type default_order = 20;

    /*!
     * \brief Construct.
     *
     * \param[in] order Order.
     */
    explicit gauss_legendre_integrator(index_type order = default_order)
        : roots_(order) {
        NUM_COLLECT_ASSERT(order > 0);
        update_weight();
    }

    /*!
     * \brief Compute internal variables for integration.
     *
     * \param[in] order Order.
     */
    void prepare(index_type order) {
        NUM_COLLECT_ASSERT(order > 0);
        roots_.compute(order);
        update_weight();
    }

    /*!
     * \brief Integrate a function.
     *
     * \tparam Function Type of function.
     * \param[in] function Function.
     * \param[in] left Left boundary.
     * \param[in] right Right boundary.
     * \return Result.
     */
    template <base::concepts::invocable_as<result_type(variable_type)> Function>
    [[nodiscard]] auto integrate(const Function& function, variable_type left,
        variable_type right) const -> result_type {
        const auto order = roots_.order();
        const auto mean = constants::half<variable_type> * (left + right);
        const auto half_width = constants::half<variable_type> * (right - left);
        Result sum = function(mean) * constants::zero<variable_type>;
        for (index_type i = 0; i < order; ++i) {
            const variable_type x = mean + half_width * roots_[i];
            const variable_type weight = weights_[i];
            sum += weight * function(x);
        }
        return sum * half_width;
    }

    /*!
     * \brief Integrate a function.
     *
     * \tparam Function Type of function.
     * \param[in] function Function.
     * \param[in] left Left boundary.
     * \param[in] right Right boundary.
     * \return Result.
     */
    template <base::concepts::invocable_as<result_type(variable_type)> Function>
    [[nodiscard]] auto operator()(const Function& function, variable_type left,
        variable_type right) const -> result_type {
        return integrate(function, left, right);
    }

private:
    /*!
     * \brief Update weight for roots.
     */
    void update_weight() {
        const auto order = roots_.order();
        weights_.resize(order);
        for (index_type i = 0; i < order; ++i) {
            const variable_type x = roots_[i];
            const auto temp = static_cast<variable_type>(order) *
                functions::legendre(x, order - 1);
            weights_[i] = constants::two<variable_type> *
                (constants::one<variable_type> - x * x) / (temp * temp);
        }
    }

    //! Roots of Legendre function.
    functions::legendre_roots<variable_type> roots_;

    //! List of weights for roots.
    Eigen::Matrix<variable_type, Eigen::Dynamic, 1> weights_{};
};

}  // namespace num_collect::integration
