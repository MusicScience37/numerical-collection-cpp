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
 * \brief Definition of implicit_formula_solver_strategies namespace.
 */
#pragma once

namespace num_collect::ode::runge_kutta {  // NOLINT

//! Namespace of tags for solvers in implicit formulas in Runge-Kutta method.
namespace implicit_formula_solver_strategies {  // NOLINT

/*!
 * \brief Tag class for modified Newton-Raphson method.
 */
struct modified_newton_raphson_tag {};

}  // namespace implicit_formula_solver_strategies

}  // namespace num_collect::ode::runge_kutta
