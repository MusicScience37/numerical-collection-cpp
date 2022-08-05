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
 * \brief Definition of shekel_function class.
 */
#pragma once

#include <Eigen/Core>

namespace num_prob_collect::opt {

/*!
 * \brief Class of Shekel function in 4 dimensions.
 *
 * This function has m local minimum and the global one at (4, 4, 4, 4).
 * Search region is [0, 10] for each dimension.
 *
 * Reference: https://www.sfu.ca/~ssurjano/shekel.html
 */
class shekel_function {
public:
    //! Type of variables.
    using variable_type = Eigen::Vector4d;

    //! Type of function values.
    using value_type = double;

    /*!
     * \brief Constructor.
     *
     * \param[in] m Number of terms.
     */
    explicit shekel_function(int m) : m_(m) {
        // NOLINTNEXTLINE
        beta_ << 0.1, 0.2, 0.2, 0.4, 0.4, 0.6, 0.3, 0.7, 0.5, 0.5;
        // NOLINTNEXTLINE
        c_ << 4.0, 1.0, 8.0, 6.0, 3.0, 2.0, 5.0, 8.0, 6.0, 7.0,
            // NOLINTNEXTLINE
            4.0, 1.0, 8.0, 6.0, 7.0, 9.0, 3.0, 1.0, 2.0, 3.6,
            // NOLINTNEXTLINE
            4.0, 1.0, 8.0, 6.0, 3.0, 2.0, 5.0, 8.0, 6.0, 7.0,
            // NOLINTNEXTLINE
            4.0, 1.0, 8.0, 6.0, 7.0, 9.0, 3.0, 1.0, 2.0, 3.6;
    }

    /*!
     * \brief Evaluate function value on variable.
     *
     * \param[in] x Variable.
     */
    void evaluate_on(const Eigen::Vector4d& x) {
        value_ = 0.0;
        for (int i = 0; i < m_; ++i) {
            value_ -= 1.0 / ((x - c_.col(i)).squaredNorm() + beta_(i));
        }
    }

    /*!
     * \brief Get function value.
     *
     * \return Function value.
     */
    [[nodiscard]] auto value() const -> const double& { return value_; }

private:
    //! Number of terms.
    int m_;

    //! Offset in the function.
    Eigen::Matrix<double, 10, 1> beta_{};  // NOLINT

    //! Centers in the function.
    Eigen::Matrix<double, 4, 10> c_{};  // NOLINT

    //! Function value.
    double value_{};
};

}  // namespace num_prob_collect::opt
