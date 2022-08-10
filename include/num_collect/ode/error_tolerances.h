/*
 * Copyright 2022 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of error_tolerances class.
 */
#pragma once

#include <cmath>
#include <variant>

#include "num_collect/base/concepts/real_scalar.h"  // IWYU pragma: keep
#include "num_collect/base/concepts/real_scalar_dense_vector.h"  // IWYU pragma: keep
#include "num_collect/util/assert.h"

namespace num_collect::ode {

namespace impl {

/*!
 * \brief Default tolerance of relative errors.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
constexpr Scalar default_tol_rel_error = static_cast<Scalar>(1e-6);

/*!
 * \brief Default tolerance of absolute errors.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
constexpr Scalar default_tol_abs_error = static_cast<Scalar>(1e-6);

}  // namespace impl

/*!
 * \brief Class of error tolerances \cite Hairer1993.
 *
 * \tparam Variable Type of variables.
 */
template <typename Variable>
class error_tolerances;

/*!
 * \brief Class of error tolerances \cite Hairer1993.
 *
 * \tparam Variable Type of variables.
 */
template <base::concepts::real_scalar_dense_vector Variable>
class error_tolerances<Variable> {
public:
    //! Type of variables.
    using variable_type = Variable;

    //! Type of scalars.
    using scalar_type = typename variable_type::Scalar;

    /*!
     * \brief Constructor.
     */
    error_tolerances()
        : tol_rel_error_(impl::default_tol_rel_error<scalar_type>),
          tol_abs_error_(impl::default_tol_abs_error<scalar_type>) {}

    /*!
     * \brief Check whether the given error satisfies tolerances.
     *
     * \param[in] variable Variable used for the relative error.
     * \param[in] error Error.
     * \retval true Given error satisfies tolerances.
     * \retval false Given error doesn't satisfy tolerances.
     */
    [[nodiscard]] auto check(const variable_type& variable,
        const variable_type& error) const -> bool {
        class evaluator {
        public:
            evaluator(const variable_type& variable, const variable_type& error)
                : variable_(variable), error_(error) {}

            [[nodiscard]] auto operator()(const scalar_type& tol_rel_error,
                const scalar_type& tol_abs_error) const -> bool {
                return (error_.array().abs() <=
                    (tol_rel_error * variable_.array().abs() + tol_abs_error))
                    .all();
            }

            [[nodiscard]] auto operator()(const variable_type& tol_rel_error,
                const scalar_type& tol_abs_error) const -> bool {
                return (error_.array().abs() <=
                    (tol_rel_error.array() * variable_.array().abs() +
                        tol_abs_error))
                    .all();
            }

            [[nodiscard]] auto operator()(const scalar_type& tol_rel_error,
                const variable_type& tol_abs_error) const -> bool {
                return (error_.array().abs() <=
                    (tol_rel_error * variable_.array().abs() +
                        tol_abs_error.array()))
                    .all();
            }

            [[nodiscard]] auto operator()(const variable_type& tol_rel_error,
                const variable_type& tol_abs_error) const -> bool {
                return (error_.array().abs() <=
                    (tol_rel_error.array() * variable_.array().abs() +
                        tol_abs_error.array()))
                    .all();
            }

        private:
            const variable_type& variable_;
            const variable_type& error_;
        };

        evaluator eval{variable, error};
        return std::visit(eval, tol_rel_error_, tol_abs_error_);
    }

    /*!
     * \brief Calculate the norm of the error determined by tolerances.
     *
     * \param[in] variable Variable used for the relative error.
     * \param[in] error Error.
     * \return Norm value.
     */
    [[nodiscard]] auto calc_norm(const variable_type& variable,
        const variable_type& error) const -> scalar_type {
        class evaluator {
        public:
            evaluator(const variable_type& variable, const variable_type& error)
                : variable_(variable), error_(error) {}

            [[nodiscard]] auto operator()(const scalar_type& tol_rel_error,
                const scalar_type& tol_abs_error) const -> scalar_type {
                using std::sqrt;
                return sqrt((error_.array() /
                                (tol_rel_error * variable_.array().abs() +
                                    tol_abs_error))
                                .abs2()
                                .sum() /
                    static_cast<scalar_type>(variable_.size()));
            }

            [[nodiscard]] auto operator()(const variable_type& tol_rel_error,
                const scalar_type& tol_abs_error) const -> scalar_type {
                using std::sqrt;
                return sqrt(
                    (error_.array() /
                        (tol_rel_error.array() * variable_.array().abs() +
                            tol_abs_error))
                        .abs2()
                        .sum() /
                    static_cast<scalar_type>(variable_.size()));
            }

            [[nodiscard]] auto operator()(const scalar_type& tol_rel_error,
                const variable_type& tol_abs_error) const -> scalar_type {
                using std::sqrt;
                return sqrt((error_.array() /
                                (tol_rel_error * variable_.array().abs() +
                                    tol_abs_error.array()))
                                .abs2()
                                .sum() /
                    static_cast<scalar_type>(variable_.size()));
            }

            [[nodiscard]] auto operator()(const variable_type& tol_rel_error,
                const variable_type& tol_abs_error) const -> scalar_type {
                using std::sqrt;
                return sqrt(
                    (error_.array() /
                        (tol_rel_error.array() * variable_.array().abs() +
                            tol_abs_error.array()))
                        .abs2()
                        .sum() /
                    static_cast<scalar_type>(variable_.size()));
            }

        private:
            const variable_type& variable_;
            const variable_type& error_;
        };

        evaluator eval{variable, error};
        return std::visit(eval, tol_rel_error_, tol_abs_error_);
    }

