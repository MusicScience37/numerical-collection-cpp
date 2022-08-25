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
 * \brief Definition of string_wave_1d_problem class.
 */
#pragma once

#include <cmath>

#include <Eigen/Core>

#include "num_collect/base/index_type.h"
#include "num_collect/constants/pi.h"
#include "num_collect/ode/evaluation_type.h"
#include "num_collect/util/assert.h"

namespace num_prob_collect::ode {

/*!
 * \brief Struct of parameters in string_wave_1d_problem.
 */
struct string_wave_1d_parameters {
public:
    //! Speed of the wave.
    double speed{1.0};

    //! Number of spatial points.
    num_collect::index_type num_points{101};  // NOLINT

    //! Length of the space.
    double length{1.0};
};

/*!
 * \brief Class of ODE problem to solve 1D wave equation of strings discretized
 * using finite difference.
 *
 * \note The lower half of variables are displacements and the upper half are
 * velocities.
 */
class string_wave_1d_problem {
public:
    //! Type of variables.
    using variable_type = Eigen::VectorXd;

    //! Type of scalars.
    using scalar_type = double;

    //! Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{.diff_coeff = true};

    /*!
     * \brief Constructor.
     *
     * \param[in] params Parameters.
     */
    explicit string_wave_1d_problem(const string_wave_1d_parameters& params)
        : speed_(params.speed), num_points_(params.num_points) {
        NUM_COLLECT_ASSERT(params.num_points >= 3);
        points_ = variable_type::LinSpaced(num_points_, 0.0, params.length);
        diff_coeff_ = variable_type::Zero(num_points_ * 2);
    }

    /*!
     * \brief Evaluate on a (time, variable) pair.
     *
     * \param[in] variable Variable.
     */
    void evaluate_on(scalar_type /*time*/, const variable_type& variable,
        num_collect::ode::evaluation_type /*evaluations*/) {
        const double dx = points_(1) - points_(0);
        // acceleration.
        diff_coeff_.segment(1, num_points_ - 2) = speed_ * speed_ / (dx * dx) *
            (variable.segment(num_points_ + 2, num_points_ - 2) -
                2.0  // NOLINT
                    * variable.segment(num_points_ + 1, num_points_ - 2) +
                variable.segment(num_points_, num_points_ - 2));

        // velocity.
        diff_coeff_.segment(num_points_, num_points_) =
            variable.segment(0, num_points_);
    }

    /*!
     * \brief Get the differential coefficient.
     *
     * \return Differential coefficient.
     */
    [[nodiscard]] auto diff_coeff() const noexcept -> const variable_type& {
        return diff_coeff_;
    }

    /*!
     * \brief Get the spacial points.
     *
     * \return Points.
     */
    [[nodiscard]] auto points() const noexcept -> const variable_type& {
        return points_;
    }

private:
    //! Speed.
    double speed_;

    //! Number of points.
    num_collect::index_type num_points_;

    //! Spatial points.
    variable_type points_;

    //! Differential coefficient.
    variable_type diff_coeff_;
};

/*!
 * \brief Class to calculate exact solution of string_wave_1d_problem.
 */
class string_wave_1d_solution {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] params Parameters.
     */
    explicit string_wave_1d_solution(const string_wave_1d_parameters& params)
        : speed_(params.speed),
          length_(params.length),
          points_(Eigen::VectorXd::LinSpaced(
              params.num_points, 0.0, params.length)),
          solution_(Eigen::VectorXd::Zero(params.num_points * 2)) {}

    /*!
     * \brief Evaluate solution.
     *
     * \param[in] time Time.
     */
    void evaluate_on(double time) {
        const num_collect::index_type num_points = points_.size();

        // velocity.
        solution_.segment(0, num_points) =
            (points_ * num_collect::constants::pi<double> / length_)
                .array()
                .sin() *
            (-num_collect::constants::pi<double> * speed_ / length_) *
            std::sin(
                time * num_collect::constants::pi<double> * speed_ / length_);

        // displacement.
        solution_.segment(num_points, num_points) =
            (points_ * num_collect::constants::pi<double> / length_)
                .array()
                .sin() *
            std::cos(
                time * num_collect::constants::pi<double> * speed_ / length_);
    }

    /*!
     * \brief Get the solution.
     *
     * \return Solution.
     */
    [[nodiscard]] auto solution() -> const Eigen::VectorXd& {
        return solution_;
    }

private:
    //! Speed.
    double speed_;

    //! Length.
    double length_;

    //! Spatial points.
    Eigen::VectorXd points_;

    //! Solution.
    Eigen::VectorXd solution_;
};

}  // namespace num_prob_collect::ode
