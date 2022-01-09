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
 * \brief Definition of newton_raphson class.
 */
#pragma once

#include <limits>
#include <type_traits>

#include <Eigen/LU>

#include "num_collect/logging/log_tag_view.h"
#include "num_collect/roots/function_root_finder_base.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/is_eigen_vector.h"

namespace num_collect::roots {

//! Tag of newton_raphson.
inline constexpr auto newton_raphson_tag =
    logging::log_tag_view("num_collect::roots::newton_raphson");

/*!
 * \brief Class of Newton-Raphson method.
 *
 * \tparam Function Type of the function of equation.
 */
template <typename Function, typename = void>
class newton_raphson;

/*!
 * \brief Class of Newton-Raphson method.
 *
 * This version is used when variable type is a floting-point number type.
 *
 * \tparam Function Type of the function of equation.
 */
template <typename Function>
class newton_raphson<Function,
    std::enable_if_t<
        std::is_floating_point_v<typename Function::variable_type>>>
    : public function_root_finder_base<newton_raphson<Function>, Function> {
public:
    //! Type of this object.
    using this_type = newton_raphson<Function>;

    //! Type of base class.
    using base_type =
        function_root_finder_base<newton_raphson<Function>, Function>;

    using typename base_type::function_type;
    using typename base_type::variable_type;

    static_assert(
        std::is_same_v<variable_type, typename function_type::jacobian_type>);

    /*!
     * \brief Construct.
     *
     * \param[in] function Function of equation.
     */
    explicit newton_raphson(const function_type& function = function_type())
        : base_type(newton_raphson_tag, function) {}

    /*!
     * \brief Initialize.
     *
     * \param[in] variable Initial variable.
     */
    void init(const variable_type& variable) {
        variable_ = variable;
        last_change_ = std::numeric_limits<variable_type>::infinity();
        iterations_ = 0;
        evaluations_ = 0;

        function().evaluate_on(variable_);
        ++evaluations_;
        using std::abs;
        value_norm_ = abs(function().value());
    }

    //! \copydoc function_root_finder_base::iterate
    void iterate() {
        change_ = -function().value() / function().jacobian();
        variable_ += change_;

        function().evaluate_on(variable_);
        ++evaluations_;
        ++iterations_;
        using std::abs;
        last_change_ = abs(change_);
        value_norm_ = abs(function().value());
    }

    //! \copydoc iterative_solver_base::is_stop_criteria_satisfied
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return (iterations() > max_iterations_) ||
            (last_change() < tol_last_change_) ||
            (value_norm() < tol_value_norm_);
    }

    //! \copydoc iterative_solver_base::configure_iteration_logger
    void configure_iteration_logger(
        logging::iteration_logger& iteration_logger) const {
        iteration_logger.append<index_type>(
            "Iter.", [this] { return iterations(); });
        iteration_logger.append<index_type>(
            "Eval.", [this] { return evaluations(); });
        iteration_logger.append<variable_type>(
            "Value", [this] { return value_norm(); });
        iteration_logger.append<variable_type>(
            "Change", [this] { return last_change(); });
    }

    using base_type::function;

    /*!
     * \brief Get current variable.
     *
     * \return Current variable.
     */
    [[nodiscard]] auto variable() const -> const variable_type& {
        return variable_;
    }

    /*!
     * \brief Get current value.
     *
     * \return Current value.
     */
    [[nodiscard]] auto value() const
        -> std::invoke_result_t<decltype(&function_type::value),
            const function_type> {
        return function().value();
    }

    /*!
     * \brief Get Jacobian matrix.
     *
     * \return Jacobian matrix.
     */
    [[nodiscard]] auto jacobian() const
        -> std::invoke_result_t<decltype(&function_type::jacobian),
            const function_type> {
        return function().jacobian();
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return iterations_;
    }

    /*!
     * \brief Get the number of function evaluations.
     *
     * \return Number of function evaluations.
     */
    [[nodiscard]] auto evaluations() const noexcept -> index_type {
        return evaluations_;
    }

