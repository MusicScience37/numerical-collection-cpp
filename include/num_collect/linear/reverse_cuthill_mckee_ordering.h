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
 * \brief Definition of reverse_cuthill_mckee_ordering class.
 */
#pragma once

#include <Eigen/Core>
#include <Eigen/SparseCore>

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/linear/cuthill_mckee_ordering.h"

namespace num_collect::linear {

/*!
 * \brief Class to perform reverse Cuthill-McKee ordering method
 * \cite Golub2013, \cite Knabner2003.
 *
 * \tparam StorageIndex Type of indices in storages of indices.
 *
 * This class assumes that the coefficient matrix is symmetric.
 */
template <typename StorageIndex>
class reverse_cuthill_mckee_ordering {
public:
    //! Type of indices in storages of indices.
    using storage_index_type = StorageIndex;

    //! Type of permutations.
    using permutation_type = Eigen::PermutationMatrix<Eigen::Dynamic,
        Eigen::Dynamic, storage_index_type>;

    //! Type of permutations (for Eigen library).
    using PermutationType = permutation_type;

    //! Constructor.
    reverse_cuthill_mckee_ordering() = default;

    /*!
     * \brief Create a permutation matrix.
     *
     * \tparam MatrixType Type of the input matrix.
     * \param[in] matrix Input matrix.
     * \param[out] permutation Permutation matrix.
     */
    template <base::concepts::sparse_matrix MatrixType>
    void operator()(const MatrixType& matrix, permutation_type& permutation) {
        cuthill_mckee_ordering<storage_index_type>()(matrix, permutation);
        const auto size =
            static_cast<storage_index_type>(permutation.indices().size());
        const storage_index_type size_minus_one = size - 1;
        for (storage_index_type& index : permutation.indices()) {
            index = size_minus_one - index;
        }
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
