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
 * \brief Definition of quad_approx function.
 */
#pragma once

#include <catch2/matchers/catch_matchers_templated.hpp>
#include <fmt/format.h>

#include "format_quad_for_test.h"
#include "num_collect/multi_double/quad.h"
#include "num_collect/multi_double/quad_math.h"

/*!
 * \brief Class of matcher in Catch2 library to check approximate equality of
 * quad numbers using relative errors.
 */
class quad_within_rel_matcher : public Catch::Matchers::MatcherGenericBase {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] expected Expected value.
     * \param[in] tolerance Tolerance of relative error.
     */
    quad_within_rel_matcher(num_collect::multi_double::quad expected,
        num_collect::multi_double::quad tolerance)
        : expected_(expected), tolerance_(tolerance) {}

    /*!
     * \brief Check whether two numbers are approximately equal.
     *
     * \param[in] actual Actual number.
     * \retval true Two numbers are approximately equal.
     * \retval false Two numbers are not approximately equal.
     */
    [[nodiscard]] auto match(
        const num_collect::multi_double::quad& actual) const -> bool {
        const auto error = abs(actual - expected_);
        const auto absolute_tolerance = abs(expected_) * tolerance_;
        return error <= absolute_tolerance;
    }

    /*!
     * \brief Get description.
     *
     * \return Description.
     */
    [[nodiscard]] auto describe() const -> std::string override {
        return fmt::format(
            "is approximately equal to {} with relative error {}",
            format_quad_for_test(expected_), tolerance_.high());
    }

private:
    //! Expected value.
    num_collect::multi_double::quad expected_;

    //! Tolerance of relative error.
    num_collect::multi_double::quad tolerance_;
};

/*!
 * \brief Create a matcher to check approximate equality of quad numbers using
 * relative errors.
 *
 * \param[in] expected Expected value.
 * \param[in] tolerance Tolerance of relative error.
 * \return Matcher.
 */
inline auto quad_within_rel(num_collect::multi_double::quad expected,
    num_collect::multi_double::quad tolerance) {
    return quad_within_rel_matcher{expected, tolerance};
}
