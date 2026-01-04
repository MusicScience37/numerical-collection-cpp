/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Declaration of write_png function.
 */
#pragma once

#include <string>

#include <Eigen/Core>

/*!
 * \brief Write a PNG file.
 *
 * \param[in] matrix Matrix of data.
 * \param[in] file_path Path to the PNG file.
 * \param[in] max_value Maximum value in data. This value is mapped to 255 in
 * PNG.
 * \param[in] min_value Minimum value in data. This value is mapped to 0 in PNG.
 */
void write_png(const Eigen::MatrixXd& matrix, const std::string& file_path,
    double max_value = 1.0, double min_value = 0.0);
