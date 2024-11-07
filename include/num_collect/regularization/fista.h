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
 * \brief Definition of fista class.
 */
#pragma once

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/logging/iterations/iteration_logger.h"
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/regularization/impl/approximate_max_eigen_aat.h"
#include "num_collect/regularization/impl/weak_coeff_param.h"  // IWYU pragma: keep
#include "num_collect/regularization/iterative_regularized_solver_base.h"

namespace num_collect::regularization {

//! Tag of fista.
constexpr auto fista_tag =
    logging::log_tag_view("num_collect::regularization::fista");

/*!
 * \brief Class for fast iterative shrinkage-thresholding algorithm (FISTA)
 * \cite Beck2009 for L1-regularization of linear equations.
 *
 * This class execute fast iterative shrinkage-thresholding algorithm (FISTA)
 * for L1-regularization of linear equations. This class is for large
 * inferior-determined problems, and implemented with OpenMP.
 *
 * \tparam Coeff Type of coefficient matrices.
 * \tparam Data Type of data vectors.
 */
template <typename Coeff, typename Data>
class fista
    : public iterative_regularized_solver_base<fista<Coeff, Data>, Data> {
public:
    //! This type.
    using this_type = fista<Coeff, Data>;

    //! Type of the base class.
    using base_type = iterative_regularized_solver_base<this_type, Data>;

    using typename base_type::data_type;
    using typename base_type::scalar_type;

    //! Type of coefficient matrices.
    using coeff_type = Coeff;

    /*!
     * \brief Constructor.
     */
    fista() : base_type(fista_tag) {}

    /*!
     * \brief Compute internal parameters.
     *
     * \param[in] coeff Coefficient matrix.
     * \param[in] data Data vector.
     *
     * \note Pointers to the arguments are saved in this object, so don't
     * destruct those arguments.
     * \note Call this before init.
     */
    void compute(const Coeff& coeff, const Data& data) {
        coeff_ = &coeff;
        data_ = &data;
        inv_max_eigen_ = static_cast<scalar_type>(1) /
            impl::approximate_max_eigen_aat(coeff);
        NUM_COLLECT_LOG_TRACE(
            this->logger(), "inv_max_eigen={}", inv_max_eigen_);
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::init
    void init(const scalar_type& param, data_type& solution) {
        (void)param;

        if (coeff_->rows() != data_->rows()) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "Coefficient matrix and data vector must have the same number "
                "of rows.");
        }
        if (coeff_->cols() != solution.rows()) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "The number of columns in the coefficient matrix must match "
                "the number of rows in solution vector.");
        }
        if (data_->cols() != solution.cols()) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "Data and solution must have the same number of columns.");
        }

        iterations_ = 0;
        t_ = static_cast<scalar_type>(1);
        y_ = solution;
        update_ = std::numeric_limits<scalar_type>::infinity();
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::iterate
    void iterate(const scalar_type& param, data_type& solution) {
        const scalar_type t_before = t_;
        using std::sqrt;
        t_ = static_cast<scalar_type>(0.5) *        // NOLINT
            (static_cast<scalar_type>(1) +          // NOLINT
                sqrt(static_cast<scalar_type>(1) +  // NOLINT
                    static_cast<scalar_type>(4)     // NOLINT
                        * t_before * t_before));
        const scalar_type coeff_update =
            (t_before - static_cast<scalar_type>(1)) / t_;

        const scalar_type twice_step = inv_max_eigen_;
        const scalar_type step = static_cast<scalar_type>(0.5) * twice_step;
        const scalar_type trunc_thresh = param * step;

        residual_ = -(*data_);
        auto update_sum2 = static_cast<scalar_type>(0);
#pragma omp parallel default(shared)
        {
            const index_type size = solution.size();
            data_type temp_res = Data::Zero(residual_.size());
#pragma omp for nowait
            for (index_type i = 0; i < size; ++i) {
                using std::abs;
                if (abs(y_(i)) > static_cast<scalar_type>(0)) {
                    temp_res += y_(i) * coeff_->col(i);
                }
            }
#pragma omp critical
            residual_ += temp_res;
#pragma omp barrier

#pragma omp for reduction(+ : update_sum2)
            for (index_type i = 0; i < size; ++i) {
                scalar_type cur_next_sol =
                    y_(i) - twice_step * coeff_->col(i).dot(residual_);

                if (cur_next_sol > trunc_thresh) {
                    cur_next_sol = cur_next_sol - trunc_thresh;
                } else if (cur_next_sol < -trunc_thresh) {
                    cur_next_sol = cur_next_sol + trunc_thresh;
                } else {
                    cur_next_sol = static_cast<scalar_type>(0);
                }

                const scalar_type current_update = cur_next_sol - solution(i);
                update_sum2 += current_update * current_update;

                y_(i) = cur_next_sol + coeff_update * current_update;
                solution(i) = cur_next_sol;
            }
        }

        update_ = sqrt(update_sum2);
        ++iterations_;
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::is_stop_criteria_satisfied
    [[nodiscard]] auto is_stop_criteria_satisfied(
        const data_type& solution) const -> bool {
        return (iterations() > max_iterations()) ||
            (update() < tol_update_rate() * solution.norm());
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::configure_iteration_logger
    void configure_iteration_logger(
        logging::iterations::iteration_logger<this_type>& iteration_logger)
        const {
        iteration_logger.template append<index_type>(
            "Iter.", &this_type::iterations);
        iteration_logger.template append<scalar_type>(
            "Update", &this_type::update);
        iteration_logger.template append<scalar_type>(
            "Res.Rate", &this_type::residual_norm_rate);
    }

    //! \copydoc num_collect::regularization::implicit_regularized_solver_base::residual_norm
    [[nodiscard]] auto residual_norm(const data_type& solution) const
        -> scalar_type {
        return ((*coeff_) * solution - (*data_)).squaredNorm();
    }

    //! \copydoc num_collect::regularization::implicit_regularized_solver_base::regularization_term
    [[nodiscard]] auto regularization_term(const data_type& solution) const
        -> scalar_type {
        return solution.template lpNorm<1>();
    }

    //! \copydoc num_collect::regularization::implicit_regularized_solver_base::change_data
    void change_data(const data_type& data) { data_ = &data; }

    //! \copydoc num_collect::regularization::implicit_regularized_solver_base::calculate_data_for
    void calculate_data_for(const data_type& solution, data_type& data) const {
        data = (*coeff_) * solution;
    }

    //! \copydoc num_collect::regularization::regularized_solver_base::data_size
    [[nodiscard]] auto data_size() const -> index_type { return data_->size(); }

    //! \copydoc num_collect::regularization::regularized_solver_base::param_search_region
    [[nodiscard]] auto param_search_region() const
        -> std::pair<scalar_type, scalar_type> {
        const scalar_type max_sol_est =
            (coeff_->transpose() * (*data_)).cwiseAbs().maxCoeff();
        NUM_COLLECT_LOG_TRACE(this->logger(), "max_sol_est={}", max_sol_est);
        constexpr auto tol_update_coeff_multiplier =
            static_cast<scalar_type>(10);
        return {max_sol_est *
                std::max(impl::weak_coeff_min_param<scalar_type>,
                    tol_update_coeff_multiplier * tol_update_rate_),
            max_sol_est * impl::weak_coeff_max_param<scalar_type>};
    }

    /*!
     * \brief Get the number of iterations.
     *
     * \return Value.
     */
    [[nodiscard]] auto iterations() const noexcept -> index_type {
        return iterations_;
    }

    /*!
     * \brief Get the norm of the update of the solution in the last iteration.
     *
     * \return Value.
     */
    [[nodiscard]] auto update() const noexcept -> scalar_type {
        return update_;
    }

    /*!
     * \brief Get the rate of the last residual norm.
     *
     * \return Rate of the residual norm.
     */
    [[nodiscard]] auto residual_norm_rate() const -> scalar_type {
        return residual_.squaredNorm() / data_->squaredNorm();
    }

    /*!
     * \brief Get the maximum number of iterations.
     *
     * \return Value.
     */
    [[nodiscard]] auto max_iterations() const -> index_type {
        return max_iterations_;
    }

    /*!
     * \brief Set the maximum number of iterations.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto max_iterations(index_type value) -> fista& {
        if (value <= 0) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "Maximum number of iterations must be a positive integer.");
        }
        max_iterations_ = value;
        return *this;
    }

    /*!
     * \brief Get the tolerance of update rate of the solution.
     *
     * \return Value.
     */
    [[nodiscard]] auto tol_update_rate() const -> scalar_type {
        return tol_update_rate_;
    }

    /*!
     * \brief Set the tolerance of update rate of the solution.
     *
     * \param[in] value Value.
     * \return This object.
     */
    auto tol_update_rate(scalar_type value) -> fista& {
        if (value <= static_cast<scalar_type>(0)) {
            NUM_COLLECT_LOG_AND_THROW(invalid_argument,
                "Tolerance of update rate of the solution must be a positive "
                "value.");
        }
        tol_update_rate_ = value;
        return *this;
    }

private:
    //! Coefficient matrix.
    const coeff_type* coeff_{nullptr};

    //! Data vector.
    const data_type* data_{nullptr};

    /*!
     * \brief Inverse of maximum eigenvalue of \f$ AA^T \f$ for coefficient
     * matrix \f$ A \f$.
     */
    scalar_type inv_max_eigen_{};

    //! Number of iterations.
    index_type iterations_{};

    //! Parameter for step size of y_.
    scalar_type t_{};

    //! Another vector to update in FISTA.
    data_type y_{};

    //! Residual vector.
    data_type residual_{};

    //! Norm of the update of the solution in the last iteration.
    scalar_type update_{};

    //! Default maximum number of iterations.
    static constexpr index_type default_max_iterations = 1000;

    //! Maximum number of iterations.
    index_type max_iterations_{default_max_iterations};

    //! Default tolerance of update rate of the solution.
    static constexpr auto default_tol_update_rate =
        static_cast<scalar_type>(1e-4);

    //! Tolerance of update rate of the solution.
    scalar_type tol_update_rate_{default_tol_update_rate};
};

}  // namespace num_collect::regularization
