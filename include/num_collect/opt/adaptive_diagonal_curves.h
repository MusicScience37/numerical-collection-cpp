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

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <string_view>
#include <utility>
#include <vector>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/opt/concepts/multi_variate_objective_function.h"
#include "num_collect/opt/impl/adc_group.h"
#include "num_collect/opt/impl/adc_sample_dict.h"
#include "num_collect/opt/optimizer_base.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/safe_cast.h"

namespace num_collect::opt {

//! Tag of adaptive_diagonal_curves.
constexpr auto adaptive_diagonal_curves_tag =
    logging::log_tag_view("num_collect::opt::adaptive_diagonal_curves");

/*!
 * \brief Class of adaptive diagonal curves (ADC) method \cite Sergeyev2006 for
 * optimization.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 * \tparam MaxDigits Maximum number of ternary digits per dimension at compile
 * time.
 */
template <concepts::multi_variate_objective_function ObjectiveFunction,
    index_type MaxDigits = 8>
class adaptive_diagonal_curves
    : public optimizer_base<
          adaptive_diagonal_curves<ObjectiveFunction, MaxDigits>> {
public:
    //! This class.
    using this_type = adaptive_diagonal_curves<ObjectiveFunction, MaxDigits>;

    //! Type of the objective function.
    using objective_function_type = ObjectiveFunction;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

    /*!
     * \brief Enumeration of states in ADC method.
     */
    enum class state_type : std::uint8_t {
        none,          //!< No operation.
        local,         //!< Local phase (not last iteration).
        local_last,    //!< Last iteration in local phase.
        global,        //!< Global phase (not last iteration).
        global_last,   //!< Last iteration in global phase.
        non_dividable  //!< No rectangle can be divided.
    };

    /*!
     * \brief Convert a state to string.
     *
     * \param[in] state State.
     * \return Name of state.
     */
    [[nodiscard]] static auto state_name(state_type state) -> std::string_view {
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
        case state_type::non_dividable:
            return "non dividable";
        default:
            return "invalid process";
        }
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit adaptive_diagonal_curves(
        const objective_function_type& obj_fun = objective_function_type())
        : optimizer_base<this_type>(adaptive_diagonal_curves_tag),
          value_dict_(obj_fun) {}

    /*!
     * \brief Change the objective function.
     *
     * \param[in] obj_fun Objective function.
     */
    void change_objective_function(const objective_function_type& obj_fun) {
        value_dict_.change_objective_function(obj_fun);
    }

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

        // Initialize groups_, optimal_value_, optimal_group_index_.
        create_first_rectangle();

        // prec_optimal_value_, prec_optimal_group_index, and
        // iterations_in_current_phase_ are initialized in switch_state().

        value_dict_.reserve(max_evaluations_);
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
        case state_type::non_dividable:
            // Nothing can be done.
            break;
        default:
            NUM_COLLECT_LOG_AND_THROW(algorithm_failure,
                "invalid state (bug in adaptive_diagonal_curve class)");
        }

