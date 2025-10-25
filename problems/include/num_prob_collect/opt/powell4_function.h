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
 * \brief Definition of powell4_function class.
 */
#pragma once

#include <Eigen/Core>

namespace num_prob_collect::opt {

/*!
 * \brief Class of Powell function in 4 dimensions.
 *
 * This function has no local minimum except for the global one at
 * (0, 0, 0, 0).
 * Search region is [-4, 5] for each dimension.
 *
 * Reference:
 * http://www-optima.amp.i.kyoto-u.ac.jp/member/student/hedar/Hedar_files/TestGO_files/Page2720.htm
 */
class powell4_function {
public:
    //! Type of variables.
    using variable_type = Eigen::Vector4d;

    //! Type of function values.
    using value_type = double;

    /*!
     * \brief Evaluate function value on variable.
     *
     * \param[in] x Variable.
     */
    void evaluate_on(const Eigen::Vector4d& x) {
        value_ = std::pow(x(0) + 10.0 * x(1), 2) +
            5.0 * std::pow(x(2) - x(3), 2) + std::pow(x(1) - 2.0 * x(2), 4) +
            10.0 * std::pow(x(0) - x(3), 4);

        grad_(0) = 2.0 * (x(0) + 10.0 * x(1)) + 40.0 * std::pow(x(0) - x(3), 3);
        grad_(1) =
            20.0 * (x(0) + 10.0 * x(1)) + 4.0 * std::pow(x(1) - 2.0 * x(2), 3);
        grad_(2) = 10.0 * (x(2) - x(3)) - 8.0 * std::pow(x(1) - 2.0 * x(2), 3);
        grad_(3) = -10.0 * (x(2) - x(3)) - 40.0 * std::pow(x(0) - x(3), 3);
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
    [[nodiscard]] auto gradient() const -> const Eigen::Vector4d& {
        return grad_;
    }

private:
    //! Function value.
    double value_{};

    //! Gradient.
    Eigen::Vector4d grad_{};
};

}  // namespace num_prob_collect::opt
