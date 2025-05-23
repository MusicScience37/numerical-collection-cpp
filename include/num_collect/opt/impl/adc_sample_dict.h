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
 * \brief Definition of adc_sample_dict class.
 */
#pragma once

#include <cstddef>
#include <string_view>

#include <hash_tables/maps/multi_open_address_map_st.h>

#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/opt/concepts/multi_variate_objective_function.h"
#include "num_collect/opt/concepts/objective_function.h"
#include "num_collect/opt/impl/adc_ternary_vector.h"
#include "num_collect/util/assert.h"

namespace num_collect::opt::impl {

/*!
 * \brief Class of dictionaries of sampling points in \ref
 * num_collect::opt::adaptive_diagonal_curves.
 *
 * \tparam ObjectiveFunction Type of objective function.
 */
template <concepts::objective_function ObjectiveFunction>
class adc_sample_dict;

/*!
 * \brief Class of dictionaries of sampling points in \ref
 * num_collect::opt::adaptive_diagonal_curves.
 *
 * \tparam ObjectiveFunction Type of objective function.
 */
template <concepts::multi_variate_objective_function ObjectiveFunction>
class adc_sample_dict<ObjectiveFunction> {
public:
    //! Type of the objective function.
    using objective_function_type = ObjectiveFunction;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

    /*!
     * \brief Constructor.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit adc_sample_dict(
        const objective_function_type& obj_fun = objective_function_type())
        : obj_fun_(obj_fun) {
        constexpr std::size_t initial_space = 10000;
        value_dict_.reserve_approx(initial_space);
    }

    /*!
     * \brief Change the objective function.
     *
     * \param[in] obj_fun Objective function.
     */
    void change_objective_function(const objective_function_type& obj_fun) {
        obj_fun_ = obj_fun;
    }

    /*!
     * \brief Initialize this object.
     *
     * \param[in] lower Element-wise lower limit.
     * \param[in] upper Element-wise upper limit.
     */
    void init(const variable_type& lower, const variable_type& upper) {
        NUM_COLLECT_PRECONDITION(lower.size() == upper.size(),
            "Element-wise limits must have the same size.");
        NUM_COLLECT_PRECONDITION((lower.array() < upper.array()).all(),
            "Element-wise limits must satisfy lower < upper for each element.");

        lower_ = lower;
        width_ = upper - lower;
        dim_ = lower.size();
        value_dict_.clear();
    }

    /*!
     * \brief Evaluate or get function value.
     *
     * \warning This function assumes that init() has already been called.
     *
     * \param[in] point Point in the unit hyper-cube.
     * \return Function value.
     */
    [[nodiscard]] auto operator()(const adc_ternary_vector& point)
        -> value_type {
        return value_dict_.get_or_create_with_factory(
            point, [this, &point] { return evaluate_on(point); });
    }

    /*!
     * \brief Get the number of dimension.
     *
     * \return Number of dimension.
     */
    [[nodiscard]] auto dim() const -> index_type { return dim_; }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_variable
     */
    [[nodiscard]] auto opt_variable() const -> const variable_type& {
        return opt_variable_;
    }

    /*!
     * \brief Get the point in the unit hyper-cube for the current optimal
     * variable.
     *
     * \return Point in the unit hyper-cube for the current optimal variable.
     */
    [[nodiscard]] auto opt_point() const -> const adc_ternary_vector& {
        return opt_point_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_value
     */
    [[nodiscard]] auto opt_value() const -> const value_type& {
        return opt_value_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::evaluations
     */
    [[nodiscard]] auto evaluations() const noexcept -> index_type {
        return static_cast<index_type>(value_dict_.size());
    }

private:
    /*!
     * \brief Evaluate function value.
     *
     * \warning This function assumes that init() has already been called.
     *
     * \param[in] point Point in the unit hyper-cube.
     * \return Function value.
     */
    [[nodiscard]] auto evaluate_on(const adc_ternary_vector& point)
        -> value_type {
        NUM_COLLECT_DEBUG_ASSERT(point.dim() == dim_);
        auto var = variable_type(dim_);
        for (index_type i = 0; i < dim_; ++i) {
            var(i) = lower_(i) +
                width_(i) * point.elem_as<typename variable_type::Scalar>(i);
        }
        obj_fun_.evaluate_on(var);

        if (value_dict_.empty() || obj_fun_.value() < opt_value_) {
            opt_point_ = point;
            opt_variable_ = var;
            opt_value_ = obj_fun_.value();
        }

        return obj_fun_.value();
    }

    //! Objective function.
    objective_function_type obj_fun_;

    //! Element-wise lower limit.
    variable_type lower_{};

    //! Element-wise width.
    variable_type width_{};

    //! Number of dimension.
    index_type dim_{0};

    //! Dictionary of sampled points.
    hash_tables::maps::multi_open_address_map_st<impl::adc_ternary_vector,
        value_type>
        value_dict_{};

    //! Point in the unit hyper-cube for the current optimal variable.
    adc_ternary_vector opt_point_{};

    //! Current optimal variable.
    variable_type opt_variable_{};

    //! Current optimal value.
    value_type opt_value_{};
};

}  // namespace num_collect::opt::impl
