/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of formattable_real_scalar class.
 */
#pragma once

#include "num_collect/base/concepts/formattable.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar.h"  // IWYU pragma: keep

namespace num_collect::logging::concepts {

/*!
 * \brief Concept of formattable real scalars.
 *
 * \tparam T Type.
 */
template <typename T>
concept formattable_real_scalar =
    base::concepts::formattable<T> && base::concepts::real_scalar<T>;

}  // namespace num_collect::logging::concepts
