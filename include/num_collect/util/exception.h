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
 * \brief Definition of exceptions.
 */
#pragma once

#include <stdexcept>

namespace num_collect {

/*!
 * \brief Class of exception in this project.
 */
class num_collect_exception : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/*!
 * \brief Class of exception on assertion failure.
 */
class assetion_failure : public num_collect_exception {
public:
    using num_collect_exception::num_collect_exception;
};

}  // namespace num_collect
