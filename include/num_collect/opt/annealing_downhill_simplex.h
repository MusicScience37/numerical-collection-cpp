/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of annealing_annealing_downhill_simplex class.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <random>
#include <string_view>
#include <vector>

#include <Eigen/Core>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/concepts/multi_variate_objective_function.h"  // IWYU pragma: keep
#include "num_collect/opt/concepts/objective_function.h"  // IWYU pragma: keep
#include "num_collect/opt/optimizer_base.h"
#include "num_collect/util/safe_cast.h"

namespace num_collect::opt {

//! Tag of annealing_downhill_simplex.
inline constexpr auto annealing_downhill_simplex_tag =
    logging::log_tag_view("num_collect::opt::annealing_downhill_simplex");

/*!
 * \brief Class of downhill simplex method with simulated annealing
 * \cite Press2007.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <concepts::objective_function ObjectiveFunction>
class annealing_downhill_simplex;

/*!
 * \brief Class of downhill simplex method with simulated annealing
 * \cite Press2007.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <concepts::multi_variate_objective_function ObjectiveFunction>
class annealing_downhill_simplex<ObjectiveFunction>
    : public optimizer_base<annealing_downhill_simplex<ObjectiveFunction>> {
public:
    // TODO Refactoring.

    //! This class.
    using this_type = annealing_downhill_simplex<ObjectiveFunction>;

    //! Type of the objective function.
    using objective_function_type = ObjectiveFunction;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of scalars in variables.
    using variable_scalar_type = typename variable_type::Scalar;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

    //! Type of the random number generator.
    using random_number_generator_type = std::mt19937;

    //! Type of processes.
    enum class process_type {
        none,                      //!< None.
        reflection,                //!< Reflection.
        reflection_and_expansion,  //!< Reflection and expansion.
        contraction,               //!< Contraction.
        multiple_contraction       //!< Multiple contraction.
    };

    /*!
     * \brief Convert type of process to string.
     *
     * \param[in] process Type of process.
     * \return Name of process.
     */
    [[nodiscard]] static auto process_name(process_type process)
        -> std::string_view {
        switch (process) {
        case process_type::none:
            return "none";
        case process_type::reflection:
            return "reflection";
        case process_type::reflection_and_expansion:
            return "reflection and expansion";
        case process_type::contraction:
            return "contraction";
        case process_type::multiple_contraction:
            return "multiple contraction";
        default:
            return "invalid process";
        }
    }

