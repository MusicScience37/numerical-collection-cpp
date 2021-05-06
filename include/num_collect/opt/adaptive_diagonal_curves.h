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
#include <algorithm>
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
            opt_point_ = point;
            opt_variable_ = var;
            opt_value_ = obj_fun_.value();
        }

        return obj_fun_.value();
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
    [[nodiscard]] auto opt_point() const -> const ternary_vector& {
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

    //! Point in the unit hyper-cube for the current optimal variable.
    ternary_vector opt_point_{};

    //! Current optimal variable.
    variable_type opt_variable_{};

    //! Current optimal value.
    value_type opt_value_{};
};

/*!
 * \brief Class of rectangles as proposed in \cite Sergeyev2000 for \ref
 * num_collect::opt::adaptive_diagonal_curves.
 *
 * \tparam Value Type of function values.
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
        return determine_sample_points(vertex_);
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
     * \brief Determine sampling points.
     *
     * \param[in] lowest_vertex A vertex with lower first component.
     * \return Sampling points.
     */
    [[nodiscard]] static auto determine_sample_points(
        const ternary_vector& lowest_vertex)
        -> std::pair<ternary_vector, ternary_vector> {
        auto res = std::make_pair(lowest_vertex, lowest_vertex);
        const auto dim = lowest_vertex.dim();
        for (index_type i = 0; i < dim; ++i) {
            const auto digits = lowest_vertex.digits(i);
            NUM_COLLECT_DEBUG_ASSERT(digits > 0);
            std::uint_fast32_t one_count = 0;
            for (index_type j = 0; j < digits; ++j) {
                if (lowest_vertex(i, j) == ternary_vector::digit_type(1)) {
                    ++one_count;
                }
            }

            auto last_digit =  // NOLINTNEXTLINE: false positive
                static_cast<std::int_fast32_t>(lowest_vertex(i, digits - 1));
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

private:
    //! A vertex with lower first component.
    impl::ternary_vector vertex_;

    //! Average function value.
    value_type ave_value_;
};

/*!
 * \brief Class of groups in \cite Sergeyev2006 for \ref
 * num_collect::opt::adaptive_diagonal_curves.
 *
 * \tparam Value Type of function values.
 */
template <typename Value>
class adc_group {
public:
    //! Type of function values.
    using value_type = Value;

    //! Type of hyper-rectangles.
    using rectangle_type = adc_rectangle<value_type>;

    //! Type of pointers of hyper-rectangles.
    using rectangle_pointer_type = std::shared_ptr<rectangle_type>;

    /*!
     * \brief Construct.
     *
     * \param[in] dist Distance between center point and vertex.
     */
    explicit adc_group(value_type dist) : dist_(dist) {}

    /*!
     * \brief Add a hyper-rectangle to this group.
     *
     * \param[in] rect Rectangle.
     */
    void push(rectangle_pointer_type rect) {
        NUM_COLLECT_DEBUG_ASSERT(rect);
        rects_.push(std::move(rect));
    }

    /*!
     * \brief Access the hyper-rectangle with the smallest average of
     * function values at diagonal vertices.
     *
     * \return Reference of pointer to the rectangle.
     */
    [[nodiscard]] auto min_rect() const -> const rectangle_pointer_type& {
        NUM_COLLECT_DEBUG_ASSERT(!rects_.empty());
        return rects_.top();
    }

    /*!
     * \brief Check whether this group is empty.
     *
     * \return Whether this group is empty.
     */
    [[nodiscard]] auto empty() const -> bool { return rects_.empty(); }

    /*!
     * \brief Pick out the hyper-rectangle with the smallest average of function
     * values at diagonal vertices.
     *
     * \return Rectangle.
     */
    [[nodiscard]] auto pop() -> rectangle_pointer_type {
        NUM_COLLECT_DEBUG_ASSERT(!rects_.empty());
        auto rect = rects_.top();
        rects_.pop();
        return rect;
    }

    /*!
     * \brief Get the distance between center point and vertex.
     *
     * \return Distance between center point and vertex.
     */
    [[nodiscard]] auto dist() const -> const value_type& { return dist_; }

private:
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
        [[nodiscard]] auto operator()(const rectangle_pointer_type& left,
            const rectangle_pointer_type& right) const -> bool {
            return left->ave_value() > right->ave_value();
        }
    };

    //! Type of queues of rectangles.
    using queue_type = std::priority_queue<rectangle_pointer_type,
        std::vector<rectangle_pointer_type>, greater>;

    //! Rectangles.
    queue_type rects_{};

    //! Distance between center point and vertex.
    value_type dist_;
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
     * \brief Enumeration of states in ADC method.
     */
    enum class state_type {
        none,         //!< No operation.
        local,        //!< Local phase (not last iteration).
        local_last,   //!< Last iteration in local phase.
        global,       //!< Global phase (not last iteration).
        global_last,  //!< Last iteration in global phase.
    };

    /*!
     * \brief Convert a state to string.
     *
     * \param[in] state State.
     * \return Name of state.
     */
    [[nodiscard]] static auto state_name(state_type state) -> std::string {
        switch (state) {
        case state_type::none:
            return "none";
        case state_type::local:
            return "local";
        case state_type::local_last:
            return "local (last)";
        case state_type::global:
            return "global";
        case state_type::global_last:
            return "global (last)";
        default:
            return "invalid process";
        }
    }

    /*!
     * \brief Construct.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit adaptive_diagonal_curves(
        const objective_function_type& obj_fun = objective_function_type())
        : value_dict_(obj_fun) {}

    /*!
     * \brief Initialize the algorithm.
     *
     * \param[in] lower Lower limit.
     * \param[in] upper Upper limit.
     */
    void init(const variable_type& lower, const variable_type& upper) {
        value_dict_.init(lower, upper);
        groups_.clear();
        iterations_ = 0;
        state_ = state_type::none;
        create_first_rectangle();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() {
        switch_state();

        switch (state_) {
        case state_type::local:
            iterate_locally();
            break;
        case state_type::local_last:
            iterate_locally_last();
            break;
        case state_type::global:
            iterate_globally();
            break;
        case state_type::global_last:
            iterate_globally_last();
            break;
        default:
            throw algorithm_failure(
                "invalid state (bug in adaptive_diagonal_curve class)");
        }

        ++iterations_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return evaluations() >= max_evaluations_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::set_info_to
     */
    void set_info_to(iteration_logger& logger) const {
        logger["Iter."] = iterations();
        logger["Eval."] = evaluations();
        logger["Value"] = static_cast<double>(opt_value());
        logger["State"] = state_name(last_state());
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_variable
     */
    [[nodiscard]] auto opt_variable() const -> const variable_type& {
        return value_dict_.opt_variable();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_value
     */
    [[nodiscard]] auto opt_value() const -> const value_type& {
        return value_dict_.opt_value();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterations
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return iterations_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::evaluations
     */
    [[nodiscard]] auto evaluations() const noexcept -> index_type {
        return value_dict_.evaluations();
    }

    /*!
     * \brief Get the last state.
     *
     * \return Last state.
     */
    [[nodiscard]] auto last_state() const noexcept -> state_type {
        return state_;
    }

    /*!
     * \brief Set the maximum number of function evaluations.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto max_evaluations(index_type value) -> adaptive_diagonal_curves& {
        NUM_COLLECT_ASSERT(value > 0);
        max_evaluations_ = value;
        return *this;
    }

private:
    //! Type of dictionaries of sample points.
    using dict_type = impl::adc_sample_dict<objective_function_type>;

    //! Type of groups of hyper-rectangles.
    using group_type = impl::adc_group<value_type>;

    //! Type of hyper-rectangles.
    using rectangle_type = typename group_type::rectangle_type;

    /*!
     * \brief Create the first hyper-rectangle.
     */
    void create_first_rectangle() {
        const index_type dim = value_dict_.dim();
        auto point = impl::ternary_vector(dim);
        for (index_type i = 0; i < dim; ++i) {
            point.push_back(i, 0);
        }

        const auto [lower_vertex, upper_vertex] =
            rectangle_type::determine_sample_points(point);
        const auto lower_vertex_value = value_dict_(lower_vertex);
        const auto upper_vertex_value = value_dict_(upper_vertex);
        const auto ave_value = half * (lower_vertex_value + upper_vertex_value);
        const auto rect = std::make_shared<rectangle_type>(point, ave_value);

        groups_.emplace_back(rect->dist());
        groups_.front().push(rect);
        using std::min;
        optimal_value_ = min(lower_vertex_value, upper_vertex_value);
        optimal_group_index_ = 0;
    }

    /*!
     * \brief Switch to the next state if necessary.
     */
    void switch_state() {
        // TODO
    }

    /*!
     * \brief Iterate once in the local phase (not last iteration).
     */
    void iterate_locally() {
        // TODO
    }

    /*!
     * \brief Iterate once at the last of the local phase.
     */
    void iterate_locally_last() {
        // TODO
    }

    /*!
     * \brief Iterate once in the global phase (not last iteration).
     */
    void iterate_globally() {
        // TODO
    }

    /*!
     * \brief Iterate once at teh last of the global phase.
     */
    void iterate_globally_last() {
        // TODO
    }

    //! Half.
    static inline const auto half = static_cast<value_type>(0.5);

    //! Dictionary of sampled points.
    dict_type value_dict_;

    //! Groups of hyper-rectangles.
    std::vector<group_type> groups_{};

    //! Number of iterations.
    index_type iterations_{0};

    //! State.
    state_type state_{state_type::none};

    /*!
     * \brief Current optimal value.
     *
     * This value is used for updating \ref optimal_group_index_.
     */
    value_type optimal_value_{};

    //! Index of the group in which the optimal solution exists.
    index_type optimal_group_index_{0};

    //! Default maximum number of function evaluations.
    static constexpr index_type default_max_evaluations = 10000;

    //! Maximum number of function evaluations.
    index_type max_evaluations_{default_max_evaluations};
};

}  // namespace num_collect::opt
