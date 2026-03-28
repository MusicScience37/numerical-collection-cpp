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
 * \brief Definition of rbf_fd_assembler class.
 */
#pragma once

#include <Eigen/SparseCore>

#include "num_collect/base/index_type.h"
#include "num_collect/base/precondition.h"
#include "num_collect/rbf/distance_functions/euclidean_distance_function.h"
#include "num_collect/rbf/impl/rbf_fd_row_calculator.h"
#include "num_collect/rbf/length_parameter_calculators/global_length_parameter_calculator.h"
#include "num_collect/rbf/rbfs/gaussian_rbf.h"
#include "num_collect/util/nearest_neighbor_searcher.h"
#include "num_collect/util/vector_view.h"

namespace num_collect::rbf {

/*!
 * \brief Class to assemble system matrices in RBF-FD method.
 *
 * \tparam Variable Type of variables.
 * \tparam RBF Type of the RBF.
 * \tparam DistanceFunction Type of the distance function.
 */
template <typename Variable,
    concepts::rbf RBF = rbfs::gaussian_rbf<typename distance_functions::
            euclidean_distance_function<Variable>::value_type>,
    concepts::distance_function DistanceFunction =
        distance_functions::euclidean_distance_function<Variable>,
    concepts::length_parameter_calculator LengthParameterCalculator =
        length_parameter_calculators::global_length_parameter_calculator<
            DistanceFunction>>
    requires std::is_same_v<Variable,
                 typename DistanceFunction::variable_type> &&
    std::is_same_v<typename DistanceFunction::value_type,
        typename RBF::scalar_type> &&
    std::is_same_v<DistanceFunction,
        typename LengthParameterCalculator::distance_function_type>
class rbf_fd_assembler {
public:
    //! Type of variables.
    using variable_type = Variable;

    //! Type of the RBF.
    using rbf_type = RBF;

    //! Type of the distance function.
    using distance_function_type = DistanceFunction;

    //! Type of the calculator of length parameters.
    using length_parameter_calculator_type = LengthParameterCalculator;

    //! Type of scalars.
    using scalar_type = typename rbf_type::scalar_type;

    /*!
     * \brief Constructor.
     *
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     */
    explicit rbf_fd_assembler(
        distance_function_type distance_function = distance_function_type(),
        rbf_type rbf = rbf_type())
        : distance_function_(std::move(distance_function)),
          rbf_(std::move(rbf)) {}

    /*!
     * \brief Compute rows of the system matrix.
     *
     * \tparam Operator Type of the operator to assemble.
     * \tparam StorageIndex Type of indices in the system matrix.
     * \param[in] row_variables Variables corresponding to rows of the system
     * matrix.
     * \param[in] column_variables Variables corresponding to the columns to
     * calculate.
     * \param[in] column_variables_nearest_neighbor_searcher Nearest neighbor
     * searcher for the column variables.
     * \param[out] triplets Output vector of triplets to set values in the
     * system matrix.
     * \param[in] row_offset Offset of row indices in the system matrix.
     * \param[in] column_offset Offset of column indices in the system matrix.
     *
     * \note Existing triplets are not cleared in this function
     * because this function will be called multiple times to assemble the whole
     * system matrix.
     */
    template <typename Operator, typename StorageIndex>
    void compute_rows(util::vector_view<const variable_type> row_variables,
        util::vector_view<const variable_type> column_variables,
        const util::nearest_neighbor_searcher<variable_type>&
            column_variables_nearest_neighbor_searcher,
        util::vector<Eigen::Triplet<scalar_type, StorageIndex>>& triplets,
        index_type row_offset, index_type column_offset) {
        NUM_COLLECT_PRECONDITION(column_variables.size() >= num_neighbors_,
            "Number of column variables must be greater than or equal to the "
            "number of neighbors.");

        triplets.reserve(
            triplets.size() + row_variables.size() * num_neighbors_);

        // TODO Parallelization.
        row_calculator_type row_calculator;
        row_calculator.length_parameter_scale(length_parameter_scale_);
        for (index_type i = 0; i < row_variables.size(); ++i) {
            const auto target_operator = Operator{row_variables[i]};
            row_calculator.compute_row(distance_function_, rbf_,
                target_operator, row_variables[i], column_variables,
                column_variables_nearest_neighbor_searcher, num_neighbors_,
                triplets, row_offset + i, column_offset);
        }
    }

    /*!
     * \brief Get the number of neighbors to use in RBF-FD.
     *
     * \return Number of neighbors to use in RBF-FD.
     */
    [[nodiscard]] auto num_neighbors() const -> index_type {
        return num_neighbors_;
    }

    /*!
     * \brief Set the number of neighbors to use in RBF-FD.
     *
     * \param[in] value Value.
     */
    void num_neighbors(index_type value) {
        NUM_COLLECT_PRECONDITION(
            value > 0, "Number of neighbors must be positive.");
        num_neighbors_ = value;
    }

    /*!
     * \brief Set the scale of length parameters.
     *
     * \param[in] value Value.
     */
    void length_parameter_scale(scalar_type value) {
        length_parameter_scale_ = value;
    }

private:
    //! Type of the calculator of rows.
    using row_calculator_type =
        impl::rbf_fd_row_calculator<length_parameter_calculator_type>;

    //! Distance function.
    distance_function_type distance_function_;

    //! RBF.
    rbf_type rbf_;

    //! Default number of neighbors to use in RBF-FD.
    static constexpr index_type default_num_neighbors = 10;

    //! Number of neighbors.
    index_type num_neighbors_{default_num_neighbors};

    //! Default scale of length parameters.
    static constexpr auto default_length_parameter_scale =
        static_cast<scalar_type>(10);

    //! Scale of length parameters.
    scalar_type length_parameter_scale_{default_length_parameter_scale};
};

}  // namespace num_collect::rbf
