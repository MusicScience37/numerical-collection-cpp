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
 * \brief Definition of thin_plate_spline_rbf class.
 */
#pragma once

#include <cmath>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/constants/factorial.h"
#include "num_collect/constants/gamma_half.h"
#include "num_collect/constants/pi.h"
#include "num_collect/constants/pow.h"
#include "num_collect/rbf/rbfs/differentiated.h"
#include "num_collect/util/assert.h"

namespace num_collect::rbf::rbfs {

/*!
 * \brief Class of RBF of thin plate splines \cite Ghosh2010.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Dimension Dimension of the space.
 * \tparam Degree Number of differentiations.
 *
 * Thin plate splines uses a function space defined by the following inner
 * product of \f$ n \f$-th derivatives:
 * \f[
 * \sum_{\alpha_1 + \cdots + \alpha_d = n} \frac{n!}{\alpha_1! \cdots \alpha_d!}
 * \int_{-\infty}^{\infty} \cdots \int_{-\infty}^{\infty}
 * \frac{\partial^n f}{\partial x_1^{\alpha_1} \cdots \partial x_d^{\alpha_d}}
 * \frac{\partial^n g}{\partial x_1^{\alpha_1} \cdots \partial x_d^{\alpha_d}}
 * dx_1 \cdots dx_d
 * \f]
 * where functions \f$ f \f$ and \f$ g \f$
 * are from \f$ \mathbb{R}^d \f$ to \f$ \mathbb{R} \f$.
 *
 * This function space is a reproducing kernel Hilbert space
 * if \f$ 2n > d \f$, and the reproducing kernel is given as follows:
 * \f[
 * K(\boldsymbol{r}, \boldsymbol{s}) =
 * \begin{cases}
 *     \displaystyle
 *     \frac{(-1)^{d/2 + 1 + n}}{2^{2n-1} \pi^{d/2} (n-1)! (n-d/2)!}
 *     \|\boldsymbol{r} - \boldsymbol{s}\|_2^{2n-d}
 *     \log{\|\boldsymbol{r} - \boldsymbol{s}\|_2}
 *      &
 *     \text{if $2n-d$ is even}
 *     \\[1.5em]
 *     \displaystyle
 *     \frac{\Gamma(d/2 - n)}{2^{2n} \pi^{d/2} (n-1)!}
 *     \|\boldsymbol{r} - \boldsymbol{s}\|_2^{2n-d}
 *      &
 *     \text{if $2n-d$ is odd}
 * \end{cases}
 * \f]
 * where \f$ \boldsymbol{r}, \boldsymbol{s} \in \mathbb{R}^d \f$.
 * Note that these kernels go to zero
 * when \f$ \|\boldsymbol{r} - \boldsymbol{s}\|_2 \to 0 \f$.
 *
 * This class implements the reproducing kernel as a radial basis function
 * of the distance \f$ \|\boldsymbol{r} - \boldsymbol{s}\|_2 \f$.
 */
template <base::concepts::real_scalar Scalar, index_type Dimension,
    index_type Degree>
    requires(Dimension > 0 && Degree > 0 && Degree * 2 > Dimension)
class thin_plate_spline_rbf {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Calculate the coefficient of this RBF.
     *
     * \return Coefficient of this RBF.
     */
    [[nodiscard]] static constexpr auto coefficient() noexcept -> scalar_type {
        if constexpr (Dimension % 2 == 0) {
            // 2n - d is even
            const scalar_type numerator = constants::pow(
                static_cast<scalar_type>(-1), Dimension / 2 + 1 + Degree);
            const scalar_type denominator =
                constants::pow(static_cast<scalar_type>(2), 2 * Degree - 1) *
                constants::pow(constants::pi<scalar_type>, Dimension / 2) *
                constants::factorial<scalar_type>(Degree - 1) *
                constants::factorial<scalar_type>(Degree - Dimension / 2);
            return numerator / denominator;
        } else {
            // 2n - d is odd
            const scalar_type numerator =
                constants::gamma_half_plus<scalar_type>(
                    (Dimension - 1) / 2 - Degree);
            const scalar_type denominator =
                constants::pow(static_cast<scalar_type>(2), 2 * Degree) *
                constants::pow(constants::pi<scalar_type>,
                    static_cast<scalar_type>(Dimension) /
                        static_cast<scalar_type>(2)) *
                constants::factorial<scalar_type>(Degree - 1);
            return numerator / denominator;
        }
    }

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance Distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(const scalar_type& distance) const noexcept
        -> scalar_type {
        NUM_COLLECT_DEBUG_ASSERT(distance >= static_cast<scalar_type>(0));

        static constexpr auto coeff = coefficient();
        if constexpr (Dimension % 2 == 0) {
            // 2n - d is even
            using std::log;
            using std::pow;
            if (distance == static_cast<scalar_type>(0)) {
                return static_cast<scalar_type>(0);
            }
            return coeff * pow(distance, 2 * Degree - Dimension) *
                log(distance);
        } else {
            // 2n - d is odd
            using std::pow;
            return coeff * pow(distance, 2 * Degree - Dimension);
        }
    }
};

/*!
 * \brief Class of differentiated RBF of thin plate splines \cite Ghosh2010.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Dimension Dimension of the space.
 * \tparam Degree Number of differentiations.
 */
template <base::concepts::real_scalar Scalar, index_type Dimension,
    index_type Degree>
    requires(Dimension > 0 && Degree > 0 && Degree * 2 > Dimension)
class differentiated_thin_plate_spline_rbf {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Calculate a function value of RBF.
     *
     * \param[in] distance Distance.
     * \return Value of this RBF.
     */
    [[nodiscard]] auto operator()(const scalar_type& distance) const noexcept
        -> scalar_type {
        NUM_COLLECT_DEBUG_ASSERT(distance >= static_cast<scalar_type>(0));

        static constexpr auto coeff =
            thin_plate_spline_rbf<Scalar, Dimension, Degree>::coefficient();
        if constexpr (2 * Degree - Dimension == 1) {
            // 2n - d = 1 (special case)
            // In this case, preventing division by zero is needed.
            constexpr auto small_number = static_cast<scalar_type>(1e-50);
            if (distance < small_number) {
                return -coeff / small_number;
            }
            return -coeff / distance;
        } else if constexpr (2 * Degree - Dimension == 2) {
            // 2n - d = 2 (special case)
            // In this case, preventing pow(0, 0) is needed.
            using std::log;
            using std::pow;
            const scalar_type no_log_part = -coeff;
            if (distance == static_cast<scalar_type>(0)) {
                return no_log_part;
            }
            const scalar_type log_part =
                no_log_part * static_cast<scalar_type>(2) * log(distance);
            return log_part + no_log_part;
        } else if constexpr (Dimension % 2 == 0) {
            // 2n - d is even
            using std::log;
            using std::pow;
            const scalar_type no_log_part =
                -coeff * pow(distance, 2 * Degree - Dimension - 2);
            if (distance == static_cast<scalar_type>(0)) {
                return no_log_part;
            }
            const scalar_type log_part = no_log_part *
                static_cast<scalar_type>(2 * Degree - Dimension) *
                log(distance);
            return log_part + no_log_part;
        } else {
            // 2n - d is odd
            using std::pow;
            return -coeff * (2 * Degree - Dimension) *
                pow(distance, 2 * Degree - Dimension - 2);
        }
    }
};

/*!
 * \brief Specialization of num_collect::rbf::rbfs::differentiated for
 * num_collect::rbf::rbfs::thin_plate_spline_rbf.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Dimension Dimension of the space.
 * \tparam Degree Number of differentiations.
 */
template <base::concepts::real_scalar Scalar, index_type Dimension,
    index_type Degree>
struct differentiated<thin_plate_spline_rbf<Scalar, Dimension, Degree>> {
    //! Type of the differentiated RBF.
    using type =
        differentiated_thin_plate_spline_rbf<Scalar, Dimension, Degree>;
};

}  // namespace num_collect::rbf::rbfs
