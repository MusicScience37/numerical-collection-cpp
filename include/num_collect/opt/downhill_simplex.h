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
 * \brief Definition of downhill_simplex class.
 */
#pragma once

#include <algorithm>
#include <string>
#include <type_traits>
#include <vector>

#include <Eigen/Core>

#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/optimizer_base.h"
#include "num_collect/util/safe_cast.h"

namespace num_collect::opt {

//! Tag of downhill_simplex.
inline constexpr auto downhill_simplex_tag =
    logging::log_tag_view("num_collect::opt::downhill_simplex");

/*!
 * \brief Class of downhill simplex method.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <typename ObjectiveFunction, typename = void>
class downhill_simplex;

/*!
 * \brief Class of downhill simplex method.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <typename ObjectiveFunction>
class downhill_simplex<ObjectiveFunction,
    std::enable_if_t<std::is_base_of_v<
        Eigen::MatrixBase<typename ObjectiveFunction::variable_type>,
        typename ObjectiveFunction::variable_type>>>
    : public optimizer_base<downhill_simplex<ObjectiveFunction>> {
public:
    //! Type of the objective function.
    using objective_function_type = ObjectiveFunction;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of scalars in variables.
    using variable_scalar_type = typename variable_type::Scalar;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

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
        -> std::string {
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
     * \brief Construct.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit downhill_simplex(
        const objective_function_type& obj_fun = objective_function_type())
        : optimizer_base<downhill_simplex<ObjectiveFunction>>(
              downhill_simplex_tag),
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

        variable_type var = init_var;
        points_.reserve(safe_cast<std::size_t>(dim_ + 1));
        values_.reserve(safe_cast<std::size_t>(dim_ + 1));
        points_.push_back(var);
        values_.push_back(evaluate_on(var));
        for (index_type i = 0; i < dim_; ++i) {
            const variable_scalar_type val = var[i];
            var[i] += width;
            points_.push_back(var);
            values_.push_back(evaluate_on(var));
            var[i] = val;
        }

        value_order_.reserve(safe_cast<std::size_t>(dim_ + 1));
        for (std::size_t i = 0; i < safe_cast<std::size_t>(dim_ + 1); ++i) {
            value_order_.push_back(i);
        }
        reorder();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() {
        const variable_type face_center = calc_face_center();
        const auto min_ind = value_order_.front();
        const auto second_max_ind =
            value_order_[safe_cast<std::size_t>(dim_ - 1)];
        const auto max_ind = value_order_.back();

        reflect(face_center);
        if (values_[max_ind] < values_[min_ind]) {
            expand(face_center);
        } else if (values_[max_ind] >= values_[second_max_ind]) {
            contract(face_center);
            if (values_[max_ind] >= values_[second_max_ind]) {
                multi_contract();
            }
        } else {
            // no operation
        }

        reorder();
        ++iterations_;
    }

    /*!
     * \copydoc num_collect::iterative_solver_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return (simplex_size() < tol_simplex_size_) ||
            (iterations() >= max_iterations_);
    }

    /*!
     * \copydoc num_collect::iterative_solver_base::configure_iteration_logger
     */
    void configure_iteration_logger(
        logging::iteration_logger& iteration_logger) const {
        iteration_logger.append<index_type>(
            "Iter.", [this] { return iterations(); });
        iteration_logger.append<index_type>(
            "Eval.", [this] { return evaluations(); });
        iteration_logger.append<value_type>(
            "Value", [this] { return opt_value(); });
        iteration_logger.append<variable_scalar_type>(
            "SimplexSize", [this] { return simplex_size(); });
        constexpr index_type process_width = 26;
        iteration_logger
            .append<std::string>(
                "Process", [this] { return process_name(last_process()); })
            ->width(process_width);
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_variable
     */
    [[nodiscard]] auto opt_variable() const -> const variable_type& {
        return points_[value_order_.front()];
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_value
     */
    [[nodiscard]] auto opt_value() const -> const value_type& {
        return values_[value_order_.front()];
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
     * \brief Get the size of simplex.
     *
     * \return Size of simplex.
     */
    [[nodiscard]] auto simplex_size() const -> variable_scalar_type {
        return (points_[value_order_.front()] - points_[value_order_.back()])
            .norm();
    }

    //! Default width of simplex.
    static inline const auto default_width =
        static_cast<variable_scalar_type>(0.1);

    /*!
     * \brief Set tolerance of size of simplex.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto tol_simplex_size(const variable_scalar_type& value)
        -> downhill_simplex& {
        tol_simplex_size_ = value;
        return *this;
    }

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
        return obj_fun_.value();
    }

    /*!
     * \brief Reorder function values.
     */
    void reorder() {
        std::sort(std::begin(value_order_), std::end(value_order_),
            [this](std::size_t i, std::size_t j) {
                return values_[i] < values_[j];
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
        for (std::size_t i = 0; i < safe_cast<std::size_t>(dim_); ++i) {
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
        process_ = process_type::contraction;
    }

    /*!
     * \brief Contract all points other than the lowest point toward the lowest
     * point.
     */
    void multi_contract() {
        const auto& min_point = points_[value_order_.front()];
        for (std::size_t i = 1; i <= safe_cast<std::size_t>(dim_); ++i) {
            const std::size_t ind_move = value_order_[i];
            points_[ind_move] = half * (points_[ind_move] + min_point);
            values_[ind_move] = evaluate_on(points_[ind_move]);
        }
        process_ = process_type::multiple_contraction;
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

    //! Order of function values (ascending order).
    std::vector<std::size_t> value_order_{};

    //! Last process.
    process_type process_{process_type::none};

    //! Number of iterations.
    index_type iterations_{0};

    //! Number of function evaluations.
    index_type evaluations_{0};

    //! Default tolerance of size of simplex.
    static inline const auto default_tol_simplex_size =
        static_cast<variable_scalar_type>(1e-4);

    //! Tolerance of size of simplex.
    variable_scalar_type tol_simplex_size_{default_tol_simplex_size};

    //! Default maximum number of iterations.
    static constexpr index_type default_max_iterations = 1000;

    //! Maximum number of iterations.
    index_type max_iterations_{default_max_iterations};
};

}  // namespace num_collect::opt
