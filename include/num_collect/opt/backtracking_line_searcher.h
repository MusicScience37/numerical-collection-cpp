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
 * \brief Definition of backtracking_line_searcher class.
 */
#pragma once

#include <Eigen/Core>
#include <cstddef>
#include <type_traits>

#include "num_collect/util/index_type.h"

namespace num_collect::opt {

/*!
 * \brief Class to perform backtracking line search.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <typename ObjectiveFunction, typename = void>
class backtracking_line_searcher;

/*!
 * \brief Class to perform backtracking line search.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <typename ObjectiveFunction>
class backtracking_line_searcher<ObjectiveFunction,
    std::enable_if_t<std::is_base_of_v<
        Eigen::MatrixBase<typename ObjectiveFunction::variable_type>,
        typename ObjectiveFunction::variable_type>>> {
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
    explicit backtracking_line_searcher(
        const objective_function_type& obj_fun = objective_function_type())
        : obj_fun_(obj_fun) {}

    /*!
     * \brief Initialize.
     *
     * \param[in] init_variable Initial variable.
     */
    void init(const variable_type& init_variable) {
        variable_ = init_variable;
        obj_fun_.evaluate_on(variable_);
        evaluations_ = 1;
    }

    /*!
     * \brief Search step size.
     *
     * \param[in] direction Direction to update the variable.
     */
    void search(const variable_type& direction) {
        step_ = init_step;

        const variable_type prev_var = variable_;
        const value_type prev_value = obj_fun_.value();
        const value_type step_coeff =
            armijo_coeff_ * obj_fun_.gradient().dot(direction);

        // prevent infinite loops
        constexpr index_type max_loops = 1000;
        for (index_type i = 0; i < max_loops; ++i) {
            variable_ = prev_var + step_ * direction;
            obj_fun_.evaluate_on(variable_);
            ++evaluations_;
            if (obj_fun_.value() <= prev_value + step_ * step_coeff) {
                return;
            }
            step_ *= step_scale_;
        }

        throw std::runtime_error("failed to search step size");
    }

    /*!
     * \brief Access objective function.
     *
     * \return Reference to objective function.
     */
    [[nodiscard]] auto obj_fun() -> objective_function_type& {
        return obj_fun_;
    }

    /*!
     * \brief Access objective function.
     *
     * \return Reference to objective function.
     */
    [[nodiscard]] auto obj_fun() const -> const objective_function_type& {
        return obj_fun_;
    }

    /*!
     * \brief Get current optimal variable.
     *
     * \return Current optimal variable.
     */
    [[nodiscard]] auto opt_variable() const -> const variable_type& {
        return variable_;
    }

    /*!
     * \brief Get current optimal value.
     *
     * \return Current optimal value.
     */
    [[nodiscard]] auto opt_value() const
        -> std::invoke_result_t<decltype(&objective_function_type::value),
            const objective_function_type> {
        return obj_fun_.value();
    }

    /*!
     * \brief Get gradient for current optimal variable.
     *
     * \return Gradient for current optimal variable.
     */
    [[nodiscard]] auto gradient() const
        -> std::invoke_result_t<decltype(&objective_function_type::gradient),
            const objective_function_type> {
        return obj_fun_.gradient();
    }

    /*!
     * \brief Get the number of function evaluations.
     *
     * \return Number of function evaluations.
     */
    [[nodiscard]] auto evaluations() const -> index_type {
        return evaluations_;
    }

    /*!
     * \brief Get last step size.
     *
     * \return Last step size.
     */
    [[nodiscard]] auto last_step() const -> value_type { return step_; }

    /*!
     * \brief Set the coefficient in Armijo rule.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto armijo_coeff(value_type value) -> backtracking_line_searcher& {
        if (armijo_coeff_ <= value_type(0) || value_type(1) <= armijo_coeff_) {
            throw std::invalid_argument(
                "armijo_coeff must be in the range (0, 1)");
        }
        armijo_coeff_ = value;
        return *this;
    }

    /*!
     * \brief Set the ration to scale step sizes.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto step_scale(value_type value) -> backtracking_line_searcher& {
        if (armijo_coeff_ <= value_type(0) || value_type(1) <= armijo_coeff_) {
            throw std::invalid_argument(
                "step_scale must be in the range (0, 1)");
        }
        step_scale_ = value;
        return *this;
    }

private:
    //! Objective function.
    objective_function_type obj_fun_;

    //! Current variable.
    variable_type variable_{};

    //! Initial step size.
    static inline const auto init_step = static_cast<value_type>(1.0);

    //! Step size.
    value_type step_{init_step};

    //! Default coefficient in Armijo rule.
    static inline const auto default_armijo_coeff =
        static_cast<value_type>(0.1);

    //! Coefficient in Armijo rule.
    value_type armijo_coeff_{default_armijo_coeff};

    //! Default ratio to scale step sizes.
    static inline const auto default_step_scale = static_cast<value_type>(0.5);

    //! Ration to scale step sizes.
    value_type step_scale_{default_step_scale};

    //! Number of function evaluations.
    index_type evaluations_{0};
};

}  // namespace num_collect::opt
