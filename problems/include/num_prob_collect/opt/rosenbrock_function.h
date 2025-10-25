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
 * \brief Definition of rosenbrock_function class.
 */
#pragma once

#include <Eigen/Core>

namespace num_prob_collect::opt {

/*!
 * \brief Class of Rosenbrock function in 3 dimensions.
 *
 * This function has no local minimum except for the global one at
 * (1, 1, 1).
 *
 * Reference:
 * http://www-optima.amp.i.kyoto-u.ac.jp/member/student/hedar/Hedar_files/TestGO_files/Page2537.htm
 */
class rosenbrock_function {
public:
    //! Type of variables.
    using variable_type = Eigen::Vector3d;

    //! Type of function values.
    using value_type = double;

    /*!
     * \brief Evaluate function value on variable.
     *
     * \param[in] x Variable.
     */
    void evaluate_on(const Eigen::Vector3d& x) {
        value_ = 100.0 * std::pow(x(1) - x(0) * x(0), 2) +
            std::pow(x(0) - 1.0, 2) + 100.0 * std::pow(x(2) - x(1) * x(1), 2) +
            std::pow(x(1) - 1.0, 2);

        grad_(0) = -400.0 * (x(1) - x(0) * x(0)) * x(0) + 2.0 * (x(0) - 1.0);
        grad_(1) = 200.0 * (x(1) - x(0) * x(0)) -
            400.0 * (x(2) - x(1) * x(1)) * x(1) + 2.0 * (x(1) - 1.0);
        grad_(2) = 200.0 * (x(2) - x(1) * x(1));
    }

    /*!
     * \brief Get function value.
     *
     * \return Function value.
     */
    [[nodiscard]] auto value() const -> const double& { return value_; }

    /*!
     * \brief Get gradient.
     *
     * \return Gradient.
     */
    [[nodiscard]] auto gradient() const -> const Eigen::Vector3d& {
        return grad_;
    }

private:
    //! Function value.
    double value_{};

    //! Gradient.
    Eigen::Vector3d grad_{};
};

}  // namespace num_prob_collect::opt
