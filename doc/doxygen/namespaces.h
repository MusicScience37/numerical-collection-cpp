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
 * \brief Documentation of namespaces
 */

/*!
 * \dir num_collect
 * \brief Directory of num_collect source codes.
 */

/*!
 * \dir num_collect/util
 * \brief Directory of utility classes.
 */

//! Namespace of num_collect source codes.
namespace num_collect {

    /*!
     * \dir num_collect/base
     * \brief Directory of definitions common in this project.
     */

    //! Namespace of definitions common in this project.
    inline namespace base {

        /*!
         * \dir num_collect/base/concepts
         * \brief Directory of C++ concepts.
         */

        //! Namespace of C++ concepts.
        namespace concepts {

            //! Namespace of internal implementations.
            namespace impl {}

        }  // namespace concepts

        //! Namespace of internal implementations.
        namespace impl {}

    }  // namespace base

    /*!
     * \dir num_collect/logging
     * \brief Directory of logging.
     */

    //! Namespace of logging.
    namespace logging {

        /*!
         * \dir num_collect/logging/concepts
         * \brief Directory of C++ concepts.
         */

        //! Namespace of C++ concepts.
        namespace concepts {}

        /*!
         * \dir num_collect/logging/formatters
         * \brief Directory of formatters of logs.
         */

        //! Namespace of formatters of logs.
        namespace formatters {

            //! Namespace of internal implementations.
            namespace impl {}

        }  // namespace formatters

        /*!
         * \dir num_collect/logging/sinks
         * \brief Directory of log sinks.
         */

        //! Namespace of log sinks.
        namespace sinks {

            //! Namespace of internal implementations.
            namespace impl {}

        }  // namespace sinks

        /*!
         * \dir num_collect/logging/config
         * \brief Directory of logging configuration.
         */

        //! Namespace of logging configuration.
        namespace config {

            /*!
             * \dir num_collect/logging/config/toml
             * \brief Directory of logging configuration in TOML files.
             */

            //! Namespace of logging configuration in TOML files.
            namespace toml {

                //! Namespace of internal implementations.
                namespace impl {}

            }  // namespace toml

        }  // namespace config

        /*!
         * \dir num_collect/logging/iterations
         * \brief Directory of iteration logs.
         */

        //! Namespace of iteration logs.
        namespace iterations {

            //! Namespace of internal implementations.
            namespace impl {}

        }  // namespace iterations

        /*!
         * \dir num_collect/logging/impl
         * \brief Directory of implementation.
         */

        //! Namespace of internal implementations.
        namespace impl {}

    }  // namespace logging

    /*!
     * \dir num_collect/auto_diff
     * \brief Directory of automatic differentiation.
     */

    //! Namespace of automatic differentiation.
    namespace auto_diff {

        /*!
         * \dir num_collect/auto_diff/forward
         * \brief Directory of forward-mode automatic
         * differentiation.
         */

        //! Namespace of forward-mode automatic differentiation.
        namespace forward {

            //! Namespace of internal implementations.
            namespace impl {}

        }  // namespace forward

        /*!
         * \dir num_collect/auto_diff/backward
         * \brief Directory of backward-mode automatic
         * differentiation.
         */

        //! Namespace of backward-mode automatic differentiation.
        namespace backward {

            /*!
             * \dir num_collect/auto_diff/backward/graph
             * \brief Directory of graphs in backward-mode automatic
             * differentiation.
             */

            //! Namespace of graphs in backward-mode automatic differentiation.
            namespace graph {

                //! Namespace of internal implementations.
                namespace impl {}

            }  // namespace graph

            //! Namespace of internal implementations.
            namespace impl {}

        }  // namespace backward
    }  // namespace auto_diff

    /*!
     * \dir num_collect/opt
     * \brief Directory of optimization algorithms.
     */

    //! Namespace of optimization algorithms.
    namespace opt {

        /*!
         * \dir num_collect/opt/concepts
         * \brief Directory of C++ concepts.
         */

        //! Namespace of C++ concepts.
        namespace concepts {}

        /*!
         * \dir num_collect/opt/impl
         * \brief Directory of implementation.
         */

        //! Namespace of internal implementations.
        namespace impl {}

    }  // namespace opt

    /*!
     * \dir num_collect/regularization
     * \brief Directory of regularization algorithms.
     */

    //! Namespace of regularization algorithms.
    namespace regularization {

        /*!
         * \dir num_collect/regularization/concepts
         * \brief Directory of C++ concepts.
         */

        //! Namespace of C++ concepts.
        namespace concepts {}

        /*!
         * \dir num_collect/regularization/impl
         * \brief Directory of internal implementations.
         */

        //! Namespace of internal implementations.
        namespace impl {}

    }  // namespace regularization

    /*!
     * \dir num_collect/rbf
     * \brief Directory of RBF interpolation.
     */

    //! Namespace of RBF interpolation.
    namespace rbf {

        /*!
         * \dir num_collect/rbf/concepts
         * \brief Directory of C++ concepts.
         */

        //! Namespace of C++ concepts.
        namespace concepts {}

        /*!
         * \dir num_collect/rbf/distance_functions
         * \brief Directory of distance functions for RBF interpolation.
         */

        //! Namespace of distance functions for RBF interpolation.
        namespace distance_functions {}

