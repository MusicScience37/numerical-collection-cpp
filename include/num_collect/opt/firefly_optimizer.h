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
 * \brief Definition of firefly_optimizer class.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <random>
#include <string_view>

#include <Eigen/Core>

#include "num_collect/base/index_type.h"
#include "num_collect/base/isfinite.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/concepts/multi_variate_objective_function.h"
#include "num_collect/opt/concepts/objective_function.h"
#include "num_collect/opt/optimizer_base.h"
#include "num_collect/util/vector.h"

namespace num_collect::opt {

//! Tag of firefly_optimizer.
constexpr auto firefly_optimizer_tag =
    logging::log_tag_view("num_collect::opt::firefly_optimizer");

/*!
 * \brief Class to perform optimization using firefly algorithm
 * \cite Yang2009, \cite Yang2010.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <concepts::objective_function ObjectiveFunction>
class firefly_optimizer;

/*!
 * \brief Class to perform optimization using firefly algorithm
 * \cite Yang2009, \cite Yang2010.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <concepts::multi_variate_objective_function ObjectiveFunction>
class firefly_optimizer<ObjectiveFunction>
    : public optimizer_base<firefly_optimizer<ObjectiveFunction>> {
public:
    //! This class.
    using this_type = firefly_optimizer<ObjectiveFunction>;

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

    /*!
     * \brief Construct.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit firefly_optimizer(
        const ObjectiveFunction& obj_fun = ObjectiveFunction())
        : optimizer_base<this_type>(firefly_optimizer_tag), obj_fun_(obj_fun) {}

    /*!
     * \brief Change the objective function.
     *
     * \param[in] obj_fun Objective function.
     */
    void change_objective_function(const objective_function_type& obj_fun) {
        obj_fun_ = obj_fun;
    }

