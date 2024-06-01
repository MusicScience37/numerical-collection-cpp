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
 * \brief Definition of kernel_matrix_type enumeration.
 */
#pragma once

#include <cstdint>

namespace num_collect::rbf {

/*!
 * \brief Enumeration of types of kernel matrices.
 */
enum class kernel_matrix_type : std::uint8_t {
    //! Dense matrix.
    dense,

    //! Sparse matrix.
    sparse
};

}  // namespace num_collect::rbf