        /*!
         * \dir num_collect/rbf/impl
         * \brief Directory of internal implementations.
         */

        //! Namespace of internal implementations.
        namespace impl {}

        /*!
         * \dir num_collect/rbf/length_parameter_calculators
         * \brief Directory of calculators of length parameters.
         */

        //! Namespace of calculators of length parameters.
        namespace length_parameter_calculators {}

        /*!
         * \dir num_collect/rbf/rbfs
         * \brief Directory of RBFs.
         */

        //! Namespace of RBFs.
        namespace rbfs {}

    }  // namespace rbf

    /*!
     * \dir num_collect/numbers
     * \brief Directory of classes of numbers.
     */

    //! Namespace of classes of numbers.
    namespace numbers {

        //! Namespace of internal implementations.
        namespace impl {}

    }  // namespace numbers

    /*!
     * \dir num_collect/multi_double
     * \brief Directory of multiple precision numbers with double numbers.
     */

    //! Namespace of multiple precision numbers with double numbers.
    namespace multi_double {

        /*!
         * \dir num_collect/multi_double/impl
         * \brief Directory of internal implementations.
         */

        //! Namespace of internal implementations.
        namespace impl {}

    }  // namespace multi_double

    /*!
     * \dir num_collect/constants
     * \brief Directory of constexpr variables and functions.
     */

    //! Namespace of constexpr variables and functions.
    namespace constants {

        /*!
         * \dir num_collect/constants/impl
         * \brief Directory of internal implementations.
         */

        //! Namespace of internal implementations.
        namespace impl {}

    }  // namespace constants

    /*!
     * \dir num_collect/roots
     * \brief Directory of root-finding algorithms.
     */

    //! Namespace of root-finding algorithms.
    namespace roots {

        /*!
         * \dir num_collect/roots/concepts
         * \brief Directory of C++ concepts.
         */

        //! Namespace of C++ concepts.
        namespace concepts {}

        //! Namespace of internal implementations.
        namespace impl {}

    }  // namespace roots

    /*!
     * \dir num_collect/functions
     * \brief Directory of special functions.
     */

    //! Namespace of special functions.
    namespace functions {

        //! Namespace of internal implementations.
        namespace impl {}

    }  // namespace functions

    /*!
     * \dir num_collect/integration
     * \brief Directory of numerical integration.
     */

    //! Namespace of numerical integration.
    namespace integration {

        //! Namespace of internal implementations.
        namespace impl {}

    }  // namespace integration

    /*!
     * \dir num_collect/ode
     * \brief Directory of solvers of ordinary differential equations (ODE).
     */

    //! Namespace of solvers of ordinary differential equations (ODE).
    namespace ode {

        /*!
         * \dir num_collect/ode/concepts
         * \brief Directory of C++ concepts.
         */

        //! Namespace of C++ concepts.
        namespace concepts {}

        /*!
         * \dir num_collect/ode/impl
         * \brief Directory of internal implementations.
         */

        //! Namespace of internal implementations.
        namespace impl {}

        /*!
         * \dir num_collect/ode/runge_kutta
         * \brief Directory of Runge-Kutta method.
         */

        //! Namespace of Runge-Kutta method.
        namespace runge_kutta {}

        /*!
         * \dir num_collect/ode/rosenbrock
         * \brief Directory of Rosenbrock method.
         */

        //! Namespace of Rosenbrock method.
        namespace rosenbrock {}

        /*!
         * \dir num_collect/ode/avf
         * \brief Directory of average vector field (AVF) method.
         */

        //! Namespace of average vector field (AVF) method.
        namespace avf {

            /*!
             * \dir num_collect/ode/avf/impl
             * \brief Directory of internal implementations.
             */

            //! Namespace of internal implementations.
            namespace impl {}

        }  // namespace avf

        /*!
         * \dir num_collect/ode/symplectic
         * \brief Directory of symplectic integration.
         */

        //! Namespace of symplectic integration.
        namespace symplectic {}

    }  // namespace ode

    /*!
     * \dir num_collect/linear
     * \brief Directory of solvers of linear equations.
     */

    //! Namespace of solvers of linear equations.
    namespace linear {

        /*!
         * \dir num_collect/linear/impl
         * \brief Directory of internal implementations.
         */

        //! Namespace of internal implementations.
        namespace impl {

            /*!
             * \dir num_collect/linear/impl/amg
             * \brief Directory of internal implementations of algebraic
             * multigrid method \cite Ruge1987.
             */

            //! Namespace of internal implementations of algebraic multigrid method \cite Ruge1987.
            namespace amg {}

        }  // namespace impl

    }  // namespace linear

    /*!
     * \dir num_collect/util
     * \brief Directory of utility classes.
     */

    //! Namespace of utilities.
    namespace util {

        /*!
         * \dir num_collect/util/concepts
         * \brief Directory of C++ concepts.
         */

        //! Namespace of C++ concepts.
        namespace concepts {}

        /*!
         * \dir num_collect/util/impl
         * \brief Directory of implementation.
         */

        //! Namespace of internal implementations.
        namespace impl {}

    }  // namespace util

}  // namespace num_collect

//! Namespace of Eigen library.
namespace Eigen {}

//! Namespace of fmt library.
namespace fmt {}
