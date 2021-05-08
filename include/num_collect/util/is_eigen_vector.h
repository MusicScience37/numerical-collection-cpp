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
 * \brief Definition of is_eigen_vector class.
 */
#pragma once

#include <Eigen/Core>
#include <type_traits>

namespace num_collect {

/*!
 * \brief Class to check whether a type is a Eigen's vector.
 *
 * \tparam Type Type to be checked.
 */
template <typename Type>
class is_eigen_vector : public std::false_type {};

/*!
 * \brief Class to check whether a type is a Eigen's vector.
 *
 * \tparam Type Type to be checked.
 */
template <typename Scalar, int Rows, int Options, int MaxRows, int MaxCols>
class is_eigen_vector<Eigen::Matrix<Scalar, Rows, 1, Options, MaxRows, MaxCols>>
    : public std::true_type {};

/*!
 * \brief Get whether a type is a Eigen's vector.
 *
 * \tparam Type Type to be checked.
 */
template <typename Type>
constexpr bool is_eigen_vector_v = is_eigen_vector<Type>::value;

}  // namespace num_collect
