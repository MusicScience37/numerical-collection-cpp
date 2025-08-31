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
 * \brief Definition of polynomial_term class.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/index_type.h"

namespace num_collect::rbf {

/*!
 * \brief Class of terms of polynomials used in RBF interpolation.
 *
 * \tparam NumDimensions Number of dimensions of variables.
 *
 * This class has a vector of degrees for each dimension.
 * For example, \f$ x^2 y \f$ can be expressed using a vector `[2, 1]`.
 */
template <int NumDimensions>
    requires(NumDimensions >= 1)
class polynomial_term {
public:
    //! Type of degrees.
    using degree_type = int;

    //! Type of the vector of degrees.
    using degree_vector_type = Eigen::Vector<degree_type, NumDimensions>;

    /*!
     * \brief Constructor.
     *
     * \param[in] degrees Degrees.
     */
    explicit polynomial_term(
        const Eigen::Vector<int, NumDimensions>& degrees) noexcept
        : degrees_(degrees) {}

    /*!
     * \brief Get degrees.
     *
     * \return Degrees.
     */
    [[nodiscard]] auto degrees() const noexcept -> const degree_vector_type& {
        return degrees_;
    }

    /*!
     * \brief Evaluate this polynomial for a variable.
     *
     * \tparam Scalar Type of scalars in the variable.
     * \param[in] variable Variable.
     * \return Value of the polynomial.
     */
    template <typename Scalar>
    [[nodiscard]] auto operator()(
        const Eigen::Vector<Scalar, NumDimensions>& variable) const noexcept
        -> Scalar {
        auto result = static_cast<Scalar>(1);
        for (index_type i = 0; i < NumDimensions; ++i) {
            using std::pow;
            result *= static_cast<Scalar>(pow(variable(i), degrees_(i)));
        }
        return result;
    }

private:
    //! Degrees.
    degree_vector_type degrees_;
};

/*!
 * \brief Class of terms of polynomials used in RBF interpolation.
 *
 * This specialization handles one-dimensional variables (scalars).
 */
template <>
class polynomial_term<1> {
public:
    //! Type of degrees.
    using degree_type = int;

    /*!
     * \brief Constructor.
     *
     * \param[in] degree Degree.
     */
    explicit polynomial_term(int degree) noexcept : degree_(degree) {}

    /*!
     * \brief Constructor.
     *
     * \param[in] degrees Degrees.
     *
     * \note This function exists for compatibility with non-specialized case.
     */
    explicit polynomial_term(const Eigen::Vector<int, 1>& degrees) noexcept
        : polynomial_term(degrees(0)) {}

    /*!
     * \brief Get degree.
     *
     * \return Degree.
     */
    [[nodiscard]] auto degree() const noexcept -> degree_type {
        return degree_;
    }

    /*!
     * \brief Get degrees.
     *
     * \return Degrees.
     *
     * \note This function exists for compatibility with non-specialized case.
     */
    [[nodiscard]] auto degrees() const noexcept -> Eigen::Vector<int, 1> {
        return Eigen::Vector<int, 1>{{degree_}};
    }

    /*!
     * \brief Evaluate this polynomial for a variable.
     *
     * \tparam Scalar Type of scalars in the variable.
     * \param[in] variable Variable.
     * \return Value of the polynomial.
     */
    template <typename Scalar>
        requires(!concepts::dense_vector<Scalar>)
    [[nodiscard]] auto operator()(const Scalar& variable) const noexcept
        -> Scalar {
        using std::pow;
        return static_cast<Scalar>(pow(variable, degree_));
    }

    /*!
     * \brief Evaluate this polynomial for a variable.
     *
     * \tparam Scalar Type of scalars in the variable.
     * \param[in] variable Variable.
     * \return Value of the polynomial.
     *
     * \note This function exists for compatibility with non-specialized case.
     */
    template <typename Scalar>
    [[nodiscard]] auto operator()(
        const Eigen::Vector<Scalar, 1>& variable) const noexcept -> Scalar {
        return operator()(variable(0));
    }

private:
    //! Degree.
    degree_type degree_;
};

}  // namespace num_collect::rbf
