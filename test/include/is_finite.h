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
 * \brief Test of is_finite function.
 */
#pragma once

#include <cmath>

#include <catch2/matchers/catch_matchers_templated.hpp>

/*!
 * \brief Class of matcher in Catch2 library to check finiteness of values.
 */
class is_finite_matcher : public Catch::Matchers::MatcherGenericBase {
public:
    /*!
     * \brief Check.
     *
     * \tparam Value Type of value.
     * \param[in] value Value.
     * \return Whether the value is finite.
     */
    template <typename Value>
    [[nodiscard]] auto match(const Value& value) const -> bool {
        return std::isfinite(value);
    }

    /*!
     * \brief Get description.
     *
     * \return Description.
     */
    [[nodiscard]] auto describe() const -> std::string override {
        return "Is finite";
    }
};

/*!
 * \brief Create matcher in Catch2 library to check finiteness of values.
 *
 * \return Matcher.
 */
inline auto is_finite() { return is_finite_matcher(); }
