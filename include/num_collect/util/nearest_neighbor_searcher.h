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
 * \brief Definition of nearest_neighbor_searcher class.
 */
#pragma once

#include <cmath>
#include <cstddef>
#include <memory>
#include <utility>

#include <nanoflann.hpp>
#include <omp.h>

#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/concepts/real_scalar_dense_vector.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/vector.h"
#include "num_collect/util/vector_view.h"

namespace num_collect::util {

namespace impl {

/*!
 * \brief Class of adaptors in nanoflann library for
 * \ref num_collect::util::nearest_neighbor_searcher class.
 *
 * \tparam Point Type of points.
 */
template <typename Point>
    requires(concepts::real_scalar<Point> ||
        concepts::real_scalar_dense_vector<Point>)
class nearest_neighbor_searcher_nanoflann_adaptor;

/*!
 * \brief Class of adaptors in nanoflann library for
 * \ref num_collect::util::nearest_neighbor_searcher class.
 *
 * \tparam Point Type of points.
 */
template <concepts::real_scalar_dense_vector Point>
class nearest_neighbor_searcher_nanoflann_adaptor<Point> {
public:
    static_assert(
        Point::RowsAtCompileTime > 0, "Points must have a fixed size.");

    //! This type.
    using this_type = nearest_neighbor_searcher_nanoflann_adaptor<Point>;

    //! Type of scalar values. (Elements in points and distances.)
    using scalar_type = typename Point::Scalar;

    //! Dimension of points.
    static constexpr int dimension = Point::RowsAtCompileTime;

    //! Type of distances.
    using distance_type =
        typename nanoflann::metric_L2_Simple::template traits<scalar_type,
            this_type>::distance_t;

    //! Type of index in nanoflann.
    using nanoflann_index_type =
        nanoflann::KDTreeSingleIndexAdaptor<distance_type, this_type, dimension,
            index_type>;

    /*!
     * \brief Constructor.
     *
     * \param[in] points Points.
     */
    explicit nearest_neighbor_searcher_nanoflann_adaptor(
        vector_view<const Point> points)
        : points_(points) {
        if (points_.empty()) {
            throw invalid_argument("Points must not be empty.");
        }

        constexpr std::size_t leaf_max_size = 10;  // default value in nanoflann
        const auto num_threads =
            static_cast<unsigned int>(omp_get_max_threads());
        NUM_COLLECT_LOG_DEBUG(
            logging::logger(), "num_threads: {}", num_threads);

        index_ = std::make_unique<nanoflann_index_type>(dimension, *this,
            nanoflann::KDTreeSingleIndexAdaptorParams(leaf_max_size,
                nanoflann::KDTreeSingleIndexAdaptorFlags::None, num_threads));
    }

    /*!
     * \brief Get the index in nanoflann.
     *
     * \return Index in nanoflann.
     */
    [[nodiscard]] auto index() const -> const nanoflann_index_type& {
        NUM_COLLECT_DEBUG_ASSERT(index_ != nullptr);
        return *index_;
    }

    /*!
     * \brief Get the number of points.
     *
     * \return Number of points.
     *
     * \note This function is required in nanoflann.
     */
    [[nodiscard]] auto kdtree_get_point_count() const -> index_type {
        return points_.size();
    }

    /*!
     * \brief Get the value of a specific dimension of a specific point.
     *
     * \param[in] idx Index of the point.
     * \param[in] dim Dimension.
     * \return Value of the dimension of the point.
     *
     * \note This function is required in nanoflann.
     */
    [[nodiscard]] auto kdtree_get_pt(index_type idx, index_type dim) const
        -> scalar_type {
        NUM_COLLECT_DEBUG_ASSERT(idx >= 0);
        NUM_COLLECT_DEBUG_ASSERT(idx < points_.size());
        NUM_COLLECT_DEBUG_ASSERT(dim >= 0);
        NUM_COLLECT_DEBUG_ASSERT(dim < dimension);
        return points_[idx](dim);
    }

    /*!
     * \brief Get bounding box of points.
     *
     * \tparam BBOX Type of bounding box.
     * \return False always in this implementation.
     *
     * \note This function is required in nanoflann.
     */
    template <typename BBOX>
    [[nodiscard]] auto kdtree_get_bbox(BBOX& /*bb*/) const -> bool {
        return false;
    }

private:
    //! Points.
    vector_view<const Point> points_;

    //! Index in nanoflann.
    std::unique_ptr<nanoflann_index_type> index_;
};

}  // namespace impl

/*!
 * \brief Class of nearest neighbor search.
 *
 * \tparam Point Type of points.
 *
 * This class wraps nanoflann library to implement nearest neighbor search
 * using Euclidean distance.
 */
template <concepts::real_scalar_dense_vector Point>
class nearest_neighbor_searcher {
public:
    //! Type of scalar values. (Elements in points and distances.)
    using scalar_type = typename Point::Scalar;

    /*!
     * \brief Constructor.
     *
     * \param[in] points Points.
     */
    explicit nearest_neighbor_searcher(vector_view<const Point> points)
        : adaptor_(points) {}

    /*!
     * \brief Find k nearest neighbors of a query point.
     *
     * \param[in] num_neighbors Number of neighbors to find.
     * \param[in] query_point Query point.
     * \param[out] indices_and_distances Output vector of indices and distances.
     */
    void find_k_nearest_neighbors(index_type num_neighbors,
        const Point& query_point,
        vector<std::pair<index_type, scalar_type>>& indices_and_distances)
        const {
        // TODO get rid of temporary vectors
        vector<index_type> temp_indices(num_neighbors);
        vector<scalar_type> temp_squared_distances(num_neighbors);
        const auto num_found =
            static_cast<index_type>(adaptor_.index().knnSearch(
                query_point.data(), static_cast<std::size_t>(num_neighbors),
                temp_indices.data(), temp_squared_distances.data()));
        indices_and_distances.clear();
        indices_and_distances.reserve(num_found);
        for (index_type i = 0; i < num_found; ++i) {
            indices_and_distances.emplace_back(
                temp_indices[i], std::sqrt(temp_squared_distances[i]));
        }
    }

    /*!
     * \brief Find neighbors within a radius of a query point.
     *
     * \param[in] radius Radius.
     * \param[in] query_point Query point.
     * \param[out] indices_and_distances Output vector of indices and distances.
     */
    void find_neighbors_within_radius(scalar_type radius,
        const Point& query_point,
        vector<std::pair<index_type, scalar_type>>& indices_and_distances)
        const {
        const scalar_type squared_radius = radius * radius;
        // TODO get rid of temporary vectors
        std::vector<nanoflann::ResultItem<index_type, scalar_type>>
            temp_indices_and_distances;
        const std::size_t num_found = adaptor_.index().radiusSearch(
            query_point.data(), squared_radius, temp_indices_and_distances);
        indices_and_distances.clear();
        indices_and_distances.reserve(static_cast<index_type>(num_found));
        for (std::size_t i = 0; i < num_found; ++i) {
            indices_and_distances.emplace_back(
                temp_indices_and_distances[i].first,
                std::sqrt(temp_indices_and_distances[i].second));
        }
    }

private:
    //! Adaptor in nanoflann library.
    impl::nearest_neighbor_searcher_nanoflann_adaptor<Point> adaptor_;
};

}  // namespace num_collect::util
