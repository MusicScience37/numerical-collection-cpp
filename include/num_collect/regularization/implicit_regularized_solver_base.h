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
 * \brief Definition of implicit_regularized_solver_base class.
 */
#pragma once

#include "num_collect/base/concepts/dense_matrix.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/regularization/regularized_solver_base.h"

namespace num_collect::regularization {

/*!
 * \brief Base class of solvers using implicit formulas for regularization.
 *
 * \tparam Derived Type of derived class.
 * \tparam Data Type of data.
 */
template <typename Derived, base::concepts::dense_matrix Data>
class implicit_regularized_solver_base
    : public regularized_solver_base<Derived, Data> {
public:
    using typename regularized_solver_base<Derived, Data>::data_type;
    using typename regularized_solver_base<Derived, Data>::scalar_type;

    /*!
     * \brief Change data.
     *
     * \param[in] data New data.
     */
    void change_data(const data_type& data) { derived().change_data(data); }

    /*!
     * \brief Calculate data for a solution.
     *
     * \param[in] solution Solution.
     * \param[out] data Data.
     */
    void calculate_data_for(const data_type& solution, data_type& data) const {
        derived().calculate_data_for(solution, data);
    }

protected:
    /*!
     * \brief Constructor.
     *
     * \param[in] tag Log tag.
     */
    explicit implicit_regularized_solver_base(logging::log_tag_view tag)
        : regularized_solver_base<Derived, Data>(tag) {
        this->logger().set_iterative();
    }

    /*!
     * \brief Access derived object.
     *
     * \return Reference to the derived object.
     */
    [[nodiscard]] auto derived() noexcept -> Derived& {
        return *static_cast<Derived*>(this);
    }

    /*!
     * \brief Access derived object.
     *
     * \return Reference to the derived object.
     */
    [[nodiscard]] auto derived() const noexcept -> const Derived& {
        return *static_cast<const Derived*>(this);
    }
};

}  // namespace num_collect::regularization