    /*!
     * \brief Set the tolerance of relative error.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_rel_error(const scalar_type& val) -> error_tolerances& {
        NUM_COLLECT_ASSERT(val >= static_cast<scalar_type>(0));
        tol_rel_error_ = val;
        return *this;
    }

    /*!
     * \brief Set the tolerance of relative error.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_rel_error(const variable_type& val) -> error_tolerances& {
        NUM_COLLECT_ASSERT((val.array() >= static_cast<scalar_type>(0)).all());
        tol_rel_error_ = val;
        return *this;
    }

    /*!
     * \brief Set the tolerance of absolute error.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_abs_error(const scalar_type& val) -> error_tolerances& {
        NUM_COLLECT_ASSERT(val >= static_cast<scalar_type>(0));
        tol_abs_error_ = val;
        return *this;
    }

    /*!
     * \brief Set the tolerance of absolute error.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_abs_error(const variable_type& val) -> error_tolerances& {
        NUM_COLLECT_ASSERT((val.array() >= static_cast<scalar_type>(0)).all());
        tol_abs_error_ = val;
        return *this;
    }

private:
    //! Type of variant used for tolerances.
    using variant_type = std::variant<scalar_type, variable_type>;

    //! Tolerance of relative error.
    variant_type tol_rel_error_;

    //! Tolerance of absolute error.
    variant_type tol_abs_error_;
};

/*!
 * \brief Class of error tolerances \cite Hairer1993.
 *
 * \tparam Variable Type of variables.
 */
template <base::concepts::real_scalar Variable>
class error_tolerances<Variable> {
public:
    //! Type of variables.
    using variable_type = Variable;

    //! Type of scalars.
    using scalar_type = variable_type;

    /*!
     * \brief Constructor.
     */
    explicit error_tolerances()
        : tol_rel_error_(impl::default_tol_rel_error<scalar_type>),
          tol_abs_error_(impl::default_tol_abs_error<scalar_type>) {}

    /*!
     * \brief Check whether the given error satisfies tolerances.
     *
     * \param[in] variable Variable used for the relative error.
     * \param[in] error Error.
     * \retval true Given error satisfies tolerances.
     * \retval false Given error doesn't satisfy tolerances.
     */
    [[nodiscard]] auto check(const variable_type& variable,
        const variable_type& error) const -> bool {
        using std::abs;
        return abs(error) <= tol_rel_error_ * abs(variable) + tol_abs_error_;
    }

    /*!
     * \brief Calculate the norm of the error determined by tolerances.
     *
     * \param[in] variable Variable used for the relative error.
     * \param[in] error Error.
     * \return Norm value.
     */
    [[nodiscard]] auto calc_norm(const variable_type& variable,
        const variable_type& error) const -> scalar_type {
        using std::abs;
        return abs(error / (tol_rel_error_ * abs(variable) + tol_abs_error_));
    }

    /*!
     * \brief Set the tolerance of relative error.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_rel_error(const variable_type& val) -> error_tolerances& {
        NUM_COLLECT_ASSERT(val >= static_cast<scalar_type>(0));
        tol_rel_error_ = val;
        return *this;
    }

    /*!
     * \brief Set the tolerance of absolute error.
     *
     * \param[in] val Value.
     * \return This.
     */
    auto tol_abs_error(const variable_type& val) -> error_tolerances& {
        NUM_COLLECT_ASSERT(val >= static_cast<scalar_type>(0));
        tol_abs_error_ = val;
        return *this;
    }

private:
    //! Tolerance of relative error.
    variable_type tol_rel_error_;

    //! Tolerance of absolute error.
    variable_type tol_abs_error_;
};

}  // namespace num_collect::ode
