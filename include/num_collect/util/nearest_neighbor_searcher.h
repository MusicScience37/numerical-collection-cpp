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

#include <algorithm>
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
    requires(base::concepts::real_scalar<Point> ||
        base::concepts::real_scalar_dense_vector<Point>)
class nearest_neighbor_searcher_nanoflann_adaptor;

/*!
 * \brief Class of adaptors in nanoflann library for
 * \ref num_collect::util::nearest_neighbor_searcher class.
 * (1D points.)
 *
 * \tparam Point Type of points.
 */
template <base::concepts::real_scalar Point>
class nearest_neighbor_searcher_nanoflann_adaptor<Point> {
public:
    //! This type.
    using this_type = nearest_neighbor_searcher_nanoflann_adaptor<Point>;

    //! Type of scalar values. (Elements in points and distances.)
    using scalar_type = Point;

    //! Dimension of points.
    static constexpr int dimension = 1;

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
        NUM_COLLECT_DEBUG_ASSERT(dim == 0);
        return points_[idx];
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

/*!
 * \brief Class of adaptors in nanoflann library for
 * \ref num_collect::util::nearest_neighbor_searcher class.
 *
 * \tparam Point Type of points.
 */
template <base::concepts::real_scalar_dense_vector Point>
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

/*!
 * \brief Class to replace `nanoflann::KNNResultSet` in nanoflann library
 * with types in num_collect library.
 *
 * \tparam Scalar Type of scalar values. (Elements in points and distances.)
 */
template <base::concepts::real_scalar Scalar>
class nearest_neighbor_searcher_knn_result_set {
public:
    //! Type of scalar values. (Elements in points and distances.)
    using scalar_type = Scalar;

    //! Type of distances. (Type required in nanoflann.)
    using DistanceType = scalar_type;  // NOLINT

    //! Type of indices. (Type required in nanoflann.)
    using IndexType = index_type;  // NOLINT

    //! Type of sizes. (Type required in nanoflann.)
    using CountType = index_type;  // NOLINT

    /*!
     * \brief Constructor.
     *
     * \param[out] indices_and_distances Buffer of indices and distances.
     * \param[in] max_num_neighbors Maximum number of neighbors.
     */
    nearest_neighbor_searcher_knn_result_set(
        vector<std::pair<index_type, scalar_type>>& indices_and_distances,
        index_type max_num_neighbors)
        : indices_and_distances_(indices_and_distances),
          max_num_neighbors_(max_num_neighbors) {
        NUM_COLLECT_DEBUG_ASSERT(max_num_neighbors_ > 0);
        indices_and_distances_.clear();
        indices_and_distances_.reserve(max_num_neighbors_ + 1);
    }

    /*!
     * \brief Get the current number of neighbors.
     *
     * \return Current number of neighbors.
     *
     * \note This function is required in nanoflann.
     */
    [[nodiscard]] auto size() const -> CountType {
        return indices_and_distances_.size();
    }

    /*!
     * \brief Check whether this set is empty.
     *
     * \retval true This set is empty.
     * \retval false This set is not empty.
     *
     * \note This function is required in nanoflann.
     */
    [[nodiscard]] auto empty() const -> bool {
        return indices_and_distances_.empty();
    }

    /*!
     * \brief Check whether this set is full.
     *
     * \retval true This set is full.
     * \retval false This set is not full.
     *
     * \note This function is required in nanoflann.
     */
    [[nodiscard]] auto full() const -> bool {
        return size() == max_num_neighbors_;
    }

    /*!
     * \brief Add a point.
     *
     * \param[in] dist Distance.
     * \param[in] index Index of the point.
     * \retval true Continue adding points.
     * \retval false Stop adding points.
     *
     * \note This function is required in nanoflann.
     */
    auto addPoint(  // NOLINT(*-identifier-naming)
        DistanceType dist, IndexType index) -> bool {
        auto iter = std::lower_bound(indices_and_distances_.begin(),
            indices_and_distances_.end(), dist,
            [](const auto& pair, scalar_type value) {
                return pair.second < value;
            });
        indices_and_distances_.emplace(iter, index, dist);
        if (indices_and_distances_.size() > max_num_neighbors_) {
            indices_and_distances_.pop_back();
        }
        return true;
    }

    /*!
     * \brief Get the maximum distance allowed to add a point.
     *
     * \return Maximum distance allowed to add a point.
     */
    auto worstDist() const -> DistanceType {  // NOLINT(*-identifier-naming)
        if (!full()) {
            return std::numeric_limits<DistanceType>::max();
        }
        return indices_and_distances_.back().second;
    }

    /*!
     * \brief Sort the points.
     */
    void sort() {
        // Already sorted in addPoint.
    }

private:
    //! Buffer of indices and distances.
    vector<std::pair<index_type, scalar_type>>& indices_and_distances_;

