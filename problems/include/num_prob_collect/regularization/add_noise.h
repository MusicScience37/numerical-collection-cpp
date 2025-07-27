/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of add_noise function.
 */
#pragma once

#include <random>

#include <Eigen/Core>

#include "num_collect/base/index_type.h"

namespace num_prob_collect::regularization {

/*!
 * \brief Add noise to data.
 *
 * \param[in,out] data Data to which noise is added.
 * \param[in] rate Rate of noise in squared norm.
 */
inline void add_noise(Eigen::MatrixXd& data, double rate) {
    std::mt19937 engine;  // NOLINT: fix seed for reproducibility
    const double sigma =
        std::sqrt(data.squaredNorm() / static_cast<double>(data.size()) * rate);
    std::normal_distribution<double> dist{0.0, sigma};

    const num_collect::index_type rows = data.rows();
    const num_collect::index_type cols = data.cols();
    for (num_collect::index_type j = 0; j < cols; ++j) {
        for (num_collect::index_type i = 0; i < rows; ++i) {
            data(i, j) += dist(engine);
        }
    }
}

}  // namespace num_prob_collect::regularization
