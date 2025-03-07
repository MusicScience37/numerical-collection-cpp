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
 * \brief Definition of multi_variate_multi_optima_function class.
 */
#pragma once

#include <algorithm>
#include <random>

#include <Eigen/Core>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"

namespace num_prob_collect::opt {

/*!
 * \brief Class of functions of multi-variate optimization problems with
 * multiple local optima.
 */
class multi_variate_multi_optima_function {
public:
    //! Type of variables.
    using variable_type = Eigen::VectorXd;

    //! Type of function values.
    using value_type = double;

    //! Type of Hessian.
    using hessian_type = Eigen::MatrixXd;

    /*!
     * \brief Constructor.
     *
     * \param[in] optimal_variables Optimal variables.
     * \param[in] optimal_values Optimal values.
     * \param[in] coefficients Coefficients for optima.
     */
    multi_variate_multi_optima_function(Eigen::ArrayXXd optimal_variables,
        Eigen::ArrayXd optimal_values, Eigen::ArrayXd coefficients)
        : optimal_variables_(std::move(optimal_variables)),
          optimal_values_(std::move(optimal_values)),
          coefficients_(std::move(coefficients)) {}

    /*!
     * \brief Evaluate function value on variable.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(const Eigen::VectorXd& variable) noexcept {
        value_ = ((optimal_variables_.rowwise() - variable.transpose().array())
                      .square()
                      .rowwise()
                      .sum() *
                coefficients_ +
            optimal_values_)
                     .minCoeff();
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
     * \brief Get the optimal variables.
     *
     * \return Optimal variables.
     */
    [[nodiscard]] auto optimal_variables() const noexcept
        -> const Eigen::ArrayXXd& {
        return optimal_variables_;
    }

    /*!
     * \brief Get the optimal values.
     *
     * \return Optimal values.
     */
    [[nodiscard]] auto optimal_values() const noexcept
        -> const Eigen::ArrayXd& {
        return optimal_values_;
    }

    /*!
     * \brief Get the coefficients for optima.
     *
     * \return Coefficients for optima.
     */
    [[nodiscard]] auto coefficients() const noexcept -> const Eigen::ArrayXd& {
        return coefficients_;
    }

private:
    //! Optimal variables.
    Eigen::ArrayXXd optimal_variables_;

    //! Optimal values.
    Eigen::ArrayXd optimal_values_;

    //! Coefficients for optima.
    Eigen::ArrayXd coefficients_;

    //! Function value.
    double value_{0.0};
};

/*!
 * \brief Class to generate random multi_variate_multi_optima_function objects.
 */
class random_multi_variate_multi_optima_function_generator {
public:
    //! Minimum optimal variable.
    static constexpr double min_variable = -10.0;

    //! Maximum optimal variable.
    static constexpr double max_variable = 10.0;

    //! Global optimal value.
    static constexpr double global_optimal_value = 0.0;

    //! Minimum non-global optimal value.
    static constexpr double min_non_global_optimal_value = 1.0;

    //! Maximum non-global optimal value.
    static constexpr double max_non_global_optimal_value = 5.0;

    //! Default value of the number of local optima.
    static constexpr num_collect::index_type default_num_local_optima = 5;

    /*!
     * \brief Constructor.
     *
     * \param[in] num_variables Number of variables.
     * \param[in] num_local_optima Number of local optima.
     */
    explicit random_multi_variate_multi_optima_function_generator(
        num_collect::index_type num_variables,
        num_collect::index_type num_local_optima = default_num_local_optima)
        : num_variables_(num_variables), num_local_optima_(num_local_optima) {}

    /*!
     * \brief Generate a random quadratic function.
     *
     * \return Generated quadratic function.
     */
    [[nodiscard]] auto operator()() noexcept
        -> multi_variate_multi_optima_function {
        Eigen::ArrayXXd optimal_variables;
        optimal_variables.resize(num_local_optima_, num_variables_);
        auto reshaped_optimal_variables = optimal_variables.reshaped();
        std::generate(reshaped_optimal_variables.begin(),
            reshaped_optimal_variables.end(),
            [this] { return optimal_variable_dist_(generator_); });

        Eigen::ArrayXd optimal_values;
        optimal_values.resize(num_local_optima_);
        optimal_values[0] = global_optimal_value;
        std::generate(optimal_values.begin() + 1, optimal_values.end(),
            [this] { return non_global_optimal_value_dist_(generator_); });

        Eigen::ArrayXd coefficients;
        coefficients.resize(num_local_optima_);
        coefficients[0] = 1.0;  // NOLINT
        std::generate(coefficients.begin() + 1, coefficients.end(),
            [this] { return coefficients_dist_(generator_); });

        return multi_variate_multi_optima_function{std::move(optimal_variables),
            std::move(optimal_values), std::move(coefficients)};
    }

private:
    //! Random number generator.
    std::mt19937
        generator_{};  // NOLINT(cert-msc32-c,cert-msc51-cpp): For reproducibility.