    /*!
     * \brief Initialize the algorithm.
     *
     * \param[in] lower Lower limit.
     * \param[in] upper Upper limit.
     */
    void init(const variable_type& lower, const variable_type& upper) {
        NUM_COLLECT_PRECONDITION(lower.size() == upper.size(), this->logger(),
            "Lower and upper limits must have the same size.");
        lower_ = lower;
        upper_ = upper;
        width_ = upper_ - lower_;
        dim_ = lower_.size();

        opt_value_ = std::numeric_limits<value_type>::max();
        iterations_ = 0;
        evaluations_ = 0;

        variables_.clear();
        variables_.reserve(num_fireflies_);
        variable_type variable(dim_);
        std::generate_n(std::back_inserter(variables_), num_fireflies_,
            [this, &variable]() -> const variable_type& {
                std::generate(variable.begin(), variable.end(), [this]() {
                    return initial_distribution_(random_number_generator_);
                });
                variable = lower_ + width_.cwiseProduct(variable);
                return variable;
            });

        values_.resize(num_fireflies_);
        std::transform(variables_.begin(), variables_.end(), values_.begin(),
            [this](const auto& variable) { return evaluate_on(variable); });

        // Allocate memory. (This value is not used.)
        variable_changes_ = variables_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() {
#pragma omp parallel
        {
            // Move to brighter fireflies.
#pragma omp for
            for (index_type i = 0; i < num_fireflies_; ++i) {
                variable_changes_[i] = variable_type::Zero(dim_);
                const variable_type& moved_variable = variables_[i];
                const value_type moved_value = values_[i];
                for (index_type j = 0; j < num_fireflies_; ++j) {
                    const value_type brighter_value = values_[j];
                    if (brighter_value >= moved_value) {
                        continue;
                    }
                    const variable_type& brighter_variable = variables_[j];
                    const variable_type diff =
                        brighter_variable - moved_variable;
                    const variable_scalar_type squared_distance =
                        diff.cwiseQuotient(width_).squaredNorm() /
                        static_cast<variable_scalar_type>(dim_);
                    using std::exp;
                    variable_changes_[i] += attractiveness_coeff_ *
                        exp(-absorption_coeff_ * squared_distance) * diff;
                }
            }
#pragma omp barrier
#pragma omp for
            for (index_type i = 0; i < num_fireflies_; ++i) {
                variables_[i] += variable_changes_[i];
            }
#pragma omp barrier

            // Random walk.
#pragma omp master
            {
                for (auto& variable : variables_) {
                    for (index_type d = 0; d < dim_; ++d) {
                        variable(d) += random_coeff_ *
                            random_walk_distribution_(
                                random_number_generator_) *
                            width_(d);
                    }
                }
            }
#pragma omp barrier

            // Move to the feasible region.
#pragma omp for
            for (index_type i = 0; i < num_fireflies_; ++i) {
                variables_[i] = variables_[i].cwiseMax(lower_).cwiseMin(upper_);
            }
        }

        // Update function values.
        std::transform(variables_.begin(), variables_.end(), values_.begin(),
            [this](const auto& variable) { return evaluate_on(variable); });
        ++iterations_;
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return evaluations_ >= max_evaluations_;
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::configure_iteration_logger
     */
    void configure_iteration_logger(logging::iterations::iteration_logger<
        firefly_optimizer<ObjectiveFunction>>& iteration_logger) const {
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
     * \brief Set the number of fireflies.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto num_fireflies(index_type value) -> this_type& {
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "Number of fireflies must be a positive integer.");
        num_fireflies_ = value;
        return *this;
    }

    /*!
     * \brief Set the coefficient of the attractiveness.
     *
     * \note This value is \f$\beta 0\f$ in \cite Yang2009.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto attractiveness_coeff(variable_scalar_type value) -> this_type& {
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "Coefficient of the attractiveness must be a positive number.");
        attractiveness_coeff_ = value;
        return *this;
    }

    /*!
     * \brief Set the light absorption coefficient.
     *
     * \note This value is \f$\gamma\f$ in \cite Yang2009.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto absorption_coeff(variable_scalar_type value) -> this_type& {
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "Light absorption coefficient must be a positive number.");
        absorption_coeff_ = value;
        return *this;
    }

    /*!
     * \brief Set the coefficient of the random walk.
     *
     * \note This value is \f$\alpha\f$ in \cite Yang2009.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto random_coeff(variable_scalar_type value) -> this_type& {
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "Coefficient of the random walk must be a positive number.");
        random_coeff_ = value;
        return *this;
    }

    /*!
     * \brief Set the maximum number of function evaluations.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto max_evaluations(index_type value) -> this_type& {
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "Maximum number of function evaluations must be a positive "
            "integer.");
        max_evaluations_ = value;
        return *this;
    }

    /*!
     * \brief Change the seed of the random number generator.
     *
     * \param[in] value Value.
     * \return This.
     */
    auto seed(random_number_generator_type::result_type value) -> this_type& {
        random_number_generator_.seed(value);
        return *this;
    }

private:
    /*!
     * \brief Evaluate a function value.
     *
     * \param[in] variable Variable.
     * \return Function value.
     */
    [[nodiscard]] auto evaluate_on(const variable_type& variable)
        -> value_type {
        obj_fun_.evaluate_on(variable);
        const value_type value = correct_value_if_needed(obj_fun_.value());
        if (value < opt_value_) {
            opt_variable_ = variable;
            opt_value_ = value;
        }
        ++evaluations_;
        return value;
    }

    /*!
     * \brief Correct function values if needed.
     *
     * \param[in] value Function value.
     * \return Corrected value.
     */
    [[nodiscard]] static auto correct_value_if_needed(value_type value) noexcept
        -> value_type {
        constexpr auto safe_limit = std::numeric_limits<value_type>::max();
        if (!isfinite(value)) {
            return safe_limit;
        }
        return value;
    }

    //! Objective function.
    objective_function_type obj_fun_;

    //! Element-wise lower limit.
    variable_type lower_{};

    //! Element-wise upper limit.
    variable_type upper_{};

    //! Element-wise width.
    variable_type width_{};

    //! Number of dimension.
    index_type dim_{0};

    //! Default value of the number of fireflies.
    static constexpr index_type default_num_fireflies = 200;

    //! Number of fireflies.
    index_type num_fireflies_{default_num_fireflies};

    //! Default value of the coefficient of the attractiveness.
    static constexpr auto default_attractiveness_coeff =
        static_cast<variable_scalar_type>(1);

    /*!
     * \brief Coefficient of the attractiveness.
     *
     * \note This value is \f$\beta 0\f$ in \cite Yang2009.
     */
    variable_scalar_type attractiveness_coeff_{default_attractiveness_coeff};

    //! Default value of the light absorption coefficient.
    static constexpr auto default_absorption_coeff =
        static_cast<variable_scalar_type>(300);

    /*!
     * \brief Light absorption coefficient.
     *
     * \note This value is \f$\gamma\f$ in \cite Yang2009.
     */
    variable_scalar_type absorption_coeff_{default_absorption_coeff};

    //! Default value of the coefficient of the random walk.
    static constexpr auto default_random_coeff =
        static_cast<variable_scalar_type>(0.1);

    /*!
     * \brief Coefficient of the random walk.
     *
     * \note This value is \f$\alpha\f$ in \cite Yang2009.
     */
    variable_scalar_type random_coeff_{default_random_coeff};

    //! Current variables. (Positions of fireflies.)
    util::vector<variable_type> variables_;

    //! Buffer of variable changes.
    util::vector<variable_type> variable_changes_;

    //! Function values of the current variables.
    Eigen::VectorX<value_type> values_;

    //! Random number generator.
    random_number_generator_type random_number_generator_{
        std::random_device()()};

    //! Distribution for the initial variables. (Uniform distribution from 0 to 1.)
    std::uniform_real_distribution<variable_scalar_type>
        initial_distribution_{};

    //! Distribution for random walk. (Standard normal distribution.)
    std::normal_distribution<variable_scalar_type> random_walk_distribution_{};

    //! Current optimal variable.
    variable_type opt_variable_{};

    //! Current optimal value.
    value_type opt_value_{std::numeric_limits<value_type>::max()};

    //! Number of iterations.
    index_type iterations_{0};

    //! Number of function evaluations.
    index_type evaluations_{0};

    //! Default maximum number of function evaluations.
    static constexpr index_type default_max_evaluations = 10000;

    //! Maximum number of function evaluations.
    index_type max_evaluations_{default_max_evaluations};
};

}  // namespace num_collect::opt
