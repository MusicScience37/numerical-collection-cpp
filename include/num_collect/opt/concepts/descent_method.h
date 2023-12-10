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
 * \brief Definition of descent_method concept.
 */
#pragma once

#include "num_collect/base/concepts/const_reference_of.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/reference_of.h"        // IWYU pragma: keep
#include "num_collect/opt/concepts/line_searcher.h"        // IWYU pragma: keep
#include "num_collect/opt/concepts/optimizer.h"            // IWYU pragma: keep

namespace num_collect::opt::concepts {

/*!
 * \brief Concept of descent methods for optimization.
 *
 * \tparam T Type.
 */
template <typename T>
concept descent_method = optimizer<T> &&
        requires()
{
    typename T::line_searcher_type;
    requires line_searcher<typename T::line_searcher_type>;

    requires requires(T& solver, const typename T::variable_type& init_var) {
        { solver.init(init_var) };
    };

    requires requires(T& solver) {
        {
            solver.line_searcher()
        } -> base::concepts::reference_of<typename T::line_searcher_type>;
    };

    requires requires(const T& solver) {
        {
            solver.line_searcher()
        } -> base::concepts::const_reference_of<typename T::line_searcher_type>;

        {
            solver.gradient()
        } -> base::concepts::const_reference_of<typename T::variable_type>;
    };
};

}  // namespace num_collect::opt::concepts
