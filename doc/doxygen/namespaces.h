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
 * \brief documentation of namespaces
 */

//! Namespace of num_collect source codes.
namespace num_collect {
    //! Namespace of multiple precision numbers with double numbers.
    namespace multi_double {
        //! Namespace of implementations.
        namespace impl {}
    }  // namespace multi_double

    //! Namespace of automatic differentiation.
    namespace auto_diff {
        //! Namespace of forward-mode automatic differentiation.
        namespace forward {}
        //! Namespace of backward-mode automatic differentiation.
        namespace backward {
            //! Namespace of graphs in backward-mode automatic differentiation.
            namespace graph {}
        }  // namespace backward
    }      // namespace auto_diff

    //! Namespace of optimization algorithms.
    namespace opt {
        //! Namespace of implementations.
        namespace impl {}
    }  // namespace opt

    //! Namespace of constexpr variables and functions.
    namespace constants {
        //! Namespace of implementations.
        namespace impl {}
    }  // namespace constants

    //! Namespace of root-finding algorithms.
    namespace roots {
        //! Namespace of implementations.
        namespace impl {}
    }  // namespace roots

    //! Namespace of special functions.
    namespace functions {
        //! Namespace of implementations.
        namespace impl {}
    }  // namespace functions

    //! Namespace of numerical integration.
    namespace integration {
        //! Namespace of implementations.
        namespace impl {}
    }  // namespace integration

    //! Namespace of solvers of ordinary differential equations (ODE).
    namespace ode {
        //! Namespace of Runge-Kutta method.
        namespace runge_kutta {
            //! Namespace of implementations.
            namespace impl {}
        }  // namespace runge_kutta

        //! Namespace of average vector field (AVF) method \cite Quispel2008.
        namespace avf {
            //! Namespace of implementations.
            namespace impl {}
        }  // namespace avf
    }      // namespace ode
}  // namespace num_collect
