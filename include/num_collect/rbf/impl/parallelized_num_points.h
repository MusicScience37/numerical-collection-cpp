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
 * \brief Definition of parallelized_num_points variable.
 */
#pragma once

#include "num_collect/base/index_type.h"

namespace num_collect::rbf::impl {

/*!
 * \brief Number of points to enable parallelization.
 *
 * This value is determined in benchmarks.
 */
constexpr index_type parallelized_num_points = 50;

}  // namespace num_collect::rbf::impl
