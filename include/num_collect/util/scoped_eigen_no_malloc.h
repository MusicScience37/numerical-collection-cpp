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
 * \brief Definition of scoped_eigen_no_malloc class.
 */
#pragma once

#include <Eigen/Core>
#include <Eigen/src/Core/util/Memory.h>

namespace num_collect::util {

/*!
 * \brief Class to temporarily prohibit dynamic memory allocation in Eigen.
 *
 * This class disable dynamic memory allocation in Eigen in constructor,
 * and re-enable it in destructor.
 *
 * \note This class works only when `EIGEN_RUNTIME_NO_MALLOC` is defined.
 * Otherwise, this class has no effect.
 */
class scoped_eigen_no_malloc {
public:
    /*!
     * \brief Constructor.
     */
    scoped_eigen_no_malloc() {
#ifdef EIGEN_RUNTIME_NO_MALLOC
        Eigen::internal::set_is_malloc_allowed(false);
#endif
    }

    /*!
     * \brief Destructor.
     */
    ~scoped_eigen_no_malloc() {
#ifdef EIGEN_RUNTIME_NO_MALLOC
        Eigen::internal::set_is_malloc_allowed(true);
#endif
    }

    scoped_eigen_no_malloc(const scoped_eigen_no_malloc&) = delete;
    scoped_eigen_no_malloc(scoped_eigen_no_malloc&&) = delete;
    auto operator=(const scoped_eigen_no_malloc&)
        -> scoped_eigen_no_malloc& = delete;
    auto operator=(scoped_eigen_no_malloc&&)
        -> scoped_eigen_no_malloc& = delete;
};

}  // namespace num_collect::util
