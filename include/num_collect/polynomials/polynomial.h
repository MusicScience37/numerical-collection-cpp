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
    [[nodiscard]] auto operator[](index_type degree) const noexcept
        -> const coeff_type& {
        NUM_COLLECT_DEBUG_ASSERT(degree >= 0 && degree < coeffs_.size());
        return coeffs_[degree];
    }

    /*!
     * \brief Access a coefficient of the polynomial.
     *
     * \param[in] degree Degree of the coefficient.
     * \return Coefficient of the specified degree.
     */
    [[nodiscard]] auto operator[](index_type degree) noexcept -> coeff_type& {
        NUM_COLLECT_DEBUG_ASSERT(degree >= 0 && degree < coeffs_.size());
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

    /*!
     * \brief Change the degree of the polynomial.
     *
     * \param[in] new_degree New degree of the polynomial. -1 for the empty
     * polynomial.
     */
    void change_degree(index_type new_degree) {
        if (new_degree < 0) {
            coeffs_.clear();
        } else {
            coeffs_.resize(new_degree + 1);
        }
    }

    /*!
     * \brief Multiply a polynomial to this polynomial.
     *
     * \param[in] rhs Right-hand-side polynomial.
     * \return This polynomial after multiplication.
     */
    auto operator*=(const polynomial& rhs) -> polynomial& {
        // Multiplication creates a new vector of coefficients,
        // so implement operator*= in terms of operator*.
        *this = *this * rhs;
        return *this;
    }

    /*!
     * \brief Multiply a polynomial to this polynomial.
     *
     * \param[in] rhs Right-hand-side polynomial.
     * \return Product of this polynomial and the right-hand-side polynomial.
     */
    auto operator*(const polynomial& rhs) const -> polynomial {
        if (coeffs_.empty() || rhs.coeffs_.empty()) {
            return polynomial(coeff_vector_type{});
        }
        const index_type new_degree = degree() + rhs.degree();
        coeff_vector_type new_coeffs(
            new_degree + 1, static_cast<coeff_type>(0));
        for (index_type i = 0; i < rhs.coeffs_.size(); ++i) {
            for (index_type j = 0; j < coeffs_.size(); ++j) {
                new_coeffs[i + j] += rhs.coeffs_[i] * coeffs_[j];
            }
        }
        return polynomial(std::move(new_coeffs));
    }

private:
    //! Coefficients of the polynomial from the constant term to the highest degree term.
    coeff_vector_type coeffs_;
};

/*!
 * \brief Differentiate a polynomial in-place.
 *
 * \tparam Coeff Type of coefficients.
 * \param[in,out] poly Polynomial to be differentiated.
 *
 * \note Empty polynomials will be unchanged.
 */
template <typename Coeff>
void differentiate(polynomial<Coeff>& poly) {
    if (poly.degree() < 0) {
        return;
    }
    if (poly.degree() == 0) {
        poly[0] = static_cast<Coeff>(0);
        return;
    }
    for (index_type i = 1; i < poly.coeffs().size(); ++i) {
        poly[i - 1] = static_cast<Coeff>(i) * poly[i];
    }
    poly.coeffs().pop_back();
}

}  // namespace num_collect::polynomials