    /*!
     * \brief Constructor.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit annealing_downhill_simplex(
        const objective_function_type& obj_fun = objective_function_type())
        : optimizer_base<annealing_downhill_simplex<ObjectiveFunction>>(
              annealing_downhill_simplex_tag),
          obj_fun_(obj_fun) {}

    /*!
     * \brief Initialize the algorithm.
     *
     * \param[in] init_var An initial variable.
     * \param[in] width Width of the initial simplex.
     */
    void init(const variable_type& init_var,
        const variable_scalar_type& width = default_width) {
        dim_ = init_var.size();
        iterations_ = 0;
        evaluations_ = 0;
        iterations_in_current_trial_ = 0;
        temperature_ = highest_temperature_;
        opt_value_ = std::numeric_limits<value_type>::max();

        variable_type var = init_var;
        points_.reserve(util::safe_cast<std::size_t>(dim_ + 1));
        values_.reserve(util::safe_cast<std::size_t>(dim_ + 1));
        points_.push_back(var);
        values_.push_back(evaluate_on(var));
        for (index_type i = 0; i < dim_; ++i) {
            const variable_scalar_type val = var[i];
            var[i] += width;
            points_.push_back(var);
            values_.push_back(evaluate_on(var));
            var[i] = val;
        }

        value_order_.reserve(util::safe_cast<std::size_t>(dim_ + 1));
        for (std::size_t i = 0; i < util::safe_cast<std::size_t>(dim_ + 1);
             ++i) {
            value_order_.push_back(i);
        }
        fluctuated_values_.resize(util::safe_cast<std::size_t>(dim_ + 1));
        reorder();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() {
        const variable_type face_center = calc_face_center();
        const auto min_ind = value_order_.front();
        const auto second_max_ind =
            value_order_[util::safe_cast<std::size_t>(dim_ - 1)];
        const auto max_ind = value_order_.back();

        reflect(face_center);
        if (fluctuated_values_[max_ind] < fluctuated_values_[min_ind]) {
            expand(face_center);
        } else if (fluctuated_values_[max_ind] >=
            fluctuated_values_[second_max_ind]) {
            contract(face_center);
            if (fluctuated_values_[max_ind] >=
                fluctuated_values_[second_max_ind]) {
                multi_contract();
            }
        } else {
            // no operation
        }

        reorder();
        ++iterations_;
        ++iterations_in_current_trial_;
        if (iterations_in_current_trial_ >= max_iterations_per_trial_) {
            iterations_in_current_trial_ = 0;
        }
        using std::pow;
        temperature_ = highest_temperature_ *
            pow(static_cast<value_type>(1) -
                    static_cast<value_type>(iterations_in_current_trial_) /
                        static_cast<value_type>(max_iterations_per_trial_),
                4);
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return iterations() >= max_iterations_;
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
        iteration_logger.template append<variable_scalar_type>(
            "SimplexSize", &this_type::simplex_size);
        iteration_logger.template append<variable_scalar_type>(
            "Temperature", &this_type::temperature);
        constexpr index_type process_width = 26;
        iteration_logger
            .template append<std::string_view>(
                "Process", &this_type::last_process_name)
            ->width(process_width);
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
     * \brief Get last process.
     *
     * \return Last process.
     */
    [[nodiscard]] auto last_process() const noexcept -> process_type {
        return process_;
    }

    /*!
     * \brief Get the name of the last process.
     *
     * \return Last process.
     */
    [[nodiscard]] auto last_process_name() const noexcept -> std::string_view {
        return process_name(last_process());
    }

    /*!
     * \brief Get the size of simplex.
     *
     * \return Size of simplex.
     */
    [[nodiscard]] auto simplex_size() const -> variable_scalar_type {
        return (points_[value_order_.front()] - points_[value_order_.back()])
            .norm();
    }

    /*!
     * \brief Get the current temperature.
     *
     * \return Current temperature.
     */
    [[nodiscard]] auto temperature() const noexcept -> value_type {
        return temperature_;
    }

    /*!
     * \brief Change the seed of the random number generator.
     *
     * \param[in] value Value.
     * \return This.
     */
    auto seed(random_number_generator_type::result_type value)
        -> annealing_downhill_simplex& {
        random_number_generator_.seed(value);
        return *this;
    }

    /*!
     * \brief Set the highest temperature.
     *
     * \param[in] value Value.
     * \return This.
     */
    auto highest_temperature(value_type value) -> annealing_downhill_simplex& {
        if (value <= static_cast<value_type>(0)) {
            throw invalid_argument(
                "Highest temperature must be a positive number.");
        }
        highest_temperature_ = value;
        return *this;
    }

    /*!
     * \brief Set the maximum number of iterations in each trial.
     *
     * \param[in] value Value.
     * \return This.
     */
    auto max_iterations_per_trial(index_type value)
        -> annealing_downhill_simplex& {
        if (value == 0) {
            throw invalid_argument(
                "Maximum number of iterations in each trial must be a positive "
                "number.");
        }
        max_iterations_per_trial_ = value;
        return *this;
    }

    /*!
     * \brief Set the maximum number of iterations.
     *
     * \param[in] value Value.
     * \return This.
     */
    auto max_iterations(index_type value) -> annealing_downhill_simplex& {
        if (value == 0) {
            throw invalid_argument(
                "Maximum number of iterations must be a positive number.");
        }
        max_iterations_ = value;
        return *this;
    }

    //! Default width of simplex.
    static inline const auto default_width =
        static_cast<variable_scalar_type>(0.1);

private:
    /*!
     * \brief Evaluate function value.
     *
     * \param[in] variable Variable.
     * \return Function value.
     */
    [[nodiscard]] auto evaluate_on(const variable_type& variable) {
        obj_fun_.evaluate_on(variable);
        ++evaluations_;
        const auto& value = obj_fun_.value();
        if (value < opt_value_) {
            opt_value_ = value;
            opt_variable_ = variable;
        }
        return value;
    }

    /*!
     * \brief Reorder function values.
     */
    void reorder() {
        for (std::size_t i = 0; i < values_.size(); ++i) {
            fluctuated_values_[i] = values_[i] + generate_fluctuation();
        }
        std::sort(std::begin(value_order_), std::end(value_order_),
            [this](std::size_t i, std::size_t j) {
                return fluctuated_values_[i] < fluctuated_values_[j];
            });
    }

    /*!
     * \brief Calculate center of the face composed from points other than
     * highest point.
     *
     * \return Center.
     */
    [[nodiscard]] auto calc_face_center() const -> variable_type {
        variable_type face_center = variable_type::Zero(dim_);
        for (std::size_t i = 0; i < util::safe_cast<std::size_t>(dim_); ++i) {
            face_center += points_[value_order_[i]];
        }
        face_center /= static_cast<variable_scalar_type>(dim_);
        return face_center;
    }

    //! Two.
    static inline const auto twice = static_cast<variable_scalar_type>(2.0);

    //! Half.
    static inline const auto half = static_cast<variable_scalar_type>(0.5);

    /*!
     * \brief Reflect the highest point.
     *
     * \param[in] face_center Center of the face composed from points other than
     * highest point.
     */
    void reflect(const variable_type& face_center) {
        const std::size_t ind_move = value_order_.back();
        points_[ind_move] = twice * face_center - points_[ind_move];
        values_[ind_move] = evaluate_on(points_[ind_move]);
        fluctuated_values_[ind_move] =
            values_[ind_move] - generate_fluctuation();
        process_ = process_type::reflection;
    }

    /*!
     * \brief Expand the simplex.
     *
     * \param[in] face_center Center of the face composed from points other than
     * highest point.
     */
    void expand(const variable_type& face_center) {
        const std::size_t ind_move = value_order_.back();
        points_[ind_move] = twice * points_[ind_move] - face_center;
        values_[ind_move] = evaluate_on(points_[ind_move]);
        fluctuated_values_[ind_move] =
            values_[ind_move] - generate_fluctuation();
        process_ = process_type::reflection_and_expansion;
    }

    /*!
     * \brief Contract the highest point to the opposite face.
     *
     * \param[in] face_center Center of the face composed from points other than
     * highest point.
     */
    void contract(const variable_type& face_center) {
        const std::size_t ind_move = value_order_.back();
        points_[ind_move] = half * (points_[ind_move] + face_center);
        values_[ind_move] = evaluate_on(points_[ind_move]);
        fluctuated_values_[ind_move] =
            values_[ind_move] - generate_fluctuation();
        process_ = process_type::contraction;
    }

    /*!
     * \brief Contract all points other than the lowest point toward the lowest
     * point.
     */
    void multi_contract() {
        const auto& min_point = points_[value_order_.front()];
        for (std::size_t i = 1; i <= util::safe_cast<std::size_t>(dim_); ++i) {
            const std::size_t ind_move = value_order_[i];
            points_[ind_move] = half * (points_[ind_move] + min_point);
            values_[ind_move] = evaluate_on(points_[ind_move]);
            fluctuated_values_[ind_move] =
                values_[ind_move] - generate_fluctuation();
        }
        process_ = process_type::multiple_contraction;
    }

    /*!
     * \brief Generate a thermal fluctuation.
     *
     * \return Thermal fluctuation in the positive value.
     */
    [[nodiscard]] auto generate_fluctuation() -> value_type {
        using std::log;
        return temperature_ *
            log(static_cast<value_type>(random_number_generator_type::max()) /
                (static_cast<value_type>(random_number_generator_()) +
                    static_cast<value_type>(1)));
    }

    //! Objective function.
    objective_function_type obj_fun_;

    //! Number of dimension.
    index_type dim_{0};

    //! Points.
    std::vector<variable_type, Eigen::aligned_allocator<variable_type>>
        points_{};

    //! Function values.
    std::vector<value_type> values_{};

    //! Function values with thermal fluctuation.
    std::vector<value_type> fluctuated_values_{};

    //! Order of function values (ascending order).
    std::vector<std::size_t> value_order_{};

    //! Last process.
    process_type process_{process_type::none};

    //! Current optimal variable.
    variable_type opt_variable_{};

    //! Current optimal value.
    value_type opt_value_{};

    //! Number of iterations.
    index_type iterations_{0};

    //! Number of function evaluations.
    index_type evaluations_{0};

    //! Number of iterations in the current trial.
    index_type iterations_in_current_trial_{0};

    //! Generator of random numbers.
    random_number_generator_type random_number_generator_{
        std::random_device()()};

    //! Current temperature.
    value_type temperature_{};

    //! Default value of the highest temperature.
    static inline const auto default_highest_temperature =
        static_cast<value_type>(10);

    //! Highest temperature.
    value_type highest_temperature_{default_highest_temperature};

    //! Default value of the maximum number of iterations in each trial.
    static constexpr index_type default_max_iterations_per_trial = 100;

    //! Maximum number of iterations in each trial.
    index_type max_iterations_per_trial_{default_max_iterations_per_trial};

    //! Default maximum number of iterations.
    static constexpr index_type default_max_iterations = 1000;

    //! Maximum number of iterations.
    index_type max_iterations_{default_max_iterations};
};

}  // namespace num_collect::opt
