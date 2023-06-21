/*
 * Copyright 2023 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of cuthill_mckee_ordering class.
 */
#pragma once

#include <limits>
#include <queue>
#include <set>
#include <utility>

#include <Eigen/Core>
#include <Eigen/src/Core/util/Constants.h>
#include <hash_tables/maps/open_address_map_st.h>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/exception.h"

namespace num_collect::linear {

namespace impl {

/*!
 * \brief Class of the implementation of Cuthill-McKee ordering method
 * \cite Golub2013, \cite Knabner2003.
 *
 * \tparam StorageIndex Type of indices in storages of indices.
 */
template <typename StorageIndex>
class cuthill_mckee_ordering_impl {
public:
    //! Type of indices in storages of indices.
    using storage_index_type = StorageIndex;

    //! Type of permutations.
    using permutation_type = Eigen::PermutationMatrix<Eigen::Dynamic,
        Eigen::Dynamic, storage_index_type>;

    //! Constructor.
    cuthill_mckee_ordering_impl() = default;

    /*!
     * \brief Create a permutation matrix.
     *
     * \tparam MatrixType Type of the input matrix.
     * \param[in] matrix Input matrix.
     * \param[out] permutation Permutation matrix.
     */
    template <base::concepts::sparse_matrix MatrixType>
    void operator()(const MatrixType& matrix, permutation_type& permutation) {
        if (matrix.rows() != matrix.cols()) {
            throw invalid_argument("Square matrix is required.");
        }

        const storage_index_type first_index = calculate_degrees(matrix);
        process_indices(matrix, first_index);
        create_permutation(permutation, matrix.rows());
    }

private:
    /*!
     * \brief Struct of data of processed indices.
     */
    struct processed_index_data {
    public:
        //! Index.
        storage_index_type index;

        //! Degree.
        storage_index_type degree;

        //! Level.
        storage_index_type level;
    };

    /*!
     * \brief Function object to compare processed_index_data_less objects.
     */
    struct processed_index_data_less {
    public:
        /*!
         * \brief Compare two objects.
         *
         * \param[in] left Left-hand-side object.
         * \param[in] right Right-hand-side object.
         * \return Whether left is less than right.
         */
        [[nodiscard]] auto operator()(const processed_index_data& left,
            const processed_index_data& right) const noexcept -> bool {
            if (left.level != right.level) {
                return left.level < right.level;
            }
            if (left.degree != right.degree) {
                return left.degree < right.degree;
            }
            return left.index < right.index;
        }
    };

    /*!
     * \brief Calculate degrees.
     *
     * \tparam MatrixType Type of the input matrix.
     * \param[in] matrix Input matrix.
     * \return Index with the lowest degree.
     */
    template <base::concepts::sparse_matrix MatrixType>
    [[nodiscard]] auto calculate_degrees(const MatrixType& matrix)
        -> storage_index_type {
        const auto size = static_cast<storage_index_type>(matrix.outerSize());
        unused_index_to_degree_.clear();
        unused_index_to_degree_.reserve(size);

        storage_index_type lowest_degree =
            std::numeric_limits<storage_index_type>::max();
        storage_index_type lowest_degree_index = 0;
        for (storage_index_type i = 0; i < size; ++i) {
            storage_index_type degree = 0;
            for (typename MatrixType::InnerIterator iter(matrix, i); iter;
                 ++iter) {
                ++degree;
            }
            unused_index_to_degree_.emplace(i, degree);
            if (degree < lowest_degree) {
                lowest_degree = degree;
                lowest_degree_index = i;
            }
        }

        return lowest_degree_index;
    }

    /*!
     * \brief Process indices.
     *
     * \tparam MatrixType Type of the input matrix.
     * \param[in] matrix Input matrix.
     * \param[in] first_index The first index to process.
     */
    template <base::concepts::sparse_matrix MatrixType>
    void process_indices(
        const MatrixType& matrix, storage_index_type first_index) {
        process_one_index(matrix, first_index, 0);

        while (!next_index_level_pairs_.empty()) {
            const auto& [index, level] = next_index_level_pairs_.front();
            process_one_index(matrix, index, level);
            next_index_level_pairs_.pop();
        }

        if (!unused_index_to_degree_.empty()) {
            throw algorithm_failure("Unused indices exist.");
        }
    }

    /*!
     * \brief Process an index.
     *
     * \tparam MatrixType Type of the input matrix.
     * \param[in] matrix Input matrix.
     * \param[in] index Index.
     * \param[in] level Level.
     */
    template <base::concepts::sparse_matrix MatrixType>
    void process_one_index(const MatrixType& matrix, storage_index_type index,
        storage_index_type level) {
        const auto* degree = unused_index_to_degree_.try_get(index);
        if (degree == nullptr) {
            return;
        }

        processed_indices_.emplace(processed_index_data{
            .index = index, .degree = *degree, .level = level});
        unused_index_to_degree_.erase(index);

        for (typename MatrixType::InnerIterator iter(matrix, index); iter;
             ++iter) {
            const storage_index_type next_index = iter.index();
            if (unused_index_to_degree_.has(next_index)) {
                next_index_level_pairs_.emplace(next_index, level + 1);
            }
        }
    }

    /*!
     * \brief Create a permutation matrix.
     *
     * \param[out] permutation Permutation matrix.
     * \param[in] size Size of the matrix.
     */
    void create_permutation(
        permutation_type& permutation, storage_index_type size) {
        permutation.resize(size);
        storage_index_type moved_index = 0;
        for (const processed_index_data& data : processed_indices_) {
            permutation.indices()(data.index) = moved_index;
            ++moved_index;
        }
    }

    //! Mapping of unused indices to degrees.
    hash_tables::maps::open_address_map_st<storage_index_type,
        storage_index_type>
        unused_index_to_degree_{};

    //! Processed indices.
    std::set<processed_index_data, processed_index_data_less>
        processed_indices_{};

    //! Indices processed next.
    std::queue<std::pair<storage_index_type, storage_index_type>>
        next_index_level_pairs_{};
};

}  // namespace impl

/*!
 * \brief Class to perform Cuthill-McKee ordering method
 * \cite Golub2013, \cite Knabner2003.
 */
template <typename StorageIndex>
class cuthill_mckee_ordering {
public:
    //! Type of indices in storages of indices.
    using storage_index_type = StorageIndex;

    //! Type of permutations.
    using permutation_type = Eigen::PermutationMatrix<Eigen::Dynamic,
        Eigen::Dynamic, storage_index_type>;

    //! Constructor.
    cuthill_mckee_ordering() = default;

    /*!
     * \brief Create a permutation matrix.
     *
     * \tparam MatrixType Type of the input matrix.
     * \param[in] matrix Input matrix.
     * \param[out] permutation Permutation matrix.
     */
    template <base::concepts::sparse_matrix MatrixType>
    void operator()(const MatrixType& matrix, permutation_type& permutation) {
        impl::cuthill_mckee_ordering_impl<storage_index_type>()(
            matrix, permutation);
    }
};

}  // namespace num_collect::linear
