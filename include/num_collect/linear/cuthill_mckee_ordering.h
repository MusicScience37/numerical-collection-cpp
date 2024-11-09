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

#include <cstddef>
#include <limits>
#include <set>
#include <vector>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <hash_tables/maps/open_address_map_st.h>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/exception.h"
#include "num_collect/logging/logging_macros.h"

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
            NUM_COLLECT_LOG_AND_THROW(
                invalid_argument, "Square matrix is required.");
        }

        const storage_index_type first_index = calculate_degrees(matrix);
        process_indices(matrix, first_index);
        create_permutation(
            permutation, static_cast<storage_index_type>(matrix.rows()));
    }

private:
    /*!
     * \brief Struct of data of indices processed next.
     */
    struct next_index_data {
    public:
        //! Index.
        storage_index_type index;

        //! Degree.
        storage_index_type degree;

        //! Order in the previous level.
        storage_index_type previous_level_order;
    };

    /*!
     * \brief Function object to compare next_index_data objects.
     */
    struct next_index_data_less {
    public:
        /*!
         * \brief Compare two objects.
         *
         * \param[in] left Left-hand-side object.
         * \param[in] right Right-hand-side object.
         * \return Whether left is less than right.
         */
        [[nodiscard]] auto operator()(const next_index_data& left,
            const next_index_data& right) const noexcept -> bool {
            if (left.previous_level_order != right.previous_level_order) {
                return left.previous_level_order < right.previous_level_order;
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
        unused_index_to_degree_.reserve(static_cast<std::size_t>(size));

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
        processed_indices_.clear();
        processed_indices_.reserve(static_cast<std::size_t>(matrix.rows()));
        next_indices_.clear();
        next_indices_.emplace(next_index_data{
            .index = first_index, .degree = 0, .previous_level_order = 0});

        while (!next_indices_.empty()) {
            std::swap(current_indices_, next_indices_);
            next_indices_.clear();

            // Remove from unused indices before checking adjacent indices.
            // Also, duplicated indices are removed.
            for (auto iter = current_indices_.begin();
                iter != current_indices_.end();) {
                if (unused_index_to_degree_.erase(iter->index)) {
                    ++iter;
                } else {
                    iter = current_indices_.erase(iter);
                }
            }

            // Now add indices to processed indices, and search for indices in
            // the next level.
            storage_index_type order = 0;
            for (const next_index_data& data : current_indices_) {
                processed_indices_.push_back(data.index);

                for (typename MatrixType::InnerIterator iter(
                         matrix, data.index);
                    iter; ++iter) {
                    const storage_index_type* degree =
                        unused_index_to_degree_.try_get(iter.index());
                    if (degree == nullptr) {
                        continue;
                    }
                    next_indices_.emplace(next_index_data{.index = iter.index(),
                        .degree = *degree,
                        .previous_level_order = order});
                }

                ++order;
            }
        }

        if (!unused_index_to_degree_.empty()) {
            NUM_COLLECT_LOG_AND_THROW(
                algorithm_failure, "Unused indices exist.");
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
        for (const storage_index_type& index : processed_indices_) {
            permutation.indices()(index) = moved_index;
            ++moved_index;
        }
    }

    //! Mapping of unused indices to degrees.
    hash_tables::maps::open_address_map_st<storage_index_type,
        storage_index_type>
        unused_index_to_degree_{};

    //! Indices processed in the current level.
    std::set<next_index_data, next_index_data_less> current_indices_{};

    //! Indices processed in the next level.
    std::set<next_index_data, next_index_data_less> next_indices_{};

    //! Processed indices.
    std::vector<storage_index_type> processed_indices_{};
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

    //! Type of permutations (for Eigen library).
    using PermutationType = permutation_type;

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

    /*!
     * \brief Create a permutation matrix from SparseSelfAdjointView object.
     *
     * \tparam MatrixType Type of the input matrix.
     * \tparam Mode Mode of the input matrix.
     * \param[in] matrix Input matrix.
     * \param[in] permutation Permutation matrix.
     */
    template <typename MatrixType, unsigned int Mode>
    void operator()(
        const Eigen::SparseSelfAdjointView<MatrixType, Mode>& matrix,
        permutation_type& permutation) {
        Eigen::SparseMatrix<typename MatrixType::Scalar, Eigen::ColMajor,
            storage_index_type>
            matrix_as_ordinary_matrix = matrix;
        operator()(matrix_as_ordinary_matrix, permutation);
    }
};

}  // namespace num_collect::linear