    /*!
     * \brief Get the last change of the variable.
     *
     * \return Last change of the variable.
     */
    [[nodiscard]] auto last_change() const noexcept -> variable_type {
        return last_change_;
    }

    /*!
     * \brief Get the norm of function value.
     *
     * \return Norm of function value.
     */
    [[nodiscard]] auto value_norm() const noexcept -> variable_type {
        return value_norm_;
    }

    /*!
     * \brief Set maximum number of iterations.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_iterations(index_type val) -> this_type& {
        NUM_COLLECT_ASSERT(val > 0);
        max_iterations_ = val;
        return *this;
    }

    /*!
     * \brief Set tolerance of last change of the variable.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_last_change(const variable_type& val) -> this_type& {
        NUM_COLLECT_ASSERT(val >= static_cast<variable_type>(0));
        tol_last_change_ = val;
        return *this;
    }

    /*!
     * \brief Set tolerance of the norm of function value.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_value_norm(const variable_type& val) -> this_type& {
        NUM_COLLECT_ASSERT(val >= static_cast<variable_type>(0));
        tol_value_norm_ = val;
        return *this;
    }

private:
    //! Variable.
    variable_type variable_{};

    //! Change.
    variable_type change_{};

    //! Number of iterations.
    index_type iterations_{};

    //! Number of function evaluations.
    index_type evaluations_{};

    //! Last change of the variable.
    variable_type last_change_{};

    //! Norm of function value.
    variable_type value_norm_{};

    //! Default maximum iterations.
    static constexpr index_type default_max_iterations = 1000;

    //! Maximum iterations.
    index_type max_iterations_{default_max_iterations};

    //! Default tolerance of last change of the variable.
    static constexpr auto default_tol_last_change =
        static_cast<variable_type>(1e-6);

    //! Threshold of last change of the variable.
    variable_type tol_last_change_{default_tol_last_change};

    //! Default tolerance of the norm of function value.
    static constexpr auto default_tol_value_norm =
        static_cast<variable_type>(1e-6);

    //! Threshold of the norm of function value.
    variable_type tol_value_norm_{default_tol_value_norm};
};

/*!
 * \brief Class of Newton-Raphson method.
 *
 * This version is used when variable type is Eigen's vector.
 *
 * \tparam Function Type of the function of equation.
 */
template <typename Function>
class newton_raphson<Function,
    std::enable_if_t<is_eigen_vector_v<typename Function::variable_type>>>
    : public function_root_finder_base<newton_raphson<Function>, Function> {
public:
    //! Type of this object.
    using this_type = newton_raphson<Function>;

    //! Type of base class.
    using base_type =
        function_root_finder_base<newton_raphson<Function>, Function>;

    using typename base_type::function_type;
    using typename base_type::variable_type;

    //! Type of scalars in variables.
    using scalar_type = typename variable_type::Scalar;

    //! Type of Jacobian matrices.
    using jacobian_type = typename function_type::jacobian_type;

    //! Type of solvers of Jacobian matrices.
    using jacobian_solver_type =
        Eigen::PartialPivLU<typename Function::jacobian_type>;

    /*!
     * \brief Construct.
     *
     * \param[in] function Function of equation.
     */
    explicit newton_raphson(const function_type& function = function_type())
        : base_type(newton_raphson_tag, function) {}

    /*!
     * \brief Initialize.
     *
     * \param[in] variable Initial variable.
     */
    void init(const variable_type& variable) {
        variable_ = variable;
        last_change_ = std::numeric_limits<scalar_type>::infinity();
        iterations_ = 0;
        evaluations_ = 0;

        function().evaluate_on(variable_);
        ++evaluations_;
        value_norm_ = function().value().norm();
    }

    //! \copydoc function_root_finder_base::iterate
    void iterate() {
        jacobian_solver_.compute(function().jacobian());
        change_ = -jacobian_solver_.solve(function().value());
        variable_ += change_;

        function().evaluate_on(variable_);
        ++evaluations_;
        ++iterations_;
        last_change_ = change_.norm();
        value_norm_ = function().value().norm();
    }

    //! \copydoc function_root_finder_base::is_stop_criteria_satisfied
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return (iterations() > max_iterations_) ||
            (last_change() < tol_last_change_) ||
            (value_norm() < tol_value_norm_);
    }

