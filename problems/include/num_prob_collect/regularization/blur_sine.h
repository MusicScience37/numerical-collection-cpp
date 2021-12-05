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
 * \brief Definition of blur_sine class.
 */
#pragma once

#include <Eigen/Core>
#include <cmath>

#include "num_collect/util/index_type.h"

namespace num_prob_collect::regularization {

/*!
 * \brief Class of problem of blurred sine function.
 */
class blur_sine {
public:
    //! Type of coefficient matrices.
    using coeff_type = Eigen::MatrixXd;

    //! Type of data vector.
    using data_type = Eigen::VectorXd;

    /*!
     * \brief Construct.
     *
     * \param[in] data_size Size of data.
     * \param[in] solution_size Size of solution.
     */
    blur_sine(num_collect::index_type data_size,
        num_collect::index_type solution_size) {
        coeff_.resize(data_size, solution_size);
        for (num_collect::index_type j = 0; j < solution_size; ++j) {
            for (num_collect::index_type i = 0; i < data_size; ++i) {
                static constexpr double factor = 100.0;
                const double sol_rate =
                    static_cast<double>(j) / static_cast<double>(solution_size);
                const double data_rate =
                    static_cast<double>(i) / static_cast<double>(data_size);
                const double diff = sol_rate - data_rate;
                coeff_(i, j) = std::exp(-factor * diff * diff);
            }
        }

        solution_.resize(solution_size);
        for (num_collect::index_type i = 0; i < solution_size; ++i) {
            static constexpr double factor = 10.0;
            const double sol_rate =
                static_cast<double>(i) / static_cast<double>(solution_size);
            solution_(i) = std::sin(factor * sol_rate);
        }

        data_ = coeff_ * solution_;
    }

    /*!
     * \brief Get the coefficient matrix.
     *
     * \return Coefficient matrix.
     */
    [[nodiscard]] auto coeff() const -> const Eigen::MatrixXd& {
        return coeff_;
    }

    /*!
     * \brief Get the solution.
     *
     * \return Solution.
     */
    [[nodiscard]] auto solution() const -> const Eigen::VectorXd& {
        return solution_;
    }

    /*!
     * \brief Get the data.
     *
     * \return Data.
     */
    [[nodiscard]] auto data() const -> const Eigen::VectorXd& { return data_; }

private:
    //! Coefficient matrix.
    Eigen::MatrixXd coeff_;

    //! Solution.
    Eigen::VectorXd solution_;

    //! Data.
    Eigen::VectorXd data_;
};

}  // namespace num_prob_collect::regularization
