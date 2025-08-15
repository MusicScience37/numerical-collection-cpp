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
 * \brief Definition of index_score_table class.
 */
#pragma once

#include <concepts>
#include <map>
#include <optional>

#include "num_collect/base/index_type.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/vector.h"

namespace num_collect::linear::impl::amg {

/*!
 * \brief Class of tables for storing pairs of indices and their corresponding
 * scores.
 *
 * \note This class is specialized for \ref build_first_coarse_grid_candidate
 * function, and used only internally.
 */
template <std::signed_integral StorageIndex>
class index_score_table {
public:
    //! Type of scores.
    using score_type = StorageIndex;

    /*!
     * \brief Constructor.
     *
     * \param[in] size Number of indices.
     */
    explicit index_score_table(index_type size)
        : index_to_iterator_map_(size, std::nullopt) {}

    /*!
     * \brief Assign a pair of an index and its score.
     *
     * \param[in] index Index.
     * \param[in] score Score.
     */
    void assign(index_type index, score_type score) {
        NUM_COLLECT_DEBUG_ASSERT(index >= 0);
        NUM_COLLECT_DEBUG_ASSERT(index < index_to_iterator_map_.size());
        NUM_COLLECT_DEBUG_ASSERT(index_to_iterator_map_[index] == std::nullopt);

        const auto iterator = score_to_index_map_.emplace(score, index);
        index_to_iterator_map_[index] = iterator;
    }

    /*!
     * \brief Check whether this table is empty.
     *
     * \retval true This table is empty.
     * \retval false This table is not empty.
     */
    [[nodiscard]] auto empty() const noexcept -> bool {
        return score_to_index_map_.empty();
    }

    /*!
     * \brief Find the index of the current maximum score.
     *
     * \return Index.
     */
    [[nodiscard]] auto find_max_score_index() const -> index_type {
        NUM_COLLECT_DEBUG_ASSERT(!score_to_index_map_.empty());
        return score_to_index_map_.begin()->second;
    }

    /*!
     * \brief Remove a pair with an index.
     *
     * \param[in] index Index.
     */
    void remove(index_type index) {
        NUM_COLLECT_DEBUG_ASSERT(index >= 0);
        NUM_COLLECT_DEBUG_ASSERT(index < index_to_iterator_map_.size());

        if (!index_to_iterator_map_[index]) {
            return;
        }
        auto iterator = *index_to_iterator_map_[index];
        score_to_index_map_.erase(iterator);
        index_to_iterator_map_[index].reset();
    }

    /*!
     * \brief Add a value to the score.
     *
     * \param[in] index Index.
     * \param[in] added_value Value added to the score.
     */
    void add_score(index_type index, score_type added_value) {
        NUM_COLLECT_DEBUG_ASSERT(index >= 0);
        NUM_COLLECT_DEBUG_ASSERT(index < index_to_iterator_map_.size());

        if (!index_to_iterator_map_[index]) {
            return;
        }
        auto iterator = *index_to_iterator_map_[index];
        auto node = score_to_index_map_.extract(iterator);
        node.key() += added_value;
        iterator = score_to_index_map_.insert(std::move(node));
        index_to_iterator_map_[index] = iterator;
    }

    /*!
     * \brief Get the mapping from scores to indices.
     *
     * \return Mapping from scores to indices.
     *
     * \note This function is for unit tests.
     */
    [[nodiscard]] auto score_to_index_map() const noexcept -> const
        std::multimap<score_type, index_type, std::greater<score_type>>& {
        return score_to_index_map_;
    }

private:
    //! Mapping from scores to indices.
    std::multimap<score_type, index_type, std::greater<score_type>>
        score_to_index_map_;

    //! Mapping from indices to the iterators in score_to_index_map_.
    util::vector<
        std::optional<typename std::multimap<score_type, index_type>::iterator>>
        index_to_iterator_map_;
};

}  // namespace num_collect::linear::impl::amg
