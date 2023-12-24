/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of random_quadratic_function class.
 */
#pragma once

#include <random>

namespace num_prob_collect::opt {

/*!
 * \brief Class of quadratic functions with random coefficients and optimal
 * variables.
 */
class random_quadratic_function {
public:
    //! Type of variables.
    using variable_type = double;
    //! Type of function values.
    using value_type = double;

    /*!
     * \brief Constructor.
     *
     * \param[in] coeff Coefficient.
     * \param[in] optimal_variable Optimal variable.
     */
    random_quadratic_function(double coeff, double optimal_variable) noexcept
        : coeff_(coeff), optimal_variable_(optimal_variable) {}

    /*!
     * \brief Evaluate function value on variable.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(double variable) noexcept {
        const double diff = variable - optimal_variable_;
        value_ = coeff_ * diff * diff;
    }

    /*!
     * \brief Get function value.
     *
     * \return Function value.
     */
    [[nodiscard]] auto value() const noexcept -> const double& {
        return value_;
    }

    /*!
     * \brief Get the coefficient.
     *
     * \return Coefficient.
     */
    [[nodiscard]] auto coeff() const noexcept -> double { return coeff_; }

    /*!
     * \brief Get the optimal variable.
     *
     * \return Optimal variable.
     */
    [[nodiscard]] auto optimal_variable() const noexcept -> double {
        return optimal_variable_;
    }

private:
    //! Coefficient.
    double coeff_;

    //! Optimal variable.
    double optimal_variable_;

    //! Function value.
    double value_{0.0};
};

/*!
 * \brief Class to generate random quadratic functions.
 */
class random_quadratic_function_generator {
public:
    //! Minimum optimal variable.
    static constexpr double min_variable = -10.0;

    //! Maximum optimal variable.
    static constexpr double max_variable = 10.0;

    /*!
     * \brief Constructor.
     */
    random_quadratic_function_generator() noexcept = default;

    /*!
     * \brief Generate a random quadratic function.
     *
     * \return Generated quadratic function.
     */
    [[nodiscard]] auto operator()() noexcept -> random_quadratic_function {
        const double coeff = coeff_dist_(generator_);
        const double optimal_variable = optimal_variable_dist_(generator_);
        return random_quadratic_function(coeff, optimal_variable);
    }

private:
    //! Random number generator.
    std::mt19937
        generator_{};  // NOLINT(cert-msc32-c,cert-msc51-cpp): For reproducibility.

    //! Distribution of coefficients.
    std::uniform_real_distribution<double> coeff_dist_{0.5, 3.0};  // NOLINT

    //! Distribution of optimal variables.
    std::uniform_real_distribution<double> optimal_variable_dist_{
        min_variable, max_variable};
};

}  // namespace num_prob_collect::opt
