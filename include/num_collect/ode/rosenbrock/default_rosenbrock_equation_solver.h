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
 * \brief Definition of default_rosenbrock_equation_solver class.
 */
#pragma once

#include "num_collect/ode/concepts/multi_variate_differentiable_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/problem.h"  // IWYU pragma: keep
#include "num_collect/ode/concepts/single_variate_differentiable_problem.h"  // IWYU pragma: keep
#include "num_collect/ode/rosenbrock/lu_rosenbrock_equation_solver.h"
#include "num_collect/ode/rosenbrock/scalar_rosenbrock_equation_solver.h"

namespace num_collect::ode::rosenbrock {

/*!
 * \brief Class to get the default class to solve equations in Rosenbrock
 * methods.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::problem Problem>
struct default_rosenbrock_equation_solver;

/*!
 * \brief Class to get the default class to solve equations in Rosenbrock
 * methods.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::single_variate_differentiable_problem Problem>
struct default_rosenbrock_equation_solver<Problem> {
public:
    //! Type of the solver.
    using type = scalar_rosenbrock_equation_solver<Problem>;
};

/*!
 * \brief Class to get the default class to solve equations in Rosenbrock
 * methods.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::multi_variate_differentiable_problem Problem>
struct default_rosenbrock_equation_solver<Problem> {
public:
    //! Type of the solver.
    using type = lu_rosenbrock_equation_solver<Problem>;
};

/*!
 * \brief Default class to solve equations in Rosenbrock methods.
 *
 * \tparam Problem Type of the problem.
 */
template <concepts::problem Problem>
using default_rosenbrock_equation_solver_t =
    typename default_rosenbrock_equation_solver<Problem>::type;

}  // namespace num_collect::ode::rosenbrock
