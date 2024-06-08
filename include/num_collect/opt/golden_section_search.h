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
 * \brief Definition of golden_section_search class.
 */
#pragma once

#include <cmath>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/concepts/single_variate_objective_function.h"
#include "num_collect/opt/optimizer_base.h"

namespace num_collect::opt {

//! Tag of golden_section_search.
constexpr auto golden_section_search_tag =
    logging::log_tag_view("num_collect::opt::golden_section_search");

/*!
 * \brief Class of golden section search method.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <concepts::single_variate_objective_function ObjectiveFunction>
class golden_section_search
    : public optimizer_base<golden_section_search<ObjectiveFunction>> {
public:
    //! This type.
    using this_type = golden_section_search<ObjectiveFunction>;

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
    explicit golden_section_search(
        const objective_function_type& obj_fun = objective_function_type())
        : optimizer_base<golden_section_search<ObjectiveFunction>>(
              golden_section_search_tag),
          obj_fun_(obj_fun) {}

    /*!
     * \brief Initialize the algorithm.
     *
     * \param[in] end1 An end point.
     * \param[in] end2 Another end point.
     */
    void init(variable_type end1, variable_type end2) {
        end1_ = end1;
        end2_ = end2;

        mid1_ = end1_ + (end2_ - end1_) * mid_point_ratio;
        obj_fun_.evaluate_on(mid1_);
        value1_ = obj_fun_.value();

        iterations_ = 0;
    }

    /*!
     * \brief Set the tolerance of length of section between end points.
     *
     * \param[in] value Tolerance of length of section between end points.
     * \return This object.
     */
    auto tol_section_len(variable_type value) -> golden_section_search& {
        tol_section_len_ = value;
        return *this;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() {
        const variable_type mid2 = end2_ + (end1_ - end2_) * mid_point_ratio;
        obj_fun_.evaluate_on(mid2);
        const value_type value2 = obj_fun_.value();
        if (value1_ < value2) {
            // (end1_, mid1_, end2_) = (mid2, mid1_, end1_)
            end2_ = end1_;
            end1_ = mid2;
        } else {
            // (end1_, mid1_, end2_) = (mid1_, mid2, end2_)
            end1_ = mid1_;
            mid1_ = mid2;
            value1_ = value2;
        }
        ++iterations_;
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return section_len() < tol_section_len_;
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
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_variable
     */
    [[nodiscard]] auto opt_variable() const -> const variable_type& {
        return mid1_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_value
     */
    [[nodiscard]] auto opt_value() const -> const value_type& {
        return value1_;
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
        return iterations_ + 1;
    }

    /*!
     * \brief Get length of section between end points.
     *
     * \return Length of section between end points.
     */
    [[nodiscard]] auto section_len() const -> variable_type {
        using std::abs;
        return abs(end1_ - end2_);
    }

    //! Ratio of middle point.
    static inline const auto mid_point_ratio =
        static_cast<variable_type>((3.0 - std::sqrt(5.0)) / 2.0);

    //! Default tolerance of length of section between end points.
    static inline const auto default_tol_section_len =
        static_cast<variable_type>(1e-3);

private:
    //! Objective function.
    objective_function_type obj_fun_;

    //! An end point.
    variable_type end1_{};

    //! Another end point.
    variable_type end2_{};

    //! Middle point near to end1_.
    variable_type mid1_{};

    //! Function value on end1_.
    value_type value1_{};

    //! Number of iterations.
    index_type iterations_{0};

    //! Tolerance of length of section between end points.
    variable_type tol_section_len_{default_tol_section_len};
};

}  // namespace num_collect::opt
