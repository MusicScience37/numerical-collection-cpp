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
 * \brief Definition of kahan_addable class.
 */
#pragma once

namespace num_collect::util::concepts {

/*!
 * \brief Concept of types usable in kahan_adder class.
 *
 * \tparam T Type.
 */
template <typename T>
concept kahan_addable = requires(T& a, const T& b, const T& c) {
    a += b;
    a -= b;
    a = b - c;
};

}  // namespace num_collect::util::concepts
