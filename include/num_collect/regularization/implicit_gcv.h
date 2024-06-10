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
#include <random>

#include "num_collect/base/concepts/invocable.h"
#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/regularization/concepts/implicit_regularized_solver.h"

namespace num_collect::regularization {

/*!
 * \brief Class to calculate the objective function of GCV.
 *
 * \tparam Solver Type of solvers.
 */
template <concepts::implicit_regularized_solver Solver>
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
    }

    /*!
     * \brief Calculate GCV function.
     *
     * \param[in] param Regularization parameter.
     * \return Value.
     */
    [[nodiscard]] auto operator()(scalar_type param) -> scalar_type {
        const index_type data_size = data_->size();
        if (noise_.size() != data_size) {
            generate_noise();
        }

        solution_with_noise_ = *initial_solution_;
        solver_->change_data(data_with_noise_);
        solver_->solve(param, solution_with_noise_);
        solver_->calculate_data_for(
            solution_with_noise_, forwarded_data_with_noise_);

        solution_ = *initial_solution_;
        solver_->change_data(*data_);
        solver_->solve(param, solution_);
        solver_->calculate_data_for(solution_, forwarded_data_);

        const scalar_type sqrt_denominator =
            noise_.dot(noise_ -
                (forwarded_data_with_noise_ - forwarded_data_) /
                    noise_multiplier_) /
            noise_.squaredNorm();
        const scalar_type denominator = sqrt_denominator * sqrt_denominator;

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
        if (value <= static_cast<scalar_type>(0)) {
            throw invalid_argument("Rate of noise must be a positive value.");
        }
        noise_rate_ = value;
        using std::sqrt;
        noise_multiplier_ = data_->norm() *
            sqrt(noise_rate_ / static_cast<scalar_type>(data_->size()));
        return *this;
    }

    /*!
     * \brief Generate noise to use in calculation.
     *
     * \tparam RandomNumberGenerator Type of the generator of random numbers.
     * \param[in] generator Generator of random numbers.
     */
    template <base::concepts::invocable<> RandomNumberGenerator>
    void generate_noise(RandomNumberGenerator& generator) {
        std::normal_distribution<scalar_type> distribution;
        noise_.resize(data_->size());
        for (index_type i = 0; i < data_->size(); ++i) {
            noise_(i) = distribution(generator);
        }
        data_with_noise_ = (*data_) + noise_multiplier_ * noise_;
    }

    /*!
     * \brief Generate noise with a random seed.
     */
    void generate_noise() {
        std::mt19937 generator{std::random_device{}()};
        generate_noise(generator);
    }

private:
    //! Solver.
    solver_type* solver_;

    //! Actual data.
    const data_type* data_;

    //! Vector of noise.
    data_type noise_{};

    //! Default rate of noise.
    static constexpr scalar_type default_noise_rate = 1e-2;

    //! Rate of noise.
    scalar_type noise_rate_{default_noise_rate};

    //! Multiplier of noise.
    scalar_type noise_multiplier_{};

    //! Data with noise.
    data_type data_with_noise_{};

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

}  // namespace num_collect::regularization
