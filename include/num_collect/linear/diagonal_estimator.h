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
 * \brief Definition of diagonal_estimator class.
 */
#pragma once

#include <concepts>
#include <random>

#include <Eigen/Core>

#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/util/vector.h"

namespace num_collect::linear {

/*!
 * \brief Class to estimate diagonal elements of matrices \cite Bekas2007.
 *
 * \tparam Vector Type of vectors.
 *
 * \note This class works only with functions to multiply square matrices.
 */
template <base::concepts::real_scalar_dense_vector Vector>
class diagonal_estimator {
public:
    //! Type of vectors.
    using vector_type = Vector;

    //! Type of scalars.
    using scalar_type = typename vector_type::Scalar;

    /*!
     * \brief Constructor.
     */
    diagonal_estimator() = default;

    /*!
     * \brief Allocate internal resources and prepare random vectors for
     * estimation.
     *
     * \param[in] size Size of vectors.
     *
     * \note This function does nothing if the random vectors with the given
     * size are already prepared.
     */
    void prepare(index_type size) {
        if (random_vectors_.size() == num_random_vectors_ &&
            random_vectors_[0].size() == size) {
            return;
        }

        multiplication_buffer_.resize(size);

        random_vectors_.resize(num_random_vectors_);
        for (index_type j = 0; j < num_random_vectors_; ++j) {
            random_vectors_[j].resize(size);
            for (index_type i = 0; i < size; ++i) {
                random_vectors_[j](i) = (random_engine_() == 0)
                    ? static_cast<scalar_type>(1)
                    : static_cast<scalar_type>(-1);
            }
        }
    }

    /*!
     * \brief Estimate diagonal elements of a matrix.
     *
     * \tparam CoeffFunction Type of the function to multiply the matrix.
     * \param[in] coeff_function Function to multiply the matrix.
     * \param[in] size Size of vectors.
     * \param[out] diagonals Estimated diagonal elements.
     *
     * coeff_function is assumed to have a signature like:
     *
     * ~~~{.cpp}
     * void coeff_function(const variable_type& target, variable_type& result);
     * ~~~
     */
    template <std::invocable<const vector_type&, vector_type&> CoeffFunction>
    void estimate(CoeffFunction&& coeff_function, index_type size,
        vector_type& diagonals) {
        prepare(size);
        diagonals.resize(size);
        diagonals.setZero();
        for (index_type i = 0; i < num_random_vectors_; ++i) {
            coeff_function(random_vectors_[i], multiplication_buffer_);
            diagonals +=
                multiplication_buffer_.cwiseProduct(random_vectors_[i]);
        }
        diagonals /= static_cast<scalar_type>(num_random_vectors_);
    }

    /*!
     * \brief Change the seed of random numbers.
     *
     * \tparam SeedType Type of the seed usable in `std::mt19937`.
     * \param[in] seed Seed.
     */
    template <typename SeedType>
    void seed(SeedType&& seed) {
        random_engine_.seed(std::forward<SeedType>(seed));
    }

    /*!
     * \brief Set the number of random vectors for estimation.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto num_random_vectors(index_type val) -> diagonal_estimator& {
        NUM_COLLECT_PRECONDITION(
            val > 0, "num_random_vectors must be a positive integer.");
        num_random_vectors_ = val;
        return *this;
    }

private:
    //! List of random vectors.
    util::vector<vector_type> random_vectors_{};

    // Note:
    // random_vectors_ is not a matrix because if random_vectors_ is a matrix,
    //     coeff_function(random_vectors_.col(i), multiplication_buffer_)
    // requires inefficient copying with memory allocation.

    //! Buffer of the result of multiplying matrices and random vectors.
    vector_type multiplication_buffer_{};

    //! Engine of random numbers.
    std::independent_bits_engine<std::mt19937, 1, unsigned> random_engine_{};

    //! Default number of random vectors.
    static constexpr index_type default_num_random_vectors = 20;

    //! Number of random vectors.
    index_type num_random_vectors_{default_num_random_vectors};
};

}  // namespace num_collect::linear
