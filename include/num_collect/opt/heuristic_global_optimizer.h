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
 * \brief Definition of heuristic_global_optimizer class.
 */
#pragma once

#include <type_traits>  // IWYU pragma: keep

#include "num_collect/base/index_type.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/concepts/multi_variate_objective_function.h"
#include "num_collect/opt/concepts/objective_function.h"
#include "num_collect/opt/concepts/single_variate_objective_function.h"
#include "num_collect/opt/dividing_rectangles.h"
#include "num_collect/opt/downhill_simplex.h"
#include "num_collect/opt/golden_section_search.h"
#include "num_collect/opt/optimizer_base.h"
#include "num_collect/opt/sampling_optimizer.h"

namespace num_collect::opt {

//! Tag of heuristic_global_optimizer.
constexpr auto heuristic_global_optimizer_tag =
    logging::log_tag_view("num_collect::opt::heuristic_global_optimizer");

/*!
 * \brief Class to perform global optimization using heuristics.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <concepts::objective_function ObjectiveFunction>
class heuristic_global_optimizer;

/*!
 * \brief Class to perform global optimization in 1 dimension using heuristics.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <concepts::single_variate_objective_function ObjectiveFunction>
class heuristic_global_optimizer<ObjectiveFunction>
    : public optimizer_base<heuristic_global_optimizer<ObjectiveFunction>> {
public:
    //! This class.
    using this_type = heuristic_global_optimizer<ObjectiveFunction>;

    //! Type of the objective function.
    using objective_function_type = ObjectiveFunction;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

    static_assert(std::is_same_v<variable_type, value_type>);

    /*!
     * \brief Constructor.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit heuristic_global_optimizer(
        const objective_function_type& obj_fun = objective_function_type())
        : optimizer_base<heuristic_global_optimizer<ObjectiveFunction>>(
              heuristic_global_optimizer_tag),
          opt1_(obj_fun),
          opt2_(obj_fun) {}

    /*!
     * \brief Initialize the algorithm.
     *
     * \param[in] lower Lower limit.
     * \param[in] upper Upper limit.
     */
    void init(const variable_type& lower, const variable_type& upper) {
        opt1_.init(lower, upper);
        opt2_.init(opt1_.lower(), opt1_.upper());
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() { opt2_.iterate(); }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return opt2_.is_stop_criteria_satisfied();
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
        return opt2_.opt_variable();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_value
     */
    [[nodiscard]] auto opt_value() const -> const value_type& {
        return opt2_.opt_value();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterations
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return opt1_.iterations() + opt2_.iterations();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::evaluations
     */
    [[nodiscard]] auto evaluations() const noexcept -> index_type {
        return opt1_.evaluations() + opt2_.evaluations();
    }

private:
    //! First optimizer.
    sampling_optimizer<objective_function_type> opt1_;

    //! Second optimizer.
    golden_section_search<objective_function_type> opt2_;
};

/*!
 * \brief Class to perform global optimization in multiple variables using
 * heuristics.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <concepts::multi_variate_objective_function ObjectiveFunction>
class heuristic_global_optimizer<ObjectiveFunction>
    : public optimizer_base<heuristic_global_optimizer<ObjectiveFunction>> {
public:
    //! This class.
    using this_type = heuristic_global_optimizer<ObjectiveFunction>;

    //! Type of the objective function.
    using objective_function_type = ObjectiveFunction;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of scalars in variables.
    using variable_scalar_type = typename variable_type::Scalar;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

    /*!
     * \brief Constructor.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit heuristic_global_optimizer(
        const objective_function_type& obj_fun = objective_function_type())
        : optimizer_base<heuristic_global_optimizer<ObjectiveFunction>>(
              heuristic_global_optimizer_tag),
          opt1_(obj_fun),
          opt2_(obj_fun) {
        opt1_.max_evaluations(default_opt1_max_evaluations);
    }

    /*!
     * \brief Initialize the algorithm.
     *
     * \param[in] lower Lower limit.
     * \param[in] upper Upper limit.
     */
    void init(const variable_type& lower, const variable_type& upper) {
        opt1_.init(lower, upper);
        current_optimizer_index_ = 1;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() {
        if (current_optimizer_index_ == 1) {
            if (!opt1_.is_stop_criteria_satisfied()) {
                opt1_.iterate();
                return;
            }
            opt2_.init(opt1_.opt_variable());
            current_optimizer_index_ = 2;
        }
        opt2_.iterate();
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        if (current_optimizer_index_ == 1) {
            return false;
        }
        return opt2_.is_stop_criteria_satisfied();
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
        iteration_logger.template append<index_type>(
            "Stage", &this_type::current_optimizer_index);
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_variable
     */
    [[nodiscard]] auto opt_variable() const -> const variable_type& {
        if (current_optimizer_index_ == 1) {
            return opt1_.opt_variable();
        }
        return opt2_.opt_variable();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_value
     */
    [[nodiscard]] auto opt_value() const -> const value_type& {
        if (current_optimizer_index_ == 1) {
            return opt1_.opt_value();
        }
        return opt2_.opt_value();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterations
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        if (current_optimizer_index_ == 1) {
            return opt1_.iterations();
        }
        return opt1_.iterations() + opt2_.iterations();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::evaluations
     */
    [[nodiscard]] auto evaluations() const noexcept -> index_type {
        if (current_optimizer_index_ == 1) {
            return opt1_.evaluations();
        }
        return opt1_.evaluations() + opt2_.evaluations();
    }

    /*!
     * \brief Set the maximum number of function evaluations in the first
     * optimizer.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto opt1_max_evaluations(index_type value) -> heuristic_global_optimizer& {
        opt1_.max_evaluations(value);
        return *this;
    }

    /*!
     * \brief Set tolerance of size of simplex in the second optimizer.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto opt2_tol_simplex_size(
        const variable_scalar_type& value) -> heuristic_global_optimizer& {
        opt2_.tol_simplex_size(value);
        return *this;
    }

    /*!
     * \brief Configure this optimizer for easy problems.
     *
     * \return This object.
     */
    auto light_mode() -> heuristic_global_optimizer& {
        constexpr index_type max_evaluations = 20;
        opt1_max_evaluations(max_evaluations);
        return *this;
    }

    /*!
     * \brief Configure this optimizer for middle problems.
     *
     * \return This object.
     */
    auto middle_mode() -> heuristic_global_optimizer& {
        constexpr index_type max_evaluations = default_opt1_max_evaluations;
        opt1_max_evaluations(max_evaluations);
        return *this;
    }

    /*!
     * \brief Configure this optimizer for difficult problems.
     *
     * \return This object.
     */
    auto heavy_mode() -> heuristic_global_optimizer& {
        constexpr index_type max_evaluations = 10000;
        opt1_max_evaluations(max_evaluations);
        return *this;
    }

private:
    /*!
     * \brief Get the current optimizer index.
     *
     * \return Current optimizer index.
     */
    [[nodiscard]] auto current_optimizer_index() const noexcept -> index_type {
        return current_optimizer_index_;
    }

    //! First optimizer.
    dividing_rectangles<ObjectiveFunction> opt1_;

    //! Second optimizer.
    downhill_simplex<ObjectiveFunction> opt2_;

    //! Current optimizer index.
    index_type current_optimizer_index_{1};

    //! Default maximum number of function evaluations in the first optimizer.
    static constexpr index_type default_opt1_max_evaluations = 1000;
};

}  // namespace num_collect::opt
