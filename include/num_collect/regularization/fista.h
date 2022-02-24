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

#include <limits>

#include "num_collect/base/index_type.h"
#include "num_collect/regularization/impl/coeff_param.h"
#include "num_collect/regularization/iterative_regularized_solver_base.h"
#include "num_collect/util/assert.h"

namespace num_collect::regularization {

//! Tag of fista.
inline constexpr auto fista_tag =
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
    //! Type of the base class.
    using base_type =
        iterative_regularized_solver_base<fista<Coeff, Data>, Data>;

    using typename base_type::data_type;
    using typename base_type::scalar_type;

    //! Type of coefficient matrices.
    using coeff_type = Coeff;

    /*!
     * \brief Construct.
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
        inv_max_eigen_ = scalar_type(1) / max_eigen_aat(coeff);
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::init
    void init(const scalar_type& param, data_type& solution) {
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
        // TODO(kenta): Fix this warning.
        // NOLINTNEXTLINE(openmp-use-default-none): Difficult to resolve.
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
        return (iterations() < max_iterations()) ||
            (update() < tol_update_rate() * solution.norm());
    }

    //! \copydoc num_collect::regularization::iterative_regularized_solver_base::configure_iteration_logger
    void configure_iteration_logger(
        logging::iteration_logger& iteration_logger) const {
        iteration_logger.append<index_type>(
            "Iter.", [this] { return iterations(); });
        iteration_logger.append<scalar_type>(
            "Update", [this] { return update(); });
    }

    //! \copydoc num_collect::regularization::implicit_regularized_solver_base::residual_norm
    [[nodiscard]] auto residual_norm(const scalar_type& param,
        const data_type& solution) const -> scalar_type {
        return ((*coeff_) * solution - (*data_)).squaredNorm();
    }

    //! \copydoc num_collect::regularization::implicit_regularized_solver_base::regularization_term
    [[nodiscard]] auto regularization_term(const scalar_type& param,
        const data_type& solution) const -> scalar_type {
        return solution.template lpNorm<1>();
    }

    //! \copydoc num_collect::regularization::implicit_regularized_solver_base::data_size
    [[nodiscard]] auto data_size() const -> index_type { return data_->size(); }

    //! \copydoc num_collect::regularization::implicit_regularized_solver_base::param_search_region
    [[nodiscard]] auto param_search_region() const
        -> std::pair<scalar_type, scalar_type> {
        const scalar_type max_sol_est =
            (coeff_->transpose() * (*data_)).cwiseAbs().maxCoeff();
        return (max_sol_est * impl::coeff_min_param<scalar_type>,
            max_sol_est * impl::coeff_max_param<scalar_type>);
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
        NUM_COLLECT_ASSERT(value > 0);
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
        NUM_COLLECT_ASSERT(value > 0);
        tol_update_rate_ = value;
        return *this;
    }

    /*!
     * \brief Calculate the maximum eigenvalue of \f$ AA^T \f$ for coefficient
     * matrix \f$ A \f$.
     *
     * \param[in] coeff Coefficient matrix.
     * \return Eigenvalue.
     */
    static auto max_eigen_aat(const Coeff& coeff) -> scalar_type {
        const index_type rows = coeff.rows();
        data_type vec = Data::Random(rows);
        vec.normalize();

        data_type mul_vec = coeff * coeff.transpose() * vec;
        scalar_type eigen = vec.dot(mul_vec) / vec.squaredNorm();
        const index_type num_iterations = rows * 10;
        for (index_type i = 0; i < num_iterations; ++i) {
            const scalar_type eigen_before = eigen;
            vec = mul_vec.normalized();
            mul_vec = coeff * coeff.transpose() * vec;
            eigen = vec.dot(mul_vec) / vec.squaredNorm();
            using std::abs;
            constexpr auto tol_update = static_cast<scalar_type>(1e-4);
            if (abs(eigen - eigen_before) / abs(eigen) < tol_update) {
                break;
            }
        }

        return eigen;
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
    static constexpr scalar_type default_tol_update_rate = 1e-3;

    //! Tolerance of update rate of the solution.
    scalar_type tol_update_rate_{default_tol_update_rate};
};

}  // namespace num_collect::regularization
