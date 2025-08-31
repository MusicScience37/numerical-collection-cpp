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
 * \brief Definition of polynomial_term_generator class.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/rbf/polynomial_term.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/combination.h"
#include "num_collect/util/vector.h"

namespace num_collect::rbf {

/*!
 * \brief Class to generate terms of polynomials used in RBF interpolation.
 *
 * \tparam NumDimensions Number of dimensions of variables.
 */
template <int NumDimensions>
    requires(NumDimensions >= 1)
class polynomial_term_generator {
public:
    //! Type of degrees.
    using degree_type = int;

    //! Type of the vector of degrees.
    using degree_vector_type = Eigen::Vector<degree_type, NumDimensions>;

    /*!
     * \brief Constructor.
     *
     * \param[in] max_degree Maximum degree of polynomial terms.
     */
    explicit polynomial_term_generator(int max_degree)
        : max_degree_(max_degree) {
        if (max_degree_ < 0) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "Maximum degree of polynomials must be at least zero.");
        }
        generate();
    }

    /*!
     * \brief Get the polynomial terms.
     *
     * \return Polynomial terms.
     */
    [[nodiscard]] auto terms() const noexcept
        -> const util::vector<polynomial_term<NumDimensions>>& {
        return terms_;
    }

private:
    /*!
     * \brief Generate polynomial terms.
     */
    void generate() {
        const index_type num_terms = util::combination<index_type>(
            NumDimensions + max_degree_, max_degree_);
        terms_.reserve(num_terms);

        degree_vector_type current_degrees = degree_vector_type::Zero();
        degree_vector_type current_max_degrees =
            degree_vector_type::Constant(max_degree_);
        while (true) {
            terms_.emplace_back(current_degrees);

            index_type degree_index = NumDimensions - 1;
            for (; degree_index >= 0; --degree_index) {
                if (current_degrees(degree_index) <
                    current_max_degrees(degree_index)) {
                    ++current_degrees(degree_index);
                    break;
                }
            }
            if (degree_index < 0) {
                // All terms are generated.
                break;
            }
            ++degree_index;
            for (; degree_index < NumDimensions; ++degree_index) {
                current_max_degrees(degree_index) =
                    current_max_degrees(degree_index - 1) -
                    current_degrees(degree_index - 1);
                current_degrees(degree_index) = 0;
            }
        }

        NUM_COLLECT_ASSERT(terms_.size() == num_terms);
    }

    //! Maximum degree.
    int max_degree_;

    //! Polynomial terms.
    util::vector<polynomial_term<NumDimensions>> terms_;
};

}  // namespace num_collect::rbf
