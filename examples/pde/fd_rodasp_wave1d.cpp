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
 * \brief Example of solving the wave equation using finite difference and
 * RODASP formula.
 */
#include <cmath>
#include <exception>
#include <iostream>
#include <string_view>

#include <Eigen/Core>
#include <fmt/format.h>
#include <toml++/toml.h>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/pi.h"
#include "num_collect/logging/load_logging_config.h"
#include "num_collect/logging/logger.h"
#include "num_collect/ode/rosenbrock/bicgstab_rosenbrock_equation_solver.h"
#include "num_collect/ode/rosenbrock/rodasp_formula.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/format_dense_vector.h"

/*!
 * \brief Class of ODE problem to solve the 1-dimensional wave equation
 * discretized by the finite difference.
 */
class finite_difference_wave_equation_1d {
public:
    //! Type of variables.
    using variable_type = Eigen::VectorXd;

    //! Type of scalars.
    using scalar_type = double;

    //! Allowed evaluations.
    static constexpr auto allowed_evaluations =
        num_collect::ode::evaluation_type{.diff_coeff = true};

    //! Parameters.
    struct parameters {
    public:
        //! Speed of the wave.
        double speed{1.0};

        //! Number of spatial points.
        num_collect::index_type num_points{10};  // NOLINT

        //! Length of the space.
        double length{1.0};
    };

    /*!
     * \brief Constructor.
     *
     * \param[in] params Parameters.
     */
    explicit finite_difference_wave_equation_1d(const parameters& params)
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

class wave_equation_1d_solution {
public:
    //! Parameters.
    struct parameters {
    public:
        //! Speed of the wave.
        double speed{1.0};

        //! Length of the space.
        double length{1.0};
    };

    /*!
     * \brief Constructor.
     *
     * \param[in] params Parameters.
     */
    explicit wave_equation_1d_solution(const parameters& params)
        : speed_(params.speed), length_(params.length) {}

    /*!
     * \brief Evaluate solution.
     *
     * \param[in] time Time.
     * \param[in] points Spatial points.
     */
    void evaluate_on(double time, const Eigen::VectorXd& points) {
        const num_collect::index_type num_points = points.size();
        solution_.resize(num_points * 2);

        // vibration without derivative.
        solution_.segment(num_points, num_points) =
            (points * num_collect::constants::pi<double> / length_)
                .array()
                .sin() *
            std::cos(
                time * num_collect::constants::pi<double> * speed_ / length_);

        // velocity.
        solution_.segment(0, num_points) =
            (points * num_collect::constants::pi<double> / length_)
                .array()
                .sin() *
            (-num_collect::constants::pi<double> * speed_ / length_) *
            std::sin(
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

    //! Solution.
    Eigen::VectorXd solution_;
};

template <typename T>
static auto get_config_value(
    const toml::parse_result& config, std::string_view key) {
    const std::optional<T> res = config.as_table()
                                     ->at("fd_rodasp_wave1d")
                                     .as_table()
                                     ->at(key)
                                     .value<T>();
    if (!res) {
        throw num_collect::invalid_argument(
            fmt::format("Failed to load configuration {}.", key));
    }
    return res.value();
}

auto main(int argc, char** argv) -> int {
    try {
        std::string_view config_filepath = "examples/pde/fd_rodasp_wave1d.toml";
        if (argc == 2) {
            config_filepath = argv[1];  // NOLINT
        }
        num_collect::logging::load_logging_config(std::string(config_filepath));

        using problem_type = finite_difference_wave_equation_1d;
        using formula_type =
            num_collect::ode::rosenbrock::rodasp_formula<problem_type,
                num_collect::ode::rosenbrock::
                    bicgstab_rosenbrock_equation_solver<problem_type>>;
        using solver_type = num_collect::ode::embedded_solver<formula_type>;

        const auto config = toml::parse_file(config_filepath);
        const double speed = get_config_value<double>(config, "speed");
        const num_collect::index_type num_points =
            get_config_value<num_collect::index_type>(config, "num_points");
        const double length = get_config_value<double>(config, "length");

        problem_type problem{problem_type::parameters{
            .speed = speed, .num_points = num_points, .length = length}};
        solver_type solver{problem};

        wave_equation_1d_solution solution{
            wave_equation_1d_solution::parameters{
                .speed = speed, .length = length}};

        constexpr double init_time = 0.0;
        solution.evaluate_on(init_time, problem.points());
        solver.init(init_time, solution.solution());

        solver.step_size(1e-2);  // NOLINT

        constexpr double end_time = 0.1;
        solver.solve_till(end_time);

        num_collect::logging::logger logger;

        solution.evaluate_on(end_time, problem.points());
        logger.info()("Solution:  {:.3f}",
            num_collect::util::format_dense_vector(solver.variable()));
        logger.info()("Reference: {:.3f}",
            num_collect::util::format_dense_vector(solution.solution()));

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
