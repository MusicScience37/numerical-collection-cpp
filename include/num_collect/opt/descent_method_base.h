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
 * \brief Definition of descent_method_base class.
 */
#pragma once

#include "num_collect/logging/log_tag_view.h"
#include "num_collect/opt/concepts/line_searcher.h"
#include "num_collect/opt/optimizer_base.h"

namespace num_collect::opt {

/*!
 * \brief Base class of implementations of descent methods for optimization.
 *
 * \tparam Derived Type of derived class.
 * \tparam LineSearcher Type of class to perform line search.
 */
template <typename Derived, concepts::line_searcher LineSearcher>
class descent_method_base : public optimizer_base<Derived> {
public:
    //! Type of class to perform line search.
    using line_searcher_type = LineSearcher;

    //! Type of the objective function.
    using objective_function_type =
        typename line_searcher_type::objective_function_type;

    //! Type of variables.
    using variable_type = typename objective_function_type::variable_type;

    //! Type of function values.
    using value_type = typename objective_function_type::value_type;

    /*!
     * \brief Initialize.
     *
     * \param[in] init_variable Initial variable.
     */
    void init(const variable_type& init_variable) {
        line_searcher_.init(init_variable);
        iterations_ = 0;
    }

    /*!
     * \brief Calculate search direction.
     *
     * \return Search direction.
     */
    [[nodiscard]] auto calc_direction() -> variable_type {
        return derived().calc_direction();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterate
     */
    void iterate() {
        line_searcher_.search(calc_direction());
        ++iterations_;
    }

    /*!
     * \copydoc num_collect::base::iterative_solver_base::is_stop_criteria_satisfied
     */
    [[nodiscard]] auto is_stop_criteria_satisfied() const -> bool {
        return gradient_norm() < tol_grad_norm_;
    }

    /*!
     * \brief Access object to perform line search.
     *
     * \return Object to perform line search.
     */
    [[nodiscard]] auto line_searcher() -> line_searcher_type& {
        return line_searcher_;
    }

    /*!
     * \brief Access object to perform line search.
     *
     * \return Object to perform line search.
     */
    [[nodiscard]] auto line_searcher() const -> const line_searcher_type& {
        return line_searcher_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_variable
     */
    [[nodiscard]] auto opt_variable() const -> const variable_type& {
        return line_searcher().opt_variable();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::opt_value
     */
    [[nodiscard]] auto opt_value() const -> const value_type& {
        return line_searcher().opt_value();
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::iterations
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return iterations_;
    }

    /*!
     * \copydoc num_collect::opt::optimizer_base::evaluations
     */
    [[nodiscard]] auto evaluations() const noexcept -> index_type {
        return line_searcher().evaluations();
    }

    /*!
     * \brief Get gradient for current optimal variable.
     *
     * \return Gradient for current optimal variable.
     */
    [[nodiscard]] auto gradient() const
        -> std::invoke_result_t<decltype(&line_searcher_type::gradient),
            const line_searcher_type> {
        return line_searcher().gradient();
    }

    /*!
     * \brief Calculate norm of gradient.
     *
     * \return Norm of gradient.
     */
    [[nodiscard]] auto gradient_norm() const -> value_type {
        return gradient().norm();
    }

    /*!
     * \brief Set tolerance of norm of gradient.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto tol_gradient_norm(const value_type& value) -> Derived& {
        tol_grad_norm_ = value;
        return derived();
    }

protected:
    // use derived() function
    using optimizer_base<Derived>::derived;

    /*!
     * \brief Construct.
     *
     * \param[in] tag Log tag.
     * \param[in] obj_fun Objective function.
     */
    explicit descent_method_base(logging::log_tag_view tag,
        const objective_function_type& obj_fun = objective_function_type())
        : optimizer_base<Derived>(tag), line_searcher_(obj_fun) {}

private:
    //! Object to perform line search.
    line_searcher_type line_searcher_;

    //! Number of iterations.
    index_type iterations_{0};

    //! Default tolerance of norm of gradient.
    static inline const auto default_tol_grad_norm =
        static_cast<value_type>(1e-3);

    //! Tolerance of norm of gradient.
    value_type tol_grad_norm_{default_tol_grad_norm};
};

}  // namespace num_collect::opt