    //! Maximum number of neighbors.
    index_type max_num_neighbors_;
};

/*!
 * \brief Class to replace `nanoflann::RadiusResultSet` in nanoflann library
 * with types in num_collect library.
 *
 * \tparam Scalar Type of scalar values. (Elements in points and distances.)
 */
template <base::concepts::real_scalar Scalar>
class nearest_neighbor_searcher_radius_result_set {
public:
    //! Type of scalar values. (Elements in points and distances.)
    using scalar_type = Scalar;

    //! Type of distances. (Type required in nanoflann.)
    using DistanceType = scalar_type;  // NOLINT

    //! Type of indices. (Type required in nanoflann.)
    using IndexType = index_type;  // NOLINT

    //! Type of sizes. (Type required in nanoflann.)
    using CountType = index_type;  // NOLINT

    /*!
     * \brief Constructor.
     *
     * \param[out] indices_and_distances Buffer of indices and distances.
     * \param[in] radius Radius.
     */
    explicit nearest_neighbor_searcher_radius_result_set(
        vector<std::pair<index_type, scalar_type>>& indices_and_distances,
        DistanceType radius)
        : indices_and_distances_(indices_and_distances), radius_(radius) {
        indices_and_distances_.clear();
    }

    /*!
     * \brief Get the current number of neighbors.
     *
     * \return Current number of neighbors.
     *
     * \note This function is required in nanoflann.
     */
    [[nodiscard]] auto size() const -> CountType {
        return indices_and_distances_.size();
    }

    /*!
     * \brief Check whether this set is empty.
     *
     * \retval true This set is empty.
     * \retval false This set is not empty.
     *
     * \note This function is required in nanoflann.
     */
    [[nodiscard]] auto empty() const -> bool {
        return indices_and_distances_.empty();
    }

    /*!
     * \brief Check whether this set is full.
     *
     * \return true always in this implementation.
     *
     * \note This function is required in nanoflann.
     */
    [[nodiscard]] auto full() const -> bool { return true; }

    /*!
     * \brief Add a point.
     *
     * \param[in] dist Distance.
     * \param[in] index Index of the point.
     * \retval true Continue adding points.
     * \retval false Stop adding points.
     *
     * \note This function is required in nanoflann.
     */
    auto addPoint(  // NOLINT(*-identifier-naming)
        DistanceType dist, IndexType index) -> bool {
        if (dist < radius_) {
            indices_and_distances_.emplace_back(index, dist);
        }
        return true;
    }

    /*!
     * \brief Get the maximum distance allowed to add a point.
     *
     * \return Maximum distance allowed to add a point.
     */
    auto worstDist() const -> DistanceType {  // NOLINT(*-identifier-naming)
        return radius_;
    }

    /*!
     * \brief Sort the points.
     */
    void sort() {
        std::ranges::sort(indices_and_distances_.begin(),
            indices_and_distances_.end(), [](const auto& lhs, const auto& rhs) {
                return lhs.second < rhs.second;
            });
    }

private:
    //! Buffer of indices and distances.
    vector<std::pair<index_type, scalar_type>>& indices_and_distances_;

    //! Radius.
    DistanceType radius_;
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
template <typename Point>
    requires(base::concepts::real_scalar<Point> ||
        base::concepts::real_scalar_dense_vector<Point>)
class nearest_neighbor_searcher {
public:
    //! Type of scalar values. (Elements in points and distances.)
    using scalar_type =
        typename impl::nearest_neighbor_searcher_nanoflann_adaptor<
            Point>::scalar_type;

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
        impl::nearest_neighbor_searcher_knn_result_set<scalar_type> result_set(
            indices_and_distances, num_neighbors);
        adaptor_.index().findNeighbors(
            result_set, to_data_pointer(query_point));

        // Convert squared distances to distances.
        for (auto& pair : indices_and_distances) {
            pair.second = std::sqrt(pair.second);
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
        impl::nearest_neighbor_searcher_radius_result_set<scalar_type>
            result_set(indices_and_distances, squared_radius);
        adaptor_.index().findNeighbors(
            result_set, to_data_pointer(query_point));
        result_set.sort();

        // Convert squared distances to distances.
        for (auto& pair : indices_and_distances) {
            pair.second = std::sqrt(pair.second);
        }
    }

private:
    /*!
     * \brief Convert a query point to the pointer to the first element.
     *
     * \param[in] query_point Query point.
     * \return Pointer to the first element of the query point.
     */
    static auto to_data_pointer(const Point& query_point) noexcept
        -> const scalar_type* {
        if constexpr (base::concepts::real_scalar<Point>) {
            return &query_point;
        } else {
            return query_point.data();
        }
    }

    //! Adaptor in nanoflann library.
    impl::nearest_neighbor_searcher_nanoflann_adaptor<Point> adaptor_;
};

}  // namespace num_collect::util
