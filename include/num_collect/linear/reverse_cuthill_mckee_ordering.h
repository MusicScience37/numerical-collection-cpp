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

#include "num_collect/base/concepts/sparse_matrix.h"
#include "num_collect/base/index_type.h"
#include "num_collect/linear/cuthill_mckee_ordering.h"

namespace num_collect::linear {

/*!
 * \brief Class to perform reverse Cuthill-McKee ordering method
 * \cite Golub2013, \cite Knabner2003.
 */
template <typename StorageIndex>
class reverse_cuthill_mckee_ordering {
public:
    //! Type of indices in storages of indices.
    using storage_index_type = StorageIndex;

    //! Type of permutations.
    using permutation_type = Eigen::PermutationMatrix<Eigen::Dynamic,
        Eigen::Dynamic, storage_index_type>;

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
        const storage_index_type size = permutation.indices().size();
        const storage_index_type size_minus_one = size - 1;
        for (storage_index_type& index : permutation.indices()) {
            index = size_minus_one - index;
        }
    }
};

}  // namespace num_collect::linear
