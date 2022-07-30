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
 * \brief Definition of sampling_optimizer class.
 */
#pragma once

#include <cstddef>
#include <limits>
#include <string_view>
#include <vector>

#include "num_collect/base/index_type.h"
#include "num_collect/logging/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/concepts/objective_function.h"  // IWYU pragma: keep
#include "num_collect/opt/concepts/single_variate_objective_function.h"  // IWYU pragma: keep
#include "num_collect/opt/optimizer_base.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/safe_cast.h"

namespace num_collect::opt {

//! Tag of golden_section_search.
inline constexpr auto sampling_optimizer_tag =
    logging::log_tag_view("num_collect::opt::sampling_optimizer");

/*!
 * \brief Class to perform optimization using samples of objective functions.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <concepts::objective_function ObjectiveFunction>
class sampling_optimizer;

/*!
 * \brief Class to perform optimization using samples of objective functions.
 *
 * \tparam ObjectiveFunction Type of the objective function.
 */
template <concepts::single_variate_objective_function ObjectiveFunction>
class sampling_optimizer<ObjectiveFunction>
    : public optimizer_base<sampling_optimizer<ObjectiveFunction>> {
public:
    //! Type of the objective function.
    using objective_function_type = ObjectiveFunction;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

    /*!
     * \brief Construct.
     *
     * \param[in] obj_fun Objective function.
     */
    explicit sampling_optimizer(
        const objective_function_type& obj_fun = objective_function_type())
        : optimizer_base<sampling_optimizer<ObjectiveFunction>>(
              sampling_optimizer_tag),
          obj_fun_(obj_fun) {}

    /*!
     * \brief Initialize the algorithm.
     *
     * \param[in] lower Lower limit.
     * \param[in] upper Upper limit.
     */
    void init(const variable_type& lower, const variable_type& upper) {
        lower_ = lower;
        upper_ = upper;
        iterations_ = 0;
        evaluations_ = 0;

        // Without this, solve() causes segmentation fault.
        iterate();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() {
        const auto num_samples_size_t =
            util::safe_cast<std::size_t>(num_samples_);
        samples_.resize(num_samples_size_t);
        values_.resize(num_samples_size_t);

        value_type min_val = std::numeric_limits<value_type>::max();

        for (std::size_t i = 0; i < num_samples_size_t; ++i) {
            const variable_type rate = static_cast<variable_type>(i) /
                static_cast<variable_type>(num_samples_ - 1);
            const variable_type variable = lower_ + (upper_ - lower_) * rate;

            obj_fun_.evaluate_on(variable);
            const value_type value = obj_fun_.value();

            samples_[i] = variable;
            values_[i] = value;

            if (value < min_val) {
                min_val = value;
                ind_opt_sample_ = i;
            }

            ++evaluations_;
        }

        if (ind_opt_sample_ > 0) {
            lower_ = samples_[ind_opt_sample_ - 1];
        }
        if (ind_opt_sample_ < num_samples_size_t - 1) {
            upper_ = samples_[ind_opt_sample_ + 1];
        }

        ++iterations_;
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return iterations_ >= max_iterations_;
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::configure_iteration_logger
     */
    void configure_iteration_logger(
        logging::iteration_logger& iteration_logger) const {
        iteration_logger.append<index_type>(
            "Iter.", [this] { return iterations(); });
        iteration_logger.append<index_type>(
            "Eval.", [this] { return evaluations(); });
        iteration_logger.append<value_type>(
            "Value", [this] { return opt_value(); });
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_variable
     */
    [[nodiscard]] auto opt_variable() const -> const variable_type& {
        return samples_[ind_opt_sample_];
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_value
     */
    [[nodiscard]] auto opt_value() const -> const value_type& {
        return values_[ind_opt_sample_];
    }

    /*!
     * \brief Get the current upper limit.
     *
     * \return Upper limit.
     */
    [[nodiscard]] auto upper() const -> const variable_type& { return upper_; }

    /*!
     * \brief Get the current lower limit.
     *
     * \return Lower limit.
     */
    [[nodiscard]] auto lower() const -> const variable_type& { return lower_; }

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
     * \brief Set the number of samples.
     *
     * \param[in] value Value.
     * \return This.
     */
    auto num_samples(index_type value) -> sampling_optimizer& {
        NUM_COLLECT_ASSERT(value >= 2);
        num_samples_ = value;
        return *this;
    }

    /*!
     * \brief Set the maximum number of iterations.
     *
     * \param[in] value Value.
     * \return This.
     */
    auto max_iterations(index_type value) -> sampling_optimizer& {
        NUM_COLLECT_ASSERT(value > 0);
        max_iterations_ = value;
        return *this;
    }

private:
    //! Objective function.
    objective_function_type obj_fun_;

    //! Lower limit.
    variable_type lower_{static_cast<variable_type>(0)};

    //! Upper limit.
    variable_type upper_{static_cast<variable_type>(1)};

    //! List of sampling points.
    std::vector<variable_type> samples_{};

    //! List of function values.
    std::vector<value_type> values_{};

    //! Index of the minimum sampling point.
    std::size_t ind_opt_sample_{0};

    //! Default number of sampling points.
    static constexpr index_type default_num_samples = 21;

    //! Number of sampling points.
    index_type num_samples_{default_num_samples};

    //! Default maximum number of iterations.
    static constexpr index_type default_max_iterations = 3;

    //! Maximum number of iterations.
    index_type max_iterations_{default_max_iterations};

    //! Number of iterations.
    index_type iterations_{0};

    //! Number of function evaluations.
    index_type evaluations_{0};
};

}  // namespace num_collect::opt
