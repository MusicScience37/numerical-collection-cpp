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
 * \brief Definition of gauss_legendre_kronrod_integrator function.
 */
#pragma once

// IWYU pragma: no_include <__hash_table>
// IWYU pragma: no_include <complex>

#include <cmath>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>

#include "num_collect/base/concepts/invocable_as.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/exception.h"
#include "num_collect/base/index_type.h"
#include "num_collect/base/norm.h"
#include "num_collect/base/precondition.h"
#include "num_collect/constants/half.h"  // IWYU pragma: keep
#include "num_collect/constants/one.h"   // IWYU pragma: keep
#include "num_collect/constants/two.h"   // IWYU pragma: keep
#include "num_collect/constants/zero.h"  // IWYU pragma: keep
#include "num_collect/logging/log_tag_view.h"
#include "num_collect/logging/logging_macros.h"
#include "num_collect/logging/logging_mixin.h"
#include "num_collect/util/assert.h"
#include "num_collect/util/safe_cast.h"
#include "num_collect/util/static_stack.h"

namespace num_collect::integration {

//! Tag of gauss_legendre_kronrod_integrator.
constexpr auto gauss_legendre_kronrod_integrator_tag = logging::log_tag_view(
    "num_collect::integration::gauss_legendre_kronrod_integrator");

/*!
 * \brief Class to perform numerical adaptive integration with
 * Gauss-Legendre-Kronrod formula in \cite Laurie1997.
 *
 * \tparam Signature Function signature.
 */
template <typename Signature>
class gauss_legendre_kronrod_integrator;

/*!
 * \brief Class to perform numerical adaptive integration with
 * Gauss-Legendre-Kronrod formula in \cite Laurie1997.
 *
 * \tparam Result Type of results.
 * \tparam Variable Type of variables.
 */
template <typename Result, base::concepts::real_scalar Variable>
class gauss_legendre_kronrod_integrator<Result(Variable)>
    : public logging::logging_mixin {
public:
    //! Type of variables.
    using variable_type = std::decay_t<Variable>;

    //! Type of results.
    using result_type = std::decay_t<Result>;

    //! Default degree.
    static constexpr index_type default_degree = 5;

    /*!
     * \brief Constructor.
     *
     * \param[in] degree Degree.
     */
    explicit gauss_legendre_kronrod_integrator(
        index_type degree = default_degree)
        : logging::logging_mixin(gauss_legendre_kronrod_integrator_tag),
          degree_(degree) {
        NUM_COLLECT_PRECONDITION(degree >= 1, this->logger(),
            "Degree of Legendre function must be at least one.");
        compute_parameters();
    }

    /*!
     * \brief Prepare internal parameters.
     *
     * \param[in] degree Degree.
     */
    void prepare(index_type degree) {
        NUM_COLLECT_PRECONDITION(degree >= 1, this->logger(),
            "Degree of Legendre function must be at least one.");
        degree_ = degree;
        compute_parameters();
    }

    /*!
     * \brief Integrate a function and return two estimates.
     *
     * \tparam Function Type of function.
     * \param[in] function Function.
     * \param[in] left Left boundary.
     * \param[in] right Right boundary.
     * \return Result.
     */
    template <base::concepts::invocable_as<result_type(variable_type)> Function>
    [[nodiscard]] auto integrate_once(
        const Function& function, variable_type left, variable_type right) const
        -> std::pair<result_type, result_type> {
        const auto center = constants::half<variable_type> * (left + right);
        const auto half_width = constants::half<variable_type> * (right - left);

        result_type sum_gauss =
            function(center) * constants::zero<variable_type>;
        result_type sum_kronrod = sum_gauss;
        for (index_type i = 0; i < nodes_gauss_.size(); ++i) {
            const variable_type x = center + half_width * nodes_gauss_[i];
            const result_type val = function(x);
            sum_gauss += val * weights_gauss_[i];
            sum_kronrod += val * weights_gauss_for_kronrod_[i];
        }
        for (index_type i = 0; i < nodes_kronrod_.size(); ++i) {
            const variable_type x = center + half_width * nodes_kronrod_[i];
            const result_type val = function(x);
            sum_kronrod += val * weights_kronrod_[i];
        }

        sum_gauss *= half_width;
        sum_kronrod *= half_width;
        return {sum_gauss, sum_kronrod};
    }

    /*!
     * \brief Integrate a function adaptively.
     *
     * \tparam Function Type of function.
     * \param[in] function Function.
     * \param[in] left Left boundary.
     * \param[in] right Right boundary.
     * \return Result.
     */
    template <base::concepts::invocable_as<result_type(variable_type)> Function>
    [[nodiscard]] auto integrate(const Function& function, variable_type left,
        variable_type right) const -> result_type {
        NUM_COLLECT_PRECONDITION(left < right, this->logger(),
            "The boundaries of the range to integrate on must satisfy left < "
            "right.");

        const variable_type inv_width =
            constants::one<variable_type> / (right - left);

        result_type sum =
            function(constants::half<variable_type> * (left + right)) *
            constants::zero<variable_type>;

        constexpr auto stack_size = static_cast<std::size_t>(
            std::numeric_limits<variable_type>::digits);
        util::static_stack<variable_type, stack_size> remaining_right;
        variable_type cur_left = left;
        variable_type cur_right = right;

        while (true) {
            const auto [val_gauss, val_kronrod] =
                integrate_once(function, cur_left, cur_right);
            const variable_type val_norm = num_collect::norm(val_kronrod);
            const variable_type error_norm =
                num_collect::norm(val_gauss - val_kronrod);
            using std::abs;
            const auto div_rate = (cur_right - cur_left) * inv_width;
            if ((error_norm < tol_abs_error_ * div_rate) ||
                (error_norm < tol_rel_error_ * val_norm) ||
                (div_rate < min_div_rate_)) {
                sum += val_kronrod;
                if (remaining_right.empty()) {
                    break;
                }
                cur_left = cur_right;
                cur_right = remaining_right.top();
                remaining_right.pop();
            } else {
                remaining_right.push(cur_right);
                cur_right =
                    constants::half<variable_type> * (cur_left + cur_right);
            }
        }
        return sum;
    }

    /*!
     * \brief Integrate a function.
     *
     * \tparam Function Type of function.
     * \param[in] function Function.
     * \param[in] left Left boundary.
     * \param[in] right Right boundary.
     * \return Result.
     */
    template <base::concepts::invocable_as<result_type(variable_type)> Function>
    [[nodiscard]] auto operator()(const Function& function, variable_type left,
        variable_type right) const -> result_type {
        return integrate(function, left, right);
    }

    /*!
     * \brief Set tolerance of absolute error.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_abs_error(variable_type val)
        -> gauss_legendre_kronrod_integrator& {
        NUM_COLLECT_PRECONDITION(val > static_cast<variable_type>(0),
            this->logger(),
            "Tolerance of absolute error must be positive value.");
        tol_abs_error_ = val;
        return *this;
    }

    /*!
     * \brief Set tolerance of relative error.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_rel_error(variable_type val)
        -> gauss_legendre_kronrod_integrator& {
        NUM_COLLECT_PRECONDITION(val > static_cast<variable_type>(0),
            this->logger(),
            "Tolerance of relative error must be positive value.");
        tol_rel_error_ = val;
        return *this;
    }

    /*!
     * \brief Set minimum rate of division of integration region.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto min_div_rate(variable_type val) -> gauss_legendre_kronrod_integrator& {
        NUM_COLLECT_PRECONDITION(
            val > std::numeric_limits<variable_type>::epsilon(), this->logger(),
            "Minimum rate of division of integration region must be larger "
            "than the machine epsilon.");
        min_div_rate_ = val;
        return *this;
    }

private:
    //! Type of vectors.
    using vector_type = Eigen::Matrix<variable_type, Eigen::Dynamic, 1>;

    //! Type of matrices.
    using matrix_type =
        Eigen::Matrix<variable_type, Eigen::Dynamic, Eigen::Dynamic>;

    /*!
     * \brief Compute internal parameters.
     */
    void compute_parameters() {
        const index_type n = degree_;
        const index_type extended_size = n * 2 + 1;
        vector_type a = vector_type::Zero(extended_size);
        vector_type b = vector_type::Zero(extended_size);

        b[0] = constants::two<variable_type>;
        for (index_type i = 1; i <= (3 * n + 1) / 2; ++i) {
            auto temp = static_cast<variable_type>(i);
            temp *= temp;
            temp /= static_cast<variable_type>(2 * i + 1) *
                static_cast<variable_type>(2 * i - 1);
            b[i] = temp;
        }
        jacobi2gauss(a, b, degree_, nodes_gauss_, weights_gauss_);
        NUM_COLLECT_ASSERT(nodes_gauss_.allFinite());
        NUM_COLLECT_ASSERT(weights_gauss_.allFinite());

        generate_jacobi_kronrod_matrix(a, b);
        NUM_COLLECT_ASSERT(a.allFinite());
        NUM_COLLECT_ASSERT(b.allFinite());
        vector_type nodes_all;
        vector_type weights_all;
        jacobi2gauss(a, b, extended_size, nodes_all, weights_all);
        NUM_COLLECT_ASSERT(nodes_all.allFinite());
        NUM_COLLECT_ASSERT(weights_all.allFinite());

        std::unordered_set<index_type> additional_nodes_index;
        additional_nodes_index.reserve(
            util::safe_cast<std::size_t>(extended_size));
        for (index_type i = 0; i < extended_size; ++i) {
            additional_nodes_index.insert(i);
        }
        weights_gauss_for_kronrod_ = vector_type::Zero(degree_);
        for (index_type i = 0; i < degree_; ++i) {
            variable_type min_dist =
                std::numeric_limits<variable_type>::infinity();
            index_type min_ind = -1;
            for (index_type j : additional_nodes_index) {
                using std::abs;
                const variable_type dist = abs(nodes_gauss_[i] - nodes_all[j]);
                if (dist < min_dist) {
                    min_dist = dist;
                    min_ind = j;
                }
            }
            weights_gauss_for_kronrod_[i] = weights_all[min_ind];
            additional_nodes_index.erase(min_ind);
        }
        nodes_kronrod_ = vector_type::Zero(n + 1);
        weights_kronrod_ = vector_type::Zero(n + 1);
        const auto additional_nodes_vec = std::vector<index_type>(
            additional_nodes_index.begin(), additional_nodes_index.end());
        for (std::size_t i = 0; i < additional_nodes_vec.size(); ++i) {
            nodes_kronrod_[util::safe_cast<index_type>(i)] =
                nodes_all[additional_nodes_vec[i]];
            weights_kronrod_[util::safe_cast<index_type>(i)] =
                weights_all[additional_nodes_vec[i]];
        }
        NUM_COLLECT_ASSERT(nodes_kronrod_.allFinite());
        NUM_COLLECT_ASSERT(weights_kronrod_.allFinite());
    }

    /*!
     * \brief Generate Jacobi-Kronrod matrix as in a pseudo-code in the appendix
     * of \cite Laurie1997.
     *
     * \param[in] a Diagonal coefficients in Jacobi matrix.
     * \param[in] b Square of second diagonal coefficients in Jacobi matrix.
     */
    void generate_jacobi_kronrod_matrix(vector_type& a, vector_type& b) {
        const index_type n = degree_;
        vector_type s = vector_type::Zero(n / 2 + 2);
        vector_type t = vector_type::Zero(n / 2 + 2);
        t[1] = b[n + 1];
        for (index_type m = 0; m < n - 1; ++m) {
            variable_type u = constants::zero<variable_type>;
            for (index_type k = (m + 1) / 2; k > 0; --k) {
                index_type l = m - k;
                u += (a[k + n + 1] - a[l]) * t[k + 1] + b[k + n + 1] * s[k] -
                    b[l] * s[k + 1];
                s[k + 1] = u;
            }
            {
                constexpr index_type k = 0;
                index_type l = m - k;
                u += (a[k + n + 1] - a[l]) * t[k + 1] + b[k + n + 1] * s[k] -
                    b[l] * s[k + 1];
                s[k + 1] = u;
            }
            s.swap(t);
        }
        for (index_type j = n / 2; j > 0; --j) {
            s[j + 1] = s[j];
        }
        s[1] = s[0];
        for (index_type m = n - 1; m <= 2 * n - 3; ++m) {
            variable_type u = constants::zero<variable_type>;
            index_type j = 0;
            for (index_type k = m + 1 - n; k <= (m - 1) / 2; ++k) {
                index_type l = m - k;
                j = n - 1 - l;
                u += -(a[k + n + 1] - a[l]) * t[j + 1] -
                    b[k + n + 1] * s[j + 1] + b[l] * s[j + 2];
                s[j + 1] = u;
            }
            if (m % 2 == 0) {
                index_type k = m / 2;
                a[k + n + 1] =
                    a[k] + (s[j + 1] - b[k + n + 1] * s[j + 2]) / t[j + 2];
            } else {
                index_type k = (m + 1) / 2;
                b[k + n + 1] = s[j + 1] / s[j + 2];
            }
            s.swap(t);
        }
        a[2 * n] = a[n - 1] - b[2 * n] * s[1] * t[1];
    }

    /*!
     * \brief Calculate Gauss quadrature from Jacobi matrix.
     *
     * \param[in] a Diagonal coefficients in Jacobi matrix.
     * \param[in] b Square of second diagonal coefficients in Jacobi matrix.
     * \param[in] size Size of Jacobi matrix.
     * \param[in] nodes Nodes of Gauss quadrature.
     * \param[in] weights Weights of Gauss quadrature.
     */
    static void jacobi2gauss(const vector_type& a, const vector_type& b,
        index_type size, vector_type& nodes, vector_type& weights) {
        matrix_type jacobi = matrix_type::Zero(size, size);
        jacobi(0, 0) = a[0];
        for (index_type i = 1; i < size; ++i) {
            jacobi(i, i) = a[i];
            const variable_type temp = std::sqrt(b[i]);
            jacobi(i - 1, i) = temp;
            jacobi(i, i - 1) = temp;
        }

        Eigen::SelfAdjointEigenSolver<matrix_type> eig(
            jacobi, Eigen::ComputeEigenvectors);

        // reverse to get the results in ascending order of nodes
        nodes = eig.eigenvalues();
        weights = b[0] * eig.eigenvectors().row(0).transpose().cwiseAbs2();
    }

    //! Degree.
    index_type degree_;

    //! Nodes for Gauss quadrature.
    vector_type nodes_gauss_{};

    //! Weights for Gauss quadrature.
    vector_type weights_gauss_{};

    //! Weights of nodes in Gauss quadrature for Gauss-Kronrod quadrature.
    vector_type weights_gauss_for_kronrod_{};

    //! Additional nodes for Gauss-Kronrod quadrature.
    vector_type nodes_kronrod_{};

    //! Additional weights for Gauss-Kronrod quadrature.
    vector_type weights_kronrod_{};

    //! Default tolerance of error.
    static constexpr variable_type default_tol_error =
        std::numeric_limits<variable_type>::epsilon() *
        static_cast<variable_type>(1e+4);

    //! Tolerance of absolute error.
    variable_type tol_abs_error_{default_tol_error};

    //! Tolerance of relative error.
    variable_type tol_rel_error_{default_tol_error};

    //! Default minimum rate of division of integration region.
    static constexpr variable_type default_min_div_rate =
        std::numeric_limits<variable_type>::epsilon() *
        static_cast<variable_type>(1e+4);

    //! Minimum rate of division of integration region.
    variable_type min_div_rate_{default_min_div_rate};
};

}  // namespace num_collect::integration