        ++iterations_;
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return evaluations() >= max_evaluations_ ||
            state_ == state_type::non_dividable;
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::configure_iteration_logger
     */
    void configure_iteration_logger(
        logging::iterations::iteration_logger<this_type>& iteration_logger)
        const {
        iteration_logger.template append<index_type>(
            "Iter.", &this_type::iterations);
        iteration_logger.template append<index_type>(
            "Eval.", &this_type::evaluations);
        iteration_logger.template append<value_type>(
            "Value", &this_type::opt_value);
        constexpr index_type state_width = 15;
        iteration_logger
            .template append<std::string_view>(
                "State", &this_type::last_state_name)
            ->width(state_width);
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
     * \brief Get the name of the last state.
     *
     * \return Last state.
     */
    [[nodiscard]] auto last_state_name() const noexcept -> std::string_view {
        return state_name(last_state());
    }

    /*!
     * \brief Set the maximum number of function evaluations.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto max_evaluations(index_type value) -> adaptive_diagonal_curves& {
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "Maximum number of function evaluations must be a positive "
            "integer.");

        max_evaluations_ = value;
        return *this;
    }

    /*!
     * \brief Set the minimum rate of improvement in the function value required
     * for potentially optimal rectangles.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto min_rate_imp(value_type value) -> adaptive_diagonal_curves& {
        NUM_COLLECT_PRECONDITION(value > static_cast<value_type>(0),
            this->logger(),
            "Minimum rate of improvement in the function value required for "
            "potentially optimal rectangles must be a positive value.");
        min_rate_imp_ = value;
        return *this;
    }

    /*!
     * \brief Set the rate of function value used to check whether the function
     * value decreased in the current phase.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto decrease_rate_bound(value_type value) -> adaptive_diagonal_curves& {
        NUM_COLLECT_PRECONDITION(value > static_cast<value_type>(0),
            this->logger(),
            "Rate of function value used to check whether the function value "
            "decreased in the current phase must be a positive value.");
        decrease_rate_bound_ = value;
        return *this;
    }

private:
    //! Type of dictionaries of sample points.
    using dict_type = impl::adc_sample_dict<objective_function_type, MaxDigits>;

    //! Type of ternary vectors.
    using ternary_vector_type = typename dict_type::ternary_vector_type;

    //! Type of groups of hyper-rectangles.
    using group_type = impl::adc_group<value_type, ternary_vector_type>;

    //! Type of hyper-rectangles.
    using rectangle_type = typename group_type::rectangle_type;

    /*!
     * \brief Create the first hyper-rectangle.
     */
    void create_first_rectangle() {
        const index_type dim = value_dict_.dim();
        auto point = ternary_vector_type{dim};
        for (index_type i = 0; i < dim; ++i) {
            point.push_back(0);
        }

        const auto [lower_vertex, upper_vertex] =
            rectangle_type::determine_sample_points(point);
        const auto lower_vertex_value = value_dict_(lower_vertex);
        const auto upper_vertex_value = value_dict_(upper_vertex);
        const auto ave_value = half * (lower_vertex_value + upper_vertex_value);
        const auto rect = rectangle_type(point, ave_value);

        groups_.emplace_back(rect.dist());
        groups_.front().push(rect);
        using std::min;
        optimal_value_ = min(lower_vertex_value, upper_vertex_value);
        optimal_group_index_ = 0;
    }

    /*!
     * \brief Switch to the next state if necessary.
     *
     * Step 2.1, 2.4, 3, 4.4, 4.5, 4.7 in \cite Sergeyev2006.
     */
    void switch_state() {
        using std::abs;
        switch (state_) {
        case state_type::local:
            ++iterations_in_current_phase_;
            if (iterations_in_current_phase_ > value_dict_.dim()) {
                state_ = state_type::local_last;
            }
            return;
        case state_type::local_last:
            switch_state_on_local_last();
            return;
        case state_type::global:
            if (optimal_value_ <= prec_optimal_value_ -
                    decrease_rate_bound_ * abs(prec_optimal_value_)) {
                state_ = state_type::local;
                prec_optimal_value_ = optimal_value_;
                iterations_in_current_phase_ = 1;
                prec_optimal_group_index_ = optimal_group_index_;
                return;
            }
            ++iterations_in_current_phase_;
            if (iterations_in_current_phase_ >
                util::safe_cast<index_type>((static_cast<std::size_t>(2)
                    << util::safe_cast<std::size_t>(value_dict_.dim())))) {
                state_ = state_type::global_last;
                return;
            }
            return;
        case state_type::global_last:
            if (optimal_value_ <= prec_optimal_value_ -
                    decrease_rate_bound_ * abs(prec_optimal_value_)) {
                state_ = state_type::local;
                prec_optimal_value_ = optimal_value_;
                iterations_in_current_phase_ = 1;
                prec_optimal_group_index_ = optimal_group_index_;
                return;
            }
            state_ = state_type::global;
            iterations_in_current_phase_ = 1;
            prec_optimal_group_index_ = optimal_group_index_;
            return;
        case state_type::non_dividable:
            // Nothing can be done.
            return;
        default:
            state_ = state_type::local;
            prec_optimal_value_ = optimal_value_;
            iterations_in_current_phase_ = 1;
            prec_optimal_group_index_ = optimal_group_index_;
        }
    }

    /*!
     * \brief Switch to the next state if necessary in local_last state.
     */
    void switch_state_on_local_last() {
        using std::abs;
        if (optimal_value_ <= prec_optimal_value_ -
                decrease_rate_bound_ * abs(prec_optimal_value_)) {
            state_ = state_type::local;
            prec_optimal_value_ = optimal_value_;
            iterations_in_current_phase_ = 1;
            prec_optimal_group_index_ = optimal_group_index_;
            return;
        }

        const bool is_optimal_smallest =
            (optimal_group_index_ == groups_.size() - 1);
        const bool is_all_smallest =
            std::all_of(std::begin(groups_), std::end(groups_) - 1,
                [](const group_type& group) { return group.empty(); });
        if ((!is_optimal_smallest) || is_all_smallest) {
            state_ = state_type::local;
            iterations_in_current_phase_ = 1;
            prec_optimal_group_index_ = optimal_group_index_;
            return;
        }

        state_ = state_type::global;
        prec_optimal_value_ = optimal_value_;
        iterations_in_current_phase_ = 1;
        prec_optimal_group_index_ = optimal_group_index_;
    }

    /*!
     * \brief Iterate once in the local phase (not last iteration).
     */
    void iterate_locally() {
        const std::size_t max_group_index = std::max<std::size_t>(
            std::max<std::size_t>(prec_optimal_group_index_, 1) - 1,
            min_nonempty_group_index());
        divide_nondominated_rectangles(
            min_nonempty_group_index(), max_group_index);
    }

    /*!
     * \brief Iterate once at the last of the local phase.
     */
    void iterate_locally_last() {
        const std::size_t max_group_index = std::max<std::size_t>(
            prec_optimal_group_index_, min_nonempty_group_index());
        divide_nondominated_rectangles(
            min_nonempty_group_index(), max_group_index);
    }

    /*!
     * \brief Iterate once in the global phase (not last iteration).
     */
    void iterate_globally() {
        const std::size_t max_group_index =
            (std::max<std::size_t>(
                 prec_optimal_group_index_, min_nonempty_group_index()) +
                min_nonempty_group_index() + 1) /
            2;
        divide_nondominated_rectangles(
            min_nonempty_group_index(), max_group_index);
    }

    /*!
     * \brief Iterate once at teh last of the global phase.
     */
    void iterate_globally_last() { iterate_locally_last(); }

    /*!
     * \brief Get the minimum index of non-empty groups.
     *
     * \return Minimum index of groups.
     */
    [[nodiscard]] auto min_nonempty_group_index() const -> std::size_t {
        for (std::size_t i = 0; i < groups_.size(); ++i) {
            if (!groups_[i].empty()) {
                return i;
            }
        }
        NUM_COLLECT_LOG_AND_THROW(precondition_not_satisfied,
            "adaptive_diagonal_curves::init is not called.");
    }

    /*!
     * \brief Divide nondominated hyper-rectangles.
     *
     * \param[in] min_group Minimum index of groups to search in.
     * \param[in] max_group Maximum index of groups to search in.
     */
    void divide_nondominated_rectangles(
        std::size_t min_group, std::size_t max_group) {
        const auto search_rect =
            determine_nondominated_rectangles(min_group, max_group);
        bool divided_rectangle = false;
        for (auto iter = std::rbegin(search_rect);
            iter != std::rend(search_rect); ++iter) {
            if (divide_rectangle(iter->first)) {
                divided_rectangle = true;
            }
        }
        if (!divided_rectangle) {
            state_ = state_type::non_dividable;
            this->logger().warning()(
                "No rectangle can be divided. "
                "Stop the iteration or tune the parameter MaxDigits.");
        }
    }

    /*!
     * \brief Determine nondominated hyper-rectangles.
     *
     * \param[in] min_group Minimum index of groups to search in.
     * \param[in] max_group Maximum index of groups to search in.
     * \return List of (index of group, slope).
     */
    [[nodiscard]] auto determine_nondominated_rectangles(
        std::size_t min_group, std::size_t max_group) const
        -> std::vector<std::pair<std::size_t, value_type>> {
        NUM_COLLECT_DEBUG_ASSERT(min_group <= max_group);
        NUM_COLLECT_DEBUG_ASSERT(max_group < groups_.size());
        NUM_COLLECT_DEBUG_ASSERT(!groups_[min_group].empty());

        std::vector<std::pair<std::size_t, value_type>> search_rects;
        search_rects.emplace_back(
            min_group, std::numeric_limits<value_type>::max());

        // convex full scan
        for (std::size_t i = min_group + 1; i <= max_group; ++i) {
            if (groups_[i].empty()) {
                continue;
            }
            while (true) {
                const auto& [last_i, last_slope] = search_rects.back();
                const auto slope = calculate_slope(last_i, i);
                if (slope <= last_slope) {
                    search_rects.emplace_back(i, slope);
                    break;
                }
                search_rects.pop_back();
            }
        }

        // remove rectangles which won't update optimal value
        using std::abs;
        const auto value_bound =
            optimal_value_ - min_rate_imp_ * abs(optimal_value_);
        for (auto iter = search_rects.begin(); iter != search_rects.end();) {
            const auto& [ind, slope] = *iter;
            if (groups_[ind].min_rect().ave_value() -
                    slope * groups_[ind].dist() <=
                value_bound) {
                ++iter;
            } else {
                iter = search_rects.erase(iter);
            }
        }

        return search_rects;
    }

    /*!
     * \brief Calculate slope.
     *
     * \param[in] group_ind1 Index of group.
     * \param[in] group_ind2 Index of group.
     * \return Slope.
     */
    [[nodiscard]] auto calculate_slope(
        std::size_t group_ind1, std::size_t group_ind2) const -> value_type {
        return (groups_[group_ind1].min_rect().ave_value() -
                   groups_[group_ind2].min_rect().ave_value()) /
            (groups_[group_ind1].dist() - groups_[group_ind2].dist());
    }

    /*!
     * \brief Divide a hyper-rectangle.
     *
     * \param[in] group_ind Index of group.
     * \retval true The hyper-rectangle is divided.
     * \retval false The hyper-rectangle is not divided.
     */
    [[nodiscard]] auto divide_rectangle(std::size_t group_ind) -> bool {
        if (!groups_[group_ind].is_dividable()) {
            return false;
        }

        impl::adc_ternary_vector vertex =
            std::move(groups_[group_ind].pop().vertex());

        const auto [dim, digit] = vertex.push_back(0);
        const auto rect0 = create_rect(vertex, group_ind + 1);
        vertex(dim, digit) = 1;
        const auto rect1 = create_rect(vertex, group_ind + 1);
        vertex(dim, digit) = 2;
        const auto rect2 = create_rect(vertex, group_ind + 1);

        if (groups_.size() == group_ind + 1) {
            groups_.emplace_back(rect0.dist());
        }
        groups_[group_ind + 1].push(rect0);
        groups_[group_ind + 1].push(rect1);
        groups_[group_ind + 1].push(rect2);

        return true;
    }

    /*!
     * \brief Create a hyper-rectangle.
     *
     * \param[in] vertex Vertex with lower first component.
     * \param[in] group_ind Group index.
     * \return Hyper-rectangle.
     */
    [[nodiscard]] auto create_rect(const ternary_vector_type& vertex,
        std::size_t group_ind) -> rectangle_type {
        const auto [vertex1, vertex2] =
            rectangle_type::determine_sample_points(vertex);
        const auto value1 = value_dict_(vertex1);
        const auto value2 = value_dict_(vertex2);
        const auto ave_value = half * (value1 + value2);
        if (value1 < optimal_value_) {
            optimal_value_ = value1;
            optimal_group_index_ = group_ind;
        } else if (value1 <= optimal_value_ &&
            group_ind > optimal_group_index_) {
            optimal_group_index_ = group_ind;
        }
        if (value2 < optimal_value_) {
            optimal_value_ = value2;
            optimal_group_index_ = group_ind;
        } else if (value2 <= optimal_value_ &&
            group_ind > optimal_group_index_) {
            optimal_group_index_ = group_ind;
        }
        return rectangle_type(vertex, ave_value);
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
    std::size_t optimal_group_index_{0};

    //! Old optimal value at the start of the current phase.
    value_type prec_optimal_value_{};

    /*!
     * \brief Index of the group in which the old optimal solution at the start
     * of the current phase exists.
     */
    std::size_t prec_optimal_group_index_{0};

    /*!
     * \brief Number of iterations in the current phase.
     *
     * This is initialized at the start of the local or global phases.
     */
    index_type iterations_in_current_phase_{0};

    //! Default maximum number of function evaluations.
    static constexpr index_type default_max_evaluations = 10000;

    //! Maximum number of function evaluations.
    index_type max_evaluations_{default_max_evaluations};

    /*!
     * \brief Default minimum rate of improvement in the function value required
     * for potentially optimal rectangles.
     */
    static inline const auto default_min_rate_imp =
        static_cast<value_type>(1e-4);

    /*!
     * \brief Minimum rate of improvement in the function value required for
     * potentially optimal rectangles.
     */
    value_type min_rate_imp_{default_min_rate_imp};

    /*!
     * \brief Default rate of function value used to check whether the function
     * value decreased in the current phase.
     */
    static inline const auto default_decrease_rate_bound =
        static_cast<value_type>(0.01);

    /*!
     * \brief Rate of function value used to check whether the function value
     * decreased in the current phase.
     */
    value_type decrease_rate_bound_{default_decrease_rate_bound};
};

}  // namespace num_collect::opt