    //! \copydoc iterative_solver_base::configure_iteration_logger
    void configure_iteration_logger(
        logging::iteration_logger& iteration_logger) const {
        iteration_logger.append<index_type>(
            "Iter.", [this] { return iterations(); });
        iteration_logger.append<index_type>(
            "Eval.", [this] { return evaluations(); });
        iteration_logger.append<scalar_type>(
            "Value", [this] { return value_norm(); });
        iteration_logger.append<scalar_type>(
            "Change", [this] { return last_change(); });
    }

    using base_type::function;

    /*!
     * \brief Get current variable.
     *
     * \return Current variable.
     */
    [[nodiscard]] auto variable() const -> const variable_type& {
        return variable_;
    }

    /*!
     * \brief Get current value.
     *
     * \return Current value.
     */
    [[nodiscard]] auto value() const
        -> std::invoke_result_t<decltype(&function_type::value),
            const function_type> {
        return function().value();
    }

    /*!
     * \brief Get Jacobian matrix.
     *
     * \return Jacobian matrix.
     */
    [[nodiscard]] auto jacobian() const
        -> std::invoke_result_t<decltype(&function_type::jacobian),
            const function_type> {
        return function().jacobian();
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Number of iterations.
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return iterations_;
    }

    /*!
     * \brief Get the number of function evaluations.
     *
     * \return Number of function evaluations.
     */
    [[nodiscard]] auto evaluations() const noexcept -> index_type {
        return evaluations_;
    }

    /*!
     * \brief Get the last change of the variable.
     *
     * \return Last change of the variable.
     */
    [[nodiscard]] auto last_change() const noexcept -> scalar_type {
        return last_change_;
    }

    /*!
     * \brief Get the norm of function value.
     *
     * \return Norm of function value.
     */
    [[nodiscard]] auto value_norm() const noexcept -> scalar_type {
        return value_norm_;
    }

    /*!
     * \brief Set maximum number of iterations.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto max_iterations(index_type val) -> this_type& {
        NUM_COLLECT_ASSERT(val > 0);
        max_iterations_ = val;
        return *this;
    }

    /*!
     * \brief Set tolerance of last change of the variable.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_last_change(const scalar_type& val) -> this_type& {
        NUM_COLLECT_ASSERT(val >= static_cast<scalar_type>(0));
        tol_last_change_ = val;
        return *this;
    }

    /*!
     * \brief Set tolerance of the norm of function value.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_value_norm(const scalar_type& val) -> this_type& {
        NUM_COLLECT_ASSERT(val >= static_cast<scalar_type>(0));
        tol_value_norm_ = val;
        return *this;
    }

private:
    //! Variable.
    variable_type variable_{};

    //! Change.
    variable_type change_{};

    //! Solver of Jacobian matrices.
    jacobian_solver_type jacobian_solver_{};

    //! Number of iterations.
    index_type iterations_{};

    //! Number of function evaluations.
    index_type evaluations_{};

    //! Last change of the variable.
    scalar_type last_change_{};

    //! Norm of function value.
    scalar_type value_norm_{};

    //! Default maximum iterations.
    static constexpr index_type default_max_iterations = 1000;

    //! Maximum iterations.
    index_type max_iterations_{default_max_iterations};

    //! Default tolerance of last change of the variable.
    static constexpr auto default_tol_last_change =
        static_cast<scalar_type>(1e-6);

    //! Threshold of last change of the variable.
    scalar_type tol_last_change_{default_tol_last_change};

    //! Default tolerance of the norm of function value.
    static constexpr auto default_tol_value_norm =
        static_cast<scalar_type>(1e-6);

    //! Threshold of the norm of function value.
    scalar_type tol_value_norm_{default_tol_value_norm};
};

}  // namespace num_collect::roots
