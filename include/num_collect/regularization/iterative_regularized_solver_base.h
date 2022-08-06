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

#include "num_collect/logging/iteration_logger.h"
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
    : public implicit_regularized_solver_base<Derived, Data> {
public:
    //! Type of data.
    using typename implicit_regularized_solver_base<Derived, Data>::data_type;

    //! Type of scalars.
    using typename implicit_regularized_solver_base<Derived, Data>::scalar_type;

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
     * \brief Configure an iteration logger.
     *
     * \param[in] iteration_logger Iteration logger.
     * \param[in] solution Solution.
     */
    void configure_iteration_logger(logging::iteration_logger& iteration_logger,
        const data_type& solution) const {
        derived().configure_iteration_logger(iteration_logger, solution);
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

        logging::iteration_logger iter_logger{this->logger()};
        configure_iteration_logger(iter_logger, solution);
        iter_logger.write_iteration();

        while (!is_stop_criteria_satisfied(solution)) {
            iterate(param, solution);
            iter_logger.write_iteration();
        }

        iter_logger.write_summary();
    }

protected:
    using implicit_regularized_solver_base<Derived, Data>::derived;
    using implicit_regularized_solver_base<Derived, Data>::logger;

    /*!
     * \brief Constructor.
     *
     * \param[in] tag Log tag.
     */
    explicit iterative_regularized_solver_base(logging::log_tag_view tag)
        : implicit_regularized_solver_base<Derived, Data>(tag) {}
};

}  // namespace num_collect::regularization