    //! Distribution of optimal variables.
    std::uniform_real_distribution<double> optimal_variable_dist_{
        min_variable, max_variable};

    //! Distribution of non-global optimal values.
    std::uniform_real_distribution<double> non_global_optimal_value_dist_{
        min_non_global_optimal_value, max_non_global_optimal_value};

    //! Distribution of coefficients.
    std::uniform_real_distribution<double> coefficients_dist_{
        0.5, 3.0};  // NOLINT

    //! Number of variables.
    num_collect::index_type num_variables_;

    //! Number of local minima.
    num_collect::index_type num_local_optima_;
};

/*!
 * \brief Class to generate difficult version of random
 * multi_variate_multi_optima_function objects.
 *
 * \note Adaptive diagonal curves (ADC) method is good at solving this type of
 * problems \cite Sergeyev2006.
 */
class random_multi_variate_difficult_multi_optima_function_generator {
public:
    //! Minimum optimal variable.
    static constexpr double min_variable = -10.0;

    //! Maximum optimal variable.
    static constexpr double max_variable = 10.0;

    //! Global optimal value.
    static constexpr double global_optimal_value = 0.0;

    //! Minimum non-global optimal value.
    static constexpr double min_non_global_optimal_value = 1.0;

    //! Maximum non-global optimal value.
    static constexpr double max_non_global_optimal_value = 5.0;

    //! Default value of the number of local optima.
    static constexpr num_collect::index_type default_num_local_optima = 5;

    /*!
     * \brief Constructor.
     *
     * \param[in] num_variables Number of variables.
     * \param[in] num_local_optima Number of local optima.
     */
    explicit random_multi_variate_difficult_multi_optima_function_generator(
        num_collect::index_type num_variables,
        num_collect::index_type num_local_optima = default_num_local_optima)
        : num_variables_(num_variables), num_local_optima_(num_local_optima) {}

    /*!
     * \brief Generate a random quadratic function.
     *
     * \return Generated quadratic function.
     */
    [[nodiscard]] auto operator()() noexcept
        -> multi_variate_multi_optima_function {
        Eigen::ArrayXXd optimal_variables;
        optimal_variables.resize(num_local_optima_, num_variables_);
        auto reshaped_optimal_variables = optimal_variables.reshaped();
        std::generate(reshaped_optimal_variables.begin(),
            reshaped_optimal_variables.end(),
            [this] { return optimal_variable_dist_(generator_); });

        Eigen::ArrayXd optimal_values;
        optimal_values.resize(num_local_optima_);
        optimal_values[0] = global_optimal_value;
        std::generate(optimal_values.begin() + 1, optimal_values.end(),
            [this] { return non_global_optimal_value_dist_(generator_); });

        Eigen::ArrayXd coefficients;
        coefficients.resize(num_local_optima_);
        coefficients[0] = 10.0;  // NOLINT
        std::generate(coefficients.begin() + 1, coefficients.end(),
            [this] { return coefficients_dist_(generator_); });

        return multi_variate_multi_optima_function{std::move(optimal_variables),
            std::move(optimal_values), std::move(coefficients)};
    }

private:
    //! Random number generator.
    std::mt19937
        generator_{};  // NOLINT(cert-msc32-c,cert-msc51-cpp): For reproducibility.

    //! Distribution of optimal variables.
    std::uniform_real_distribution<double> optimal_variable_dist_{
        min_variable, max_variable};

    //! Distribution of non-global optimal values.
    std::uniform_real_distribution<double> non_global_optimal_value_dist_{
        min_non_global_optimal_value, max_non_global_optimal_value};

    //! Distribution of coefficients.
    std::uniform_real_distribution<double> coefficients_dist_{
        0.5, 3.0};  // NOLINT

    //! Number of variables.
    num_collect::index_type num_variables_;

    //! Number of local minima.
    num_collect::index_type num_local_optima_;
};

}  // namespace num_prob_collect::opt
