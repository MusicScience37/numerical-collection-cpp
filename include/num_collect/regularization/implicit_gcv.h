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
 * \brief Definition of implicit_gcv class.
 */
#pragma once

#include <cmath>
#include <concepts>
#include <random>

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/opt/any_objective_function.h"
#include "num_collect/opt/gaussian_process_optimizer.h"
#include "num_collect/regularization/concepts/regularized_solver.h"
#include "num_collect/util/vector.h"

namespace num_collect::regularization {

//! Tag of implicit_gcv.
constexpr auto implicit_gcv_tag =
    logging::log_tag_view("num_collect::regularization::implicit_gcv");

/*!
 * \brief Class to calculate the objective function of GCV.
 *
 * \tparam Solver Type of solvers.
 */
template <concepts::regularized_solver Solver>
class implicit_gcv_calculator {
public:
    //! Type of solvers.
    using solver_type = Solver;

    //! Type of scalars.
    using scalar_type = typename solver_type::scalar_type;

    //! Type of data.
    using data_type = typename solver_type::data_type;

    static_assert(base::concepts::real_scalar_dense_vector<data_type>,
        "Currently only vectors are supported as data.");

    /*!
     * \brief Constructor.
     *
     * \param[in] solver Solver.
     * \param[in] data Data.
     * \param[in] initial_solution Initial solution.
     */
    implicit_gcv_calculator(solver_type& solver, const data_type& data,
        const data_type& initial_solution)
        : solver_(&solver), data_(&data), initial_solution_(&initial_solution) {
        noise_rate(default_noise_rate);
        num_samples(1);
    }

    /*!
     * \brief Calculate GCV function.
     *
     * \param[in] param Regularization parameter.
     * \return Value.
     */
    [[nodiscard]] auto operator()(scalar_type param) -> scalar_type {
        const index_type data_size = data_->size();
        if (noise_.back().size() != data_size) {
            generate_noise();
        }

        solution_ = *initial_solution_;
        solver_->change_data(*data_);
        solver_->solve(param, solution_);
        solver_->calculate_data_for(solution_, forwarded_data_);

        auto trace_sum = static_cast<scalar_type>(0);
        for (index_type i = 0; i < noise_.size(); ++i) {
            solution_with_noise_ = *initial_solution_;
            solver_->change_data(data_with_noise_[i]);
            solver_->solve(param, solution_with_noise_);
            solver_->calculate_data_for(
                solution_with_noise_, forwarded_data_with_noise_);

            trace_sum += noise_[i].dot(noise_[i] -
                             (forwarded_data_with_noise_ - forwarded_data_) /
                                 noise_multiplier_) /
                noise_[i].squaredNorm();
        }
        trace_sum /= static_cast<scalar_type>(noise_.size());

        solver_->change_data(*data_);

        const scalar_type denominator = trace_sum * trace_sum;
        const scalar_type numerator = solver_->residual_norm(solution_) /
            static_cast<scalar_type>(data_size);
        return numerator / denominator;
    }

    /*!
     * \brief Set the rate of noise.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto noise_rate(scalar_type value) -> implicit_gcv_calculator& {
        NUM_COLLECT_PRECONDITION(value > static_cast<scalar_type>(0),
            "Rate of noise must be a positive value.");
        noise_rate_ = value;
        using std::sqrt;
        noise_multiplier_ = data_->norm() *
            sqrt(noise_rate_ / static_cast<scalar_type>(data_->size()));
        return *this;
    }

    /*!
     * \brief Set the number of samples for approximation of denominator of GCV.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto num_samples(index_type value) -> implicit_gcv_calculator& {
        NUM_COLLECT_PRECONDITION(
            value > 0, "Number of samples must be a positive value.");
        noise_.resize(value);
        data_with_noise_.resize(value);
        return *this;
    }

    /*!
     * \brief Generate noise to use in calculation.
     *
     * \tparam RandomNumberGenerator Type of the generator of random numbers.
     * \param[in] generator Generator of random numbers.
     */
    template <std::invocable<> RandomNumberGenerator>
    void generate_noise(RandomNumberGenerator& generator) {
        std::normal_distribution<scalar_type> distribution;
        for (index_type i = 0; i < noise_.size(); ++i) {
            noise_[i].resize(data_->size());
            for (index_type j = 0; j < data_->size(); ++j) {
                noise_[i](j) = distribution(generator);
            }
            data_with_noise_[i] = (*data_) + noise_multiplier_ * noise_[i];
        }
    }

    /*!
     * \brief Generate noise with a random seed.
     */
    void generate_noise() {
        std::mt19937 generator{std::random_device{}()};
        generate_noise(generator);
    }

    /*!
     * \brief Get the solver.
     *
     * \return Solver.
     */
    [[nodiscard]] auto solver() const noexcept -> solver_type& {
        return *solver_;
    }

private:
    //! Solver.
    solver_type* solver_;

