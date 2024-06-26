/*
 * Copyright 2021 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of iterative_regularized_solver_base class.
 */
#pragma once

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/logging/iterations/iteration_logger_mixin.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/regularization/implicit_regularized_solver_base.h"

namespace num_collect::regularization {

/*!
 * \brief Base class of solvers using iterative formulas for regularization.
 *
 * \tparam Derived Type of derived class.
 * \tparam Data Type of data.
 */
template <typename Derived, base::concepts::dense_matrix Data>
class iterative_regularized_solver_base
    : public implicit_regularized_solver_base<Derived, Data>,
      public logging::iterations::iteration_logger_mixin<Derived> {
public:
    //! Type of data.
    using typename implicit_regularized_solver_base<Derived, Data>::data_type;

    //! Type of scalars.
    using typename implicit_regularized_solver_base<Derived, Data>::scalar_type;

    using implicit_regularized_solver_base<Derived, Data>::logger;

    /*!
     * \brief Initialize.
     *
     * \param[in] param Regularization parameter.
     * \param[in,out] solution Solution. (Assumed to be the initial solution.)
     *
     * \warning Any required initializations (with `compute` functions) are
     * assumed to have been done.
     */
    void init(const scalar_type& param, data_type& solution) {
        derived().init(param, solution);
    }

    /*!
     * \brief Iterate the algorithm once.
     *
     * \param[in] param Regularization parameter.
     * \param[in,out] solution Solution. (Assumed to be the last solution.)
     *
     * \warning Any required initializations (with `init` functions) are assumed
     * to have been done.
     */
    void iterate(const scalar_type& param, data_type& solution) {
        derived().iterate(param, solution);
    }

    /*!
     * \brief Determine if stopping criteria of the algorithm are satisfied.
     *
     * \param[in] solution Solution.
     * \return If stopping criteria of the algorithm are satisfied.
     */
    [[nodiscard]] auto is_stop_criteria_satisfied(
        const data_type& solution) const -> bool {
        return derived().is_stop_criteria_satisfied(solution);
    }

    /*!
     * \brief Solve for a regularization parameter.
     *
     * \param[in] param Regularization parameter.
     * \param[in,out] solution Solution. (Assumed to be the initial solution.)
     *
     * \warning Any required initializations (with `compute` functions) are
     * assumed to have been done.
     */
    void solve(const scalar_type& param, data_type& solution) {
        init(param, solution);

        auto& iter_logger = this->initialize_iteration_logger();
        iter_logger.write_iteration(&derived());

        while (!is_stop_criteria_satisfied(solution)) {
            iterate(param, solution);
            iter_logger.write_iteration(&derived());
        }

        iter_logger.write_summary(&derived());
    }

protected:
    using implicit_regularized_solver_base<Derived, Data>::derived;

    /*!
     * \brief Constructor.
     *
     * \param[in] tag Log tag.
     */
    explicit iterative_regularized_solver_base(logging::log_tag_view tag)
        : implicit_regularized_solver_base<Derived, Data>(tag) {
        this->logger().set_iterative();
    }
};

}  // namespace num_collect::regularization
