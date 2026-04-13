/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of polynomial class.
 */
#pragma once

#include "num_collect/base/index_type.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/vector.h"

namespace num_collect::polynomials {

/*!
 * \brief Class of polynomials.
 *
 * \tparam Coeff Type of coefficients.
 */
template <typename Coeff>
class polynomial {
public:
    //! Type of coefficients.
    using coeff_type = Coeff;

    //! Type of vector of coefficients.
    using coeff_vector_type = util::vector<coeff_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] coeffs Coefficients of the polynomial from the constant term
     * to the highest degree term.
     */
    explicit polynomial(coeff_vector_type coeffs)
        : coeffs_(std::move(coeffs)) {}

    /*!
     * \brief Access the coefficients of the polynomial.
     *
     * \return Coefficients of the polynomial from the constant term to the
     * highest degree term.
     */
    [[nodiscard]] auto coeffs() const noexcept -> const coeff_vector_type& {
        return coeffs_;
    }

    /*!
     * \brief Access the coefficients of the polynomial.
     *
     * \return Coefficients of the polynomial from the constant term to the
     * highest degree term.
     */
    [[nodiscard]] auto coeffs() noexcept -> coeff_vector_type& {
        return coeffs_;
    }

    /*!
     * \brief Access a coefficient of the polynomial.
     *
     * \param[in] degree Degree of the coefficient.
     * \return Coefficient of the specified degree.
     */
    [[nodiscard]] auto operator[](std::size_t degree) const noexcept
        -> const coeff_type& {
        NUM_COLLECT_DEBUG_ASSERT(degree < coeffs_.size());
        return coeffs_[degree];
    }

    /*!
     * \brief Access a coefficient of the polynomial.
     *
     * \param[in] degree Degree of the coefficient.
     * \return Coefficient of the specified degree.
     */
    [[nodiscard]] auto operator[](std::size_t degree) noexcept -> coeff_type& {
        NUM_COLLECT_DEBUG_ASSERT(degree < coeffs_.size());
        return coeffs_[degree];
    }

    /*!
     * \brief Get the degree of the polynomial.
     *
     * \return Degree of the polynomial. -1 for the empty polynomial.
     */
    [[nodiscard]] auto degree() const noexcept -> index_type {
        if (coeffs_.empty()) {
            return -1;
        }
        return coeffs_.size() - 1;
    }

private:
    //! Coefficients of the polynomial from the constant term to the highest degree term.
    coeff_vector_type coeffs_;
};

}  // namespace num_collect::polynomials
