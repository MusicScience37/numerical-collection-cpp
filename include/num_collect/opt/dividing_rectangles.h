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
#include <limits>
#include <queue>
#include <tuple>
#include <type_traits>
#include <vector>

#include "num_collect/opt/optimizer_base.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/is_eigen_vector.h"

namespace num_collect::opt {

/*!
 * \brief Class of dividing rectangles (DIRECT) method for optimization.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 *
 * Reference:
 * D. R. Jones, C. D. Perttunen, and B. E. Stuckman. Lipschitzian optimization
 * without the lipschitz constant. Journal of Optimization Theory and
 * Application, Vol. 79, No. 1, 1993.
 */
template <typename ObjectiveFunction, typename = void>
class dividing_rectangles;

/*!
 * \brief Class of dividing rectangles (DIRECT) method for optimization.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 *
 * Reference:
 * D. R. Jones, C. D. Perttunen, and B. E. Stuckman. Lipschitzian optimization
 * without the lipschitz constant. Journal of Optimization Theory and
 * Application, Vol. 79, No. 1, 1993.
 */
template <typename ObjectiveFunction>
class dividing_rectangles<ObjectiveFunction,
    std::enable_if_t<
        is_eigen_vector_v<typename ObjectiveFunction::variable_type>>>
    : public optimizer_base<dividing_rectangles<ObjectiveFunction>> {
public:
    //! Type of the objective function.
    using objective_function_type = ObjectiveFunction;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

    static_assert(std::is_same_v<typename variable_type::Scalar, value_type>,
        "This class assumes that scalars in Eigen::Matrix class is same as "
        "value_type");

    /*!
     * \brief Construct.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit dividing_rectangles(
        const objective_function_type& obj_fun = objective_function_type())
        : obj_fun_(obj_fun) {}

    /*!
     * \brief Initialize the algorithm.
     *
     * \param[in] lower Lower limit.
     * \param[in] upper Upper limit.
     */
    void init(const variable_type& lower, const variable_type& upper) {
        NUM_COLLECT_ASSERT(lower.size() == upper.size());
        lower_ = lower;
        upper_ = upper;
        width_ = upper_ - lower_;
        dim_ = lower_.size();

        const auto half = static_cast<value_type>(0.5);
        const variable_type first_var =
            width_.cwiseProduct(variable_type::Constant(dim_, half)) + lower_;
        obj_fun_.evaluate_on(first_var);
        opt_variable_ = first_var;
        opt_value_ = obj_fun_.value();

        iterations_ = 0;
        evaluations_ = 1;

        rects_.clear();
        rects_.emplace_back();
        rects_[0].emplace(
            variable_type::Zero(dim_), variable_type::Ones(dim_), opt_value_);
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() {
        const auto divided_rects = determine_rects();
        for (auto iter = std::rbegin(divided_rects);
             iter != std::rend(divided_rects); ++iter) {
            divide_rect(iter->first);
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
     * \copydoc num_collect::opt::optimizer_base::iterations
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return iterations_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::evaluations
     */
    [[nodiscard]] auto evaluations() const noexcept -> index_type {
        return evaluations_;
    }

    /*!
     * \brief Set the maximum number of function evaluations.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto max_evaluations(index_type value) -> dividing_rectangles& {
        NUM_COLLECT_ASSERT(value > 0);
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
    auto min_rate_imp(variable_type value) -> dividing_rectangles& {
        NUM_COLLECT_ASSERT(value > 0);
        min_rate_imp_ = value;
        return *this;
    }

private:
    /*!
     * \brief Class of hyper rectanble in DIRECT method.
     */
    class rectangle {
    public:
        /*!
         * \brief Construct.
         *
         * \param[in] lower Element-wise lower limit.
         * \param[in] upper Element-wise upper limit.
         * \param[in] value Function value at the center.
         */
        rectangle(const variable_type& lower, const variable_type& upper,
            const value_type& value)
            : lower_(lower),
              upper_(upper),
              dist_((lower - upper).norm() * half),
              value_(value) {}

        /*!
         * \brief Get element-wise lower limit.
         *
         * \return Element-wise lower limit.
         */
        [[nodiscard]] auto lower() const -> const variable_type& {
            return lower_;
        }

        /*!
         * \brief Get element-wise upper limit.
         *
         * \return Element-wise upper limit.
         */
        [[nodiscard]] auto upper() const -> const variable_type& {
            return upper_;
        }

        /*!
         * \brief Get distance between center point and end point.
         *
         * \return Distance between center point and end point.
         */
        [[nodiscard]] auto dist() const -> const value_type& { return dist_; }

        /*!
         * \brief Get function value at the center.
         *
         * \return Function value at the center.
         */
        [[nodiscard]] auto value() const -> const value_type& { return value_; }

    private:
        //! Half.
        static inline const auto half = static_cast<value_type>(0.5);

        //! Element-wise lower limit.
        variable_type lower_;

        //! Element-wise upper limit.
        variable_type upper_;

        //! Distance between center point and end point.
        value_type dist_;

        //! Function value at the center.
        value_type value_;
    };

    /*!
     * \brief Class to compare rectangles.
     */
    struct greater_rectangle {
        /*!
         * \brief Compare rectangles.
         *
         * \param[in] left Left-hand-side rectangle.
         * \param[in] right Right-hand-side rectangle.
         * \return Result of left > right.
         */
        [[nodiscard]] auto operator()(
            const rectangle& left, const rectangle& right) const -> bool {
            return left.value() > right.value();
        }
    };

    /*!
     * \brief Evaluate function value.
     *
     * \param[in] variable Variable in unit hyper-cube.
     * \return Function value.
     */
    [[nodiscard]] auto evaluate_on(const variable_type& variable)
        -> value_type {
        variable_type actual_var = variable.cwiseProduct(width_) + lower_;
        obj_fun_.evaluate_on(actual_var);
        if (obj_fun_.value() < opt_value_) {
            opt_variable_ = actual_var;
            opt_value_ = obj_fun_.value();
        }
        ++evaluations_;
        return obj_fun_.value();
    }

    /*!
     * \brief Search rectangles to divide.
     *
     * \return List of (index in rects_, slope).
     */
    [[nodiscard]] auto determine_rects() const
        -> std::vector<std::pair<std::size_t, value_type>> {
        std::vector<std::pair<std::size_t, value_type>> search_rects;
        std::size_t ind = 0;
        // find the largest rectangle
        for (; ind < rects_.size(); ++ind) {
            if (!rects_[ind].empty()) {
                break;
            }
        }
        search_rects.emplace_back(ind, std::numeric_limits<value_type>::max());
        ++ind;
        // convex full scan
        for (; ind < rects_.size(); ++ind) {
            if (rects_[ind].empty()) {
                continue;
            }
            while (true) {
                const auto& [last_ind, last_slope] = search_rects.back();
                const auto slope =
                    calculate_slope(rects_[last_ind].top(), rects_[ind].top());
                if (slope <= last_slope) {
                    search_rects.emplace_back(ind, slope);
                    break;
                }
                search_rects.pop_back();
            }
        }
        // remove rectangles which won't update optimal value
        using std::abs;
        const auto value_bound = opt_value_ - min_rate_imp_ * abs(opt_value_);
        for (auto iter = search_rects.begin(); iter != search_rects.end();) {
            const auto& [ind, slope] = *iter;
            if (rects_[ind].top().value() - slope * rects_[ind].top().dist() <=
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
     * \param[in] larger_rect Larger rectangle.
     * \param[in] smaller_rect Smaller rectangle.
     * \return Slope.
     */
    [[nodiscard]] static auto calculate_slope(const rectangle& larger_rect,
        const rectangle& smaller_rect) -> value_type {
        return (larger_rect.value() - smaller_rect.value()) /
            (larger_rect.dist() - smaller_rect.dist());
    }

    /*!
     * \brief Divide a rectangle.
     *
     * \param[in] index Index in rects_.
     */
    void divide_rect(std::size_t index) {
        const auto& origin = rects_[index].top();
        if (index + 1 == rects_.size()) {
            rects_.emplace_back();
        }

        const auto divided_dim = widest_dimension(origin);
        const value_type divided_lowest = origin.lower()(divided_dim);
        const value_type divided_uppest = origin.upper()(divided_dim);
        const auto [divided_lower, divided_upper] =
            separate_section(divided_lowest, divided_uppest);

        variable_type lower = origin.lower();
        variable_type upper = origin.upper();
        upper(divided_dim) = divided_lower;
        static const auto half = static_cast<value_type>(0.5);
        variable_type center = (lower + upper) * half;
        value_type value = evaluate_on(center);
        rects_[index + 1].emplace(lower, upper, value);

        lower(divided_dim) = divided_lower;
        upper(divided_dim) = divided_upper;
        value = origin.value();
        rects_[index + 1].emplace(lower, upper, value);

        lower(divided_dim) = divided_upper;
        upper(divided_dim) = divided_uppest;
        center = (lower + upper) * half;
        value = evaluate_on(center);
        rects_[index + 1].emplace(lower, upper, value);

        rects_[index].pop();
    }

    /*!
     * \brief Find the widest dimension of a rectangle.
     *
     * \param[in] rect Rectangle.
     * \return Dimension index.
     */
    [[nodiscard]] static auto widest_dimension(const rectangle& rect)
        -> index_type {
        index_type res = 0;
        value_type max_width = value_type(0);
        for (index_type i = 0; i < rect.lower().size(); ++i) {
            const auto width = rect.upper()(i) - rect.lower()(i);
            if (width > max_width) {
                max_width = width;
                res = i;
            }
        }
        return res;
    }

    /*!
     * \brief Separate a section.
     *
     * \param[in] lowest Lower limit of the section.
     * \param[in] uppest Upper limit of the section.
     * \return Lower, upper point to separate the section.
     */
    [[nodiscard]] static auto separate_section(value_type lowest,
        value_type uppest) -> std::pair<value_type, value_type> {
        const auto width = uppest - lowest;
        static const auto one_third = static_cast<value_type>(1.0 / 3.0);
        static const auto two_thirds = static_cast<value_type>(2.0 / 3.0);
        return {lowest + one_third * width, lowest + two_thirds * width};
    }

    //! Objective function.
    objective_function_type obj_fun_;

    /*!
     * \brief Rectangles.
     *
     * Rectangles are listed per size.
     */
    std::vector<std::priority_queue<rectangle, std::vector<rectangle>,
        greater_rectangle>>
        rects_{};

    //! Element-wise lower limit.
    variable_type lower_{};

    //! Element-wise upper limit.
    variable_type upper_{};

    //! Element-wise width.
    variable_type width_{};

    //! Number of dimension.
    index_type dim_{0};

    //! Current optimal variable.
    variable_type opt_variable_{};

    //! Current optimal value.
    value_type opt_value_{};

    //! Number of iterations.
    index_type iterations_{0};

    //! Number of function evaluations.
    index_type evaluations_{0};

    //! Default maximum number of function evaluations.
    static constexpr index_type default_max_evaluations = 1000;

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
};

}  // namespace num_collect::opt