    //! Actual data.
    const data_type* data_;

    //! Vector of noise.
    util::vector<data_type> noise_;

    //! Default rate of noise.
    static constexpr scalar_type default_noise_rate = 1e-2;

    //! Rate of noise.
    scalar_type noise_rate_{default_noise_rate};

    //! Multiplier of noise.
    scalar_type noise_multiplier_{};

    //! Data with noise.
    util::vector<data_type> data_with_noise_{};

    //! Initial solution.
    const data_type* initial_solution_;

    //! Buffer of the solution.
    data_type solution_{};

    //! Buffer of the solution with noise.
    data_type solution_with_noise_{};

    //! Buffer of data generated from the solution.
    data_type forwarded_data_{};

    //! Buffer of data generated from the solution with noise.
    data_type forwarded_data_with_noise_{};
};

/*!
 * \brief Class to search optimal regularization parameter using GCV.
 *
 * \tparam Solver Type of solvers.
 */
template <concepts::regularized_solver Solver>
class implicit_gcv : public logging::logging_mixin {
public:
    //! Type of solvers.
    using solver_type = Solver;

    //! Type of scalars.
    using scalar_type = typename solver_type::scalar_type;

    //! Type of data.
    using data_type = typename solver_type::data_type;

    /*!
     * \brief Constructor.
     *
     * \param[in] solver Solver.
     * \param[in] data Data.
     * \param[in] initial_solution Initial solution.
     */
    implicit_gcv(solver_type& solver, const data_type& data,
        const data_type& initial_solution)
        : logging::logging_mixin(implicit_gcv_tag),
          calculator_(solver, data, initial_solution) {
        this->configure_child_algorithm_logger_if_exists(optimizer_);
        optimizer_.configure_child_algorithm_logger_if_exists(
            calculator_.solver());
        constexpr index_type max_evaluations = 10;
        optimizer_.max_evaluations(max_evaluations);
    }

    //! \copydoc explicit_param_searcher_base::search
    void search() {
        using std::log10;
        using std::pow;
        const auto [min_param, max_param] =
            calculator_.solver().param_search_region();
        NUM_COLLECT_LOG_DEBUG(
            logger(), "Region of parameters: [{}, {}]", min_param, max_param);
        const scalar_type log_min_param = log10(min_param);
        const scalar_type log_max_param = log10(max_param);

        const auto objective_function =
            [this](scalar_type log_param) -> scalar_type {
            using std::pow;
            using std::log10;
            const scalar_type param =
                pow(static_cast<scalar_type>(10),  // NOLINT
                    log_param);
            const scalar_type gcv_value = calculator_(param);
            NUM_COLLECT_LOG_TRACE(logger(), "gcv({}) = {}", param, gcv_value);
            return log10(gcv_value);
        };
        optimizer_.change_objective_function(objective_function);

        this->configure_child_algorithm_logger_if_exists(optimizer_);
        optimizer_.configure_child_algorithm_logger_if_exists(
            calculator_.solver());

        optimizer_.init(log_min_param, log_max_param);
        optimizer_.solve();
        opt_param_ = pow(static_cast<scalar_type>(10),  // NOLINT
            optimizer_.opt_variable());

        NUM_COLLECT_LOG_SUMMARY(logger(), "Selected parameter: {}", opt_param_);
    }

    //! \copydoc explicit_param_searcher_base::opt_param
    [[nodiscard]] auto opt_param() const -> scalar_type { return opt_param_; }

    //! \copydoc explicit_param_searcher_base::solve
    void solve(data_type& solution) const {
        NUM_COLLECT_LOG_DEBUG(
            logger(), "Solve with an optimal parameter: {}", opt_param_);
        calculator_.solver().solve(opt_param_, solution);
    }

    /*!
     * \brief Set the rate of noise.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto noise_rate(scalar_type value) -> implicit_gcv& {
        calculator_.noise_rate(value);
        return *this;
    }

    /*!
     * \brief Set the number of samples for approximation of denominator of GCV.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto num_samples(index_type value) -> implicit_gcv& {
        calculator_.num_samples(value);
        return *this;
    }

    /*!
     * \brief Set the maximum number of evaluations of GCV.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto max_evaluations(index_type value) -> implicit_gcv& {
        optimizer_.max_evaluations(value);
        return *this;
    }

private:
    //! Calculator of GCV.
    implicit_gcv_calculator<solver_type> calculator_;

    //! Optimizer.
    opt::gaussian_process_optimizer<
        opt::any_objective_function<scalar_type(scalar_type)>>
        optimizer_{};

    //! Optimal regularization parameter.
    scalar_type opt_param_{};
};

}  // namespace num_collect::regularization
