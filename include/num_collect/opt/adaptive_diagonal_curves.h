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
 * \brief Definition of dividing_rectangles class.
 */
#pragma once

#include <Eigen/Core>
#include <cmath>
#include <limits>
#include <queue>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "num_collect/opt/impl/ternary_vector.h"
#include "num_collect/opt/optimizer_base.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/is_eigen_vector.h"

namespace num_collect::opt {

namespace impl {

/*!
 * \brief Class of dictionaries of sampling points in \ref
 * num_collect::opt::adaptive_diagonal_curves.
 *
 * \tparam ObjectiveFunction Type of objective function.
 */
template <typename ObjectiveFunction, typename = void>
class adc_sample_dict;

/*!
 * \brief Class of dictionaries of sampling points in \ref
 * num_collect::opt::adaptive_diagonal_curves.
 *
 * \tparam ObjectiveFunction Type of objective function.
 */
template <typename ObjectiveFunction>
class adc_sample_dict<ObjectiveFunction,
    std::enable_if_t<
        is_eigen_vector_v<typename ObjectiveFunction::variable_type>>> {
public:
    //! Type of the objective function.
    using objective_function_type = ObjectiveFunction;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

    /*!
     * \brief Construct.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit adc_sample_dict(
        const objective_function_type& obj_fun = objective_function_type())
        : obj_fun_(obj_fun) {}

    /*!
     * \brief Initialize this object.
     *
     * \param[in] lower Element-wise lower limit.
     * \param[in] upper Element-wise upper limit.
     */
    void init(const variable_type& lower, const variable_type& upper) {
        NUM_COLLECT_ASSERT(lower.size() == upper.size());
        NUM_COLLECT_ASSERT((lower.array() < upper.array()).all());
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
    [[nodiscard]] auto operator()(const ternary_vector& point) -> value_type {
        if (const auto iter = value_dict_.find(point);
            iter != value_dict_.end()) {
            return iter->second;
        }

        NUM_COLLECT_DEBUG_ASSERT(point.dim() == dim_);
        auto var = variable_type(dim_);
        for (index_type i = 0; i < dim_; ++i) {
            var(i) = lower_(i) +
                width_(i) * point.elem_as<typename variable_type::Scalar>(i);
        }
        obj_fun_.evaluate_on(var);
        value_dict_.try_emplace(point, obj_fun_.value());

        if (evaluations() == 1 || obj_fun_.value() < opt_value_) {
            opt_variable_ = var;
            opt_value_ = obj_fun_.value();
        }

        return obj_fun_.value();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_variable
     */
    [[nodiscard]] auto opt_variable() const -> const variable_type& {
        return opt_variable_;
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
        return value_dict_.size();
    }

private:
    //! Objective function.
    objective_function_type obj_fun_;

    //! Element-wise lower limit.
    variable_type lower_{};

    //! Element-wise width.
    variable_type width_{};

    //! Number of dimension.
    index_type dim_{0};

    //! Dictionary of sampled points.
    std::unordered_map<impl::ternary_vector, value_type> value_dict_{};

    //! Current optimal variable.
    variable_type opt_variable_{};

    //! Current optimal value.
    value_type opt_value_{};
};

/*!
 * \brief Class of rectangles as proposed in \cite Sergeyev2000 for \ref
 * num_collect::opt::adaptive_diagonal_curves.
 */
template <typename Value>
class adc_rectangle {
public:
    //! Type of function values.
    using value_type = Value;

    /*!
     * \brief Construct.
     *
     * \param[in] vertex A vertex with lower components.
     * \param[in] ave_value Average function value.
     */
    adc_rectangle(
        const impl::ternary_vector& vertex, const value_type& ave_value)
        : vertex_(vertex), ave_value_(ave_value) {}

    /*!
     * \brief Get the vertex with lower first component.
     *
     * \return A vertex with lower first component.
     */
    [[nodiscard]] auto vertex() const -> const impl::ternary_vector& {
        return vertex_;
    }

    /*!
     * \brief Get the average function value.
     *
     * \return Average function value.
     */
    [[nodiscard]] auto ave_value() const -> const value_type& {
        return ave_value_;
    }

    /*!
     * \brief Determine sampling points.
     *
     * \return Sampling points.
     */
    [[nodiscard]] auto sample_points() const
        -> std::pair<ternary_vector, ternary_vector> {
        auto res = std::make_pair(vertex_, vertex_);
        const auto dim = vertex_.dim();
        for (index_type i = 0; i < dim; ++i) {
            const auto digits = vertex_.digits(i);
            NUM_COLLECT_DEBUG_ASSERT(digits > 0);
            std::uint_fast32_t one_count = 0;
            for (index_type j = 0; j < digits; ++j) {
                if (vertex_(i, j) == ternary_vector::digit_type(1)) {
                    ++one_count;
                }
            }

            auto last_digit =  // NOLINTNEXTLINE: false positive
                static_cast<std::int_fast32_t>(vertex_(i, digits - 1));
            ++last_digit;
            constexpr std::uint_fast32_t odd_mask = 1;
            if ((one_count & odd_mask) == odd_mask) {
                res.first(i, digits - 1) =
                    static_cast<ternary_vector::digit_type>(last_digit);
            } else {
                res.second(i, digits - 1) =
                    static_cast<ternary_vector::digit_type>(last_digit);
            }
        }
        return res;
    }

    /*!
     * \brief Get the distance between center point and vertex.
     *
     * \return Distance between center point and vertex.
     */
    [[nodiscard]] auto dist() const -> value_type {
        auto squared_sum = value_type(0);
        for (index_type i = 0; i < vertex_.dim(); ++i) {
            using std::pow;
            squared_sum +=
                pow(static_cast<value_type>(3), -2 * (vertex_.digits(i) - 1));
        }
        using std::sqrt;
        const auto half = static_cast<value_type>(0.5);
        return half * sqrt(squared_sum);
    }

    /*!
     * \brief Class to compare rectangles.
     */
    struct greater {
        /*!
         * \brief Compare rectangles.
         *
         * \param[in] left Left-hand-side rectangle.
         * \param[in] right Right-hand-side rectangle.
         * \return Result of left > right.
         */
        [[nodiscard]] auto operator()(
            const std::shared_ptr<adc_rectangle>& left,
            const std::shared_ptr<adc_rectangle>& right) const -> bool {
            return left->value() > right->value();
        }
    };

private:
    //! A vertex with lower first component.
    impl::ternary_vector vertex_;

    //! Average function value.
    value_type ave_value_;
};

}  // namespace impl

/*!
 * \brief Class of adaptive diagonal curves (ADC) method \cite Sergeyev2006 for
 * optimization.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <typename ObjectiveFunction>
class adaptive_diagonal_curves
    : public optimizer_base<adaptive_diagonal_curves<ObjectiveFunction>> {
public:
    //! Type of the objective function.
    using objective_function_type = ObjectiveFunction;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

    /*!
     * \brief Construct.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit adaptive_diagonal_curves(
        const objective_function_type& obj_fun = objective_function_type())
        : value_dict_(obj_fun) {}

    /*!
     * \brief Enumeration of states in ADC method.
     */
    enum class state_type {
        local,        //!< Local phase (not last iteration).
        local_last,   //!< Last iteration in local phase.
        global,       //!< Global phase (not last iteration).
        global_last,  //!< Last iteration in global phase.
    };

private:
    //! Type of dictionaries of sample points.
    using dict_type = impl::adc_sample_dict<objective_function_type>;

    //! Type of rectangles.
    using rectangle_type = impl::adc_rectangle<value_type>;

    /*!
     * \brief Class of groups of rectangles.
     */
    class group {
    public:
        //! Type of queues of rectangles.
        using queue_type = std::priority_queue<std::shared_ptr<rectangle_type>,
            std::vector<std::shared_ptr<rectangle_type>>,
            typename rectangle_type::greater>;

        /*!
         * \brief Construct.
         *
         * \param[in] dist Distance between center point and vertex.
         */
        explicit group(value_type dist) : dist_(dist) {}

        /*!
         * \brief Get rectangles
         *
         * \return Rectangles
         */
        [[nodiscard]] auto rects() const -> const queue_type& { return rects_; }

        /*!
         * \brief Get rectangles
         *
         * \return Rectangles
         */
        [[nodiscard]] auto rects() -> queue_type& { return rects_; }

        /*!
         * \brief Get the distance between center point and vertex.
         *
         * \return Distance between center point and vertex.
         */
        [[nodiscard]] auto dist() const -> const value_type& { return dist_; }

    private:
        //! Rectangles.
        queue_type rects_{};

        //! Distance between center point and vertex.
        value_type dist_;
    };

    //! Dictionary of sampled points.
    dict_type value_dict_;

    //! Rectangle groups.
    std::vector<group> groups_{};

    //! Number of iterations.
    index_type iterations_{0};

    //! Last state.
    state_type last_state_{};

    //! Next state.
    state_type next_state_{};

    //! Default maximum number of function evaluations.
    static constexpr index_type default_max_evaluations = 10000;

    //! Maximum number of function evaluations.
    index_type max_evaluations_{default_max_evaluations};
};

}  // namespace num_collect::opt
