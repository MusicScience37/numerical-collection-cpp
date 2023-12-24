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
 * \brief Definition of random_multi_quadratic_function class.
 */
#pragma once

#include <random>

#include <Eigen/Core>

#include "num_collect/base/index_type.h"

namespace num_prob_collect::opt {

/*!
 * \brief Class of multi-variate quadratic functions with random coefficients
 * and optimal variables.
 */
class random_multi_quadratic_function {
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
     * \param[in] optimal_variable Optimal variable.
     * \param[in] coefficients Coefficients.
     */
    random_multi_quadratic_function(
        Eigen::VectorXd optimal_variable, Eigen::VectorXd coefficients)
        : optimal_variable_(std::move(optimal_variable)),
          coefficients_(std::move(coefficients)) {
        constexpr double two = 2.0;
        hessian_ = two * coefficients_.asDiagonal();
    }

    /*!
     * \brief Evaluate function value on variable.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(const Eigen::VectorXd& variable) noexcept {
        diff_ = variable - optimal_variable_;
        value_ = diff_.array().square().matrix().dot(coefficients_);
        constexpr double two = 2.0;
        grad_ = two * diff_.cwiseProduct(coefficients_);
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
     * \brief Get gradient.
     *
     * \return Gradient.
     */
    [[nodiscard]] auto gradient() const noexcept -> const Eigen::VectorXd& {
        return grad_;
    }

    /*!
     * \brief Get Hessian.
     *
     * \return Hessian.
     */
    [[nodiscard]] auto hessian() const noexcept -> const Eigen::MatrixXd& {
        return hessian_;
    }

    /*!
     * \brief Get the optimal variable.
     *
     * \return Optimal variable.
     */
    [[nodiscard]] auto optimal_variable() const noexcept
        -> const Eigen::VectorXd& {
        return optimal_variable_;
    }

    /*!
     * \brief Get the coefficients.
     *
     * \return Coefficients.
     */
    [[nodiscard]] auto coefficients() const noexcept -> const Eigen::VectorXd& {
        return coefficients_;
    }

private:
    //! Optimal variable.
    Eigen::VectorXd optimal_variable_;

    //! Coefficients.
    Eigen::VectorXd coefficients_;

    //! Buffer of difference.
    Eigen::VectorXd diff_{};

    //! Function value.
    double value_{0.0};

    //! Gradient.
    Eigen::VectorXd grad_{};

    //! Hessian.
    Eigen::MatrixXd hessian_{};
};

/*!
 * \brief Class to generate random random_multi_quadratic_function objects.
 */
class random_multi_quadratic_function_generator {
public:
    //! Minimum optimal variable.
    static constexpr double min_variable = -10.0;

    //! Maximum optimal variable.
    static constexpr double max_variable = 10.0;

    /*!
     * \brief Constructor.
     *
     * \param[in] num_variables Number of variables.
     */
    explicit random_multi_quadratic_function_generator(
        num_collect::index_type num_variables)
        : num_variables_(num_variables) {}

    /*!
     * \brief Generate a random quadratic function.
     *
     * \return Generated quadratic function.
     */
    [[nodiscard]] auto operator()() noexcept
        -> random_multi_quadratic_function {
        Eigen::VectorXd optimal_variable;
        optimal_variable.resize(num_variables_);
        std::generate(optimal_variable.begin(), optimal_variable.end(),
            [this] { return optimal_variable_dist_(generator_); });

        Eigen::VectorXd coefficients;
        coefficients.resize(num_variables_);
        std::generate(coefficients.begin(), coefficients.end(),
            [this] { return coeff_dist_(generator_); });

        return random_multi_quadratic_function{
            std::move(optimal_variable), std::move(coefficients)};
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

    //! Number of variables.
    num_collect::index_type num_variables_;
};

}  // namespace num_prob_collect::opt
