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
 * \brief Definition of adc_rectangle class.
 */
#pragma once

#include <cmath>
#include <cstdint>
#include <utility>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/index_type.h"
#include "num_collect/util/assert.h"

namespace num_collect::opt::impl {

/*!
 * \brief Class of rectangles as proposed in \cite Sergeyev2000 for \ref
 * num_collect::opt::adaptive_diagonal_curves.
 *
 * \tparam Value Type of function values.
 * \tparam TernaryVector Type of ternary vectors.
 */
template <base::concepts::real_scalar Value, typename TernaryVector>
class adc_rectangle {
public:
    //! Type of function values.
    using value_type = Value;

    //! Type of ternary vectors.
    using ternary_vector_type = TernaryVector;

    /*!
     * \brief Constructor.
     *
     * \param[in] vertex A vertex with lower components.
     * \param[in] ave_value Average function value.
     */
    adc_rectangle(
        const ternary_vector_type& vertex, const value_type& ave_value)
        : vertex_(vertex), ave_value_(ave_value) {}

    /*!
     * \brief Get the vertex with lower first component.
     *
     * \return A vertex with lower first component.
     */
    [[nodiscard]] auto vertex() const -> const ternary_vector_type& {
        return vertex_;
    }

    /*!
     * \brief Get the average function value.
     *
     * \return Average function value.
     */
    [[nodiscard]] auto ave_value() const -> const value_type& {
        return ave_value_;
    }

    /*!
     * \brief Determine sampling points.
     *
     * \return Sampling points.
     */
    [[nodiscard]] auto sample_points() const
        -> std::pair<ternary_vector_type, ternary_vector_type> {
        return determine_sample_points(vertex_);
    }

    /*!
     * \brief Get the distance between center point and vertex.
     *
     * \return Distance between center point and vertex.
     */
    [[nodiscard]] auto dist() const -> value_type {
        auto squared_sum = static_cast<value_type>(0);
        for (index_type i = 0; i < vertex_.dim(); ++i) {
            using std::pow;
            squared_sum +=
                pow(static_cast<value_type>(3), -2 * (vertex_.digits(i) - 1));
        }
        using std::sqrt;
        const auto half = static_cast<value_type>(0.5);
        return half * sqrt(squared_sum);
    }

    /*!
     * \brief Determine sampling points.
     *
     * \param[in] lowest_vertex A vertex with lower first component.
     * \return Sampling points.
     */
    [[nodiscard]] static auto determine_sample_points(
        const ternary_vector_type& lowest_vertex)
        -> std::pair<ternary_vector_type, ternary_vector_type> {
        auto res = std::make_pair(lowest_vertex, lowest_vertex);
        const auto dim = lowest_vertex.dim();
        for (index_type i = 0; i < dim; ++i) {
            const auto digits = lowest_vertex.digits(i);
            NUM_COLLECT_DEBUG_ASSERT(digits > 0);
            std::uint_fast32_t one_count = 0;
            for (index_type j = 0; j < digits; ++j) {
                if (lowest_vertex(i, j) ==
                    typename ternary_vector_type::digit_type{1}) {
                    ++one_count;
                }
            }

            auto
                last_digit =  // NOLINTNEXTLINE(bugprone-signed-char-misuse,cert-str34-c): false positive
                static_cast<std::int_fast32_t>(lowest_vertex(i, digits - 1));
            ++last_digit;
            constexpr std::uint_fast32_t odd_mask = 1;
            if ((one_count & odd_mask) == odd_mask) {
                res.first(i, digits - 1) =
                    static_cast<typename ternary_vector_type::digit_type>(
                        last_digit);
            } else {
                res.second(i, digits - 1) =
                    static_cast<typename ternary_vector_type::digit_type>(
                        last_digit);
            }
        }
        normalize_point(res.first);
        normalize_point(res.second);
        return res;
    }

private:
    /*!
     * \brief Normalize point.
     *
     * \param[in,out] point Point.
     */
    static void normalize_point(ternary_vector_type& point) {
        for (index_type i = 0; i < point.dim(); ++i) {
            for (index_type j = point.digits(i) - 1; j > 0; --j) {
                if (point(i, j) ==
                    typename ternary_vector_type::digit_type{3}) {
                    point(i, j) = 0;
                    // NOLINTNEXTLINE(bugprone-signed-char-misuse,cert-str34-c): false positive
                    std::int_fast32_t temp = point(i, j - 1);
                    ++temp;
                    point(i, j - 1) =
                        static_cast<typename ternary_vector_type::digit_type>(
                            temp);
                }
            }
        }
    }

    //! A vertex with lower first component.
    ternary_vector_type vertex_;

    //! Average function value.
    value_type ave_value_;
};

}  // namespace num_collect::opt::impl
