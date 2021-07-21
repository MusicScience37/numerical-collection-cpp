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

#include <Eigen/Core>
#include <cmath>
#include <limits>
#include <tuple>

#include "num_collect/constants/pi.h"
#include "num_collect/constants/zero.h"
#include "num_collect/functions/legendre.h"
#include "num_collect/roots/newton_raphson.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/index_type.h"

namespace num_collect::functions {

namespace impl {

/*!
 * \brief Class of Legendre function to use with
 * num_collect::roots::newton_raphson class.
 *
 * \tparam T Type of variable.
 */
template <typename T>
class legendre_for_newton {
public:
    //! Type of variables.
    using variable_type = T;

    //! Type of Jacobians.
    using jacobian_type = T;

    /*!
     * \brief Construct.
     *
     * \param[in] order Order of Legendre function.
     */
    explicit legendre_for_newton(index_type order) : order_(order) {
        NUM_COLLECT_ASSERT(order > 0);
    }

    /*!
     * \brief Evaluate on a variable.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(const variable_type& variable) {
        std::tie(value_, jacobian_) = legendre_with_diff(variable, order_);
    }

    /*!
     * \brief Get function value.
     *
     * \return Function value.
     */
    [[nodiscard]] auto value() const noexcept -> const variable_type& {
        return value_;
    }

    /*!
     * \brief Get Jacobian.
     *
     * \return Jacobian.
     */
    [[nodiscard]] auto jacobian() const noexcept -> const jacobian_type& {
        return jacobian_;
    }

private:
    //! Order of Legendre function.
    index_type order_;

    //! Function value.
    variable_type value_{};

    //! Jacobian matrix.
    jacobian_type jacobian_{};
};

}  // namespace impl

/*!
 * \brief Class of roots of Legendre function.
 *
 * \tparam T Type of variables.
 */
template <typename T>
class legendre_roots {
public:
    //! Type of variables.
    using variable_type = T;

    /*!
     * \brief Construct.
     *
     * \param[in] order Order of Legendre function.
     */
    explicit legendre_roots(index_type order = 0) : order_(order) {
        if (order == 0) {
            roots_.resize(0);
            return;
        }
        compute(order);
    }

    /*!
     * \brief Compute roots of Legendre function.
     *
     * \param[in] order Order of Legendre function.
     */
    void compute(index_type order) {
        NUM_COLLECT_ASSERT(order > 0);
        order_ = order;

        roots_.resize(order_);
        const index_type roots_to_solve = order_ / 2;

        using function_type = impl::legendre_for_newton<variable_type>;
        using solver_type = roots::newton_raphson<function_type>;
        auto solver = solver_type(function_type(order_));
        constexpr auto tol_last_change =
            std::numeric_limits<variable_type>::epsilon() *
            static_cast<variable_type>(1e+2);
        constexpr auto tol_value_norm =
            std::numeric_limits<variable_type>::epsilon() *
            static_cast<variable_type>(1e+2);
        solver.tol_last_change(tol_last_change);
        solver.tol_value_norm(tol_value_norm);

        for (index_type i = 0; i < roots_to_solve; ++i) {
            using std::cos;
            constexpr auto offset_in_num = static_cast<variable_type>(0.75);
            constexpr auto offset_in_den = static_cast<variable_type>(0.5);
            const variable_type init_var = cos(constants::pi<variable_type> *
                (static_cast<variable_type>(i) + offset_in_num) /
                (static_cast<variable_type>(order_) + offset_in_den));

            solver.init(init_var);
            solver.solve();
            roots_[i] = solver.variable();
        }
        const index_type center = (order_ - 1) / 2;
        if (order_ % 2 == 1) {
            roots_[center] = constants::zero<variable_type>;
        }
        for (index_type i = center + 1; i < order_; ++i) {
            roots_[i] = -roots_[order_ - 1 - i];
        }
    }

    /*!
     * \brief Get the order of Legendre function.
     *
     * \return Order of Legendre function.
     */
    [[nodiscard]] auto order() const noexcept -> index_type { return order_; }

    /*!
     * \brief Get the number of roots.
     *
     * \return Number of roots.
     */
    [[nodiscard]] auto size() const noexcept -> index_type {
        return roots_.size();
    }

    /*!
     * \brief Get the i-th root.
     *
     * \param[in] i Index.
     * \return i-th root.
     */
    [[nodiscard]] auto root(index_type i) const -> variable_type {
        NUM_COLLECT_DEBUG_ASSERT(i >= 0);
        NUM_COLLECT_DEBUG_ASSERT(i < roots_.size());
        return roots_[i];
    }

    /*!
     * \brief Get the i-th root.
     *
     * \param[in] i Index.
     * \return i-th root.
     */
    [[nodiscard]] auto operator[](index_type i) const -> variable_type {
        return root(i);
    }

private:
    //! Order of Legendre function.
    index_type order_;

    //! List of roots.
    Eigen::Matrix<variable_type, Eigen::Dynamic, 1> roots_{};
};

}  // namespace num_collect::functions
