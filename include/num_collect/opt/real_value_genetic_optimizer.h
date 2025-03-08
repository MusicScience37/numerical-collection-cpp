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
 * \brief Definition of real_value_genetic_optimizer class.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <limits>
#include <random>
#include <string_view>
#include <utility>

#include <Eigen/Core>

#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/concepts/multi_variate_objective_function.h"
#include "num_collect/opt/concepts/objective_function.h"
#include "num_collect/opt/optimizer_base.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/vector.h"

namespace num_collect::opt {

//! Tag of real_value_genetic_optimizer.
constexpr auto real_value_genetic_optimizer_tag =
    logging::log_tag_view("num_collect::opt::real_value_genetic_optimizer");

/*!
 * \brief Class to perform optimization for real-valued variables using genetic
 * algorithm.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 * \tparam BitsPerDimension Number of bits per dimension.
 */
template <concepts::objective_function ObjectiveFunction,
    index_type BitsPerDimension = 10>  // NOLINT(*-magic-numbers)
class real_value_genetic_optimizer;

/*!
 * \brief Class to perform optimization for real-valued variables using genetic
 * algorithm.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 * \tparam BitsPerDimension Number of bits per dimension.
 */
template <concepts::multi_variate_objective_function ObjectiveFunction,
    index_type BitsPerDimension>
class real_value_genetic_optimizer<ObjectiveFunction, BitsPerDimension>
    : public optimizer_base<
          real_value_genetic_optimizer<ObjectiveFunction, BitsPerDimension>> {
public:
    //! This class.
    using this_type =
        real_value_genetic_optimizer<ObjectiveFunction, BitsPerDimension>;

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
     * \brief Constructor.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit real_value_genetic_optimizer(
        const ObjectiveFunction& obj_fun = ObjectiveFunction())
        : optimizer_base<this_type>(real_value_genetic_optimizer_tag),
          obj_fun_(obj_fun) {}

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

        binary_population_.reserve(population_size_);
        std::generate_n(
            std::back_inserter(binary_population_), population_size_, [this]() {
                binary_variable_type binary_var(dim_);
                std::generate(binary_var.begin(), binary_var.end(), [this]() {
                    return random_number_generator_() & binary_mask;
                });
                return binary_var;
            });

        population_values_.resize(population_size_);
        std::transform(binary_population_.begin(), binary_population_.end(),
            population_values_.begin(),
            [this](const auto& binary_var) { return evaluate_on(binary_var); });

        // Allocate memory here.
        prev_binary_population_ = binary_population_;
        buffer_variable_ = lower_;
        buffer_probabilities_.resize(population_size_);
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() {
        // Convert to positive values for probabilities.
        // The smaller the value, the higher the probability.
        buffer_probabilities_ = (-population_values_).array().exp();

        // Calculate cumulative probabilities.
        for (index_type i = 1; i < population_size_; ++i) {
            buffer_probabilities_[i] += buffer_probabilities_[i - 1];
        }
        const variable_scalar_type sum =
            buffer_probabilities_(population_size_ - 1);
        buffer_probabilities_ /= sum;
        buffer_probabilities_(population_size_ - 1) =
            static_cast<variable_scalar_type>(1);

        // Select parents.
        std::swap(binary_population_, prev_binary_population_);
        std::generate(
            binary_population_.begin(), binary_population_.end(), [this]() {
                const variable_scalar_type probability =
                    probability_distribution_(random_number_generator_);
                const auto iter =
                    std::lower_bound(buffer_probabilities_.begin(),
                        buffer_probabilities_.end(), probability);
                const index_type index = iter - buffer_probabilities_.begin();
                NUM_COLLECT_DEBUG_ASSERT(index < population_size_);
                return prev_binary_population_[index];
            });

        // Crossover.
        for (index_type i = 0; i < population_size_; i += 2) {
            const bool do_crossover =
                crossover_distribution_(random_number_generator_);
            if (!do_crossover) {
                continue;
            }
            for (index_type d = 0; d < dim_; ++d) {
                const binary_scalar_type mask =
                    random_number_generator_() & binary_mask;
                const binary_scalar_type mask_inv = (~mask) & binary_mask;
                const binary_scalar_type first_child_scalar =
                    (binary_population_[i][d] & mask) |
                    (binary_population_[i + 1][d] & mask_inv);
                const binary_scalar_type second_child_scalar =
                    (binary_population_[i][d] & mask_inv) |
                    (binary_population_[i + 1][d] & mask);
                binary_population_[i][d] = first_child_scalar;
                binary_population_[i + 1][d] = second_child_scalar;
            }
        }

        // Mutate.
        for (auto& binary_variable : binary_population_) {
            for (auto& binary_scalar : binary_variable) {
                auto mask = static_cast<binary_scalar_type>(1);
                for (index_type b = 0; b < num_bits_per_dimension; ++b) {
                    if (mutation_distribution_(random_number_generator_)) {
                        binary_scalar ^= mask;
                    }
                    mask <<= 1U;
                }
            }
        }

        // Evaluate function values.
        std::transform(binary_population_.begin(), binary_population_.end(),
            population_values_.begin(),
            [this](const auto& binary_var) { return evaluate_on(binary_var); });

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
        real_value_genetic_optimizer<ObjectiveFunction>>& iteration_logger)
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

    /*!
     * \brief Change the size of the population.
     *
     * \param[in] value Value.
     * \return This.
     */
    auto population_size(index_type value) -> this_type& {
        NUM_COLLECT_PRECONDITION(value > 0, this->logger(),
            "Population size must be a positive integer.");
        NUM_COLLECT_PRECONDITION(value % 2 == 0, this->logger(),
            "Population size must be an even number.");
        population_size_ = value;
        return *this;
    }

    /*!
     * \brief Change the probability of crossover.
     *
     * \param[in] value Value.
     * \return This.
     */
    auto crossover_probability(variable_scalar_type value) -> this_type& {
        NUM_COLLECT_PRECONDITION(
            static_cast<variable_scalar_type>(0) <= value &&
                value <= static_cast<variable_scalar_type>(1),
            this->logger(), "Probability of crossover must be in [0, 1].");
        crossover_distribution_ = std::bernoulli_distribution(value);
        return *this;
    }

    /*!
     * \brief Change the probability of mutation.
     *
     * \param[in] value Value.
     * \return This.
     */
    auto mutation_probability(variable_scalar_type value) -> this_type& {
        NUM_COLLECT_PRECONDITION(
            static_cast<variable_scalar_type>(0) <= value &&
                value <= static_cast<variable_scalar_type>(1),
            this->logger(), "Probability of mutation must be in [0, 1].");
        mutation_distribution_ = std::bernoulli_distribution(value);
        return *this;
    }

private:
    //! Number of bits per dimension.
    static constexpr index_type num_bits_per_dimension = BitsPerDimension;

    //! Type of binary representation of a scalar.
    using binary_scalar_type = std::uint32_t;

    //! Number of dimension at compile time.
    static constexpr index_type dim_at_compile_time =
        variable_type::RowsAtCompileTime;

    //! Type of binary representation of variables.
    using binary_variable_type =
        Eigen::Vector<binary_scalar_type, dim_at_compile_time>;

    //! Bit mask for binary representation.
    static constexpr binary_scalar_type binary_mask =
        (static_cast<binary_scalar_type>(1)
            << static_cast<binary_scalar_type>(num_bits_per_dimension)) -
        static_cast<binary_scalar_type>(1);

    /*!
     * \brief Evaluate a function value.
     *
     * \param[in] binary_variable Variable in binary representation.
     * \return Function value.
     */
    [[nodiscard]] auto evaluate_on(const binary_variable_type& binary_variable)
        -> value_type {
        static constexpr variable_scalar_type binary_to_rate =
            static_cast<variable_scalar_type>(1) /
            static_cast<variable_scalar_type>(binary_mask);
        buffer_variable_ =
            (binary_variable.template cast<variable_scalar_type>() *
                binary_to_rate)
                .cwiseProduct(width_) +
            lower_;

        obj_fun_.evaluate_on(buffer_variable_);
        const value_type value = correct_value_if_needed(obj_fun_.value());
        if (value < opt_value_) {
            opt_variable_ = buffer_variable_;
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

    //! Default size of population.
    static constexpr index_type default_population_size = 20;

    //! Size of population.
    index_type population_size_{default_population_size};

    //! Population in binary representation.
    util::vector<binary_variable_type> binary_population_;

    //! Previous population in binary representation.
    util::vector<binary_variable_type> prev_binary_population_;

    //! Function values of the population.
    Eigen::VectorX<value_type> population_values_;

    //! Random number generator.
    random_number_generator_type random_number_generator_{
        std::random_device()()};

    //! Distribution for probabilities.
    std::uniform_real_distribution<variable_scalar_type>
        probability_distribution_{static_cast<variable_scalar_type>(0),
            static_cast<variable_scalar_type>(1)};

    //! Default probability of crossover.
    static constexpr auto default_crossover_probability =
        static_cast<variable_scalar_type>(0.8);

    //! Distribution to determine whether to crossover.
    std::bernoulli_distribution crossover_distribution_{
        default_crossover_probability};

    //! Default probability of mutation.
    static constexpr auto default_mutation_probability =
        static_cast<variable_scalar_type>(0.1);

    //! Distribution to determine whether to mutate.
    std::bernoulli_distribution mutation_distribution_{
        default_mutation_probability};

    //! Buffer of a variable.
    variable_type buffer_variable_{};

    //! Buffer of probabilities.
    Eigen::VectorX<variable_scalar_type> buffer_probabilities_{};

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
