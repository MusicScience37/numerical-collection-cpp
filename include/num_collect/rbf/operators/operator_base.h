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
 * \brief Definition of operator_base class.
 */
#pragma once

#include <utility>

#include "num_collect/base/concepts/dense_vector.h"
#include "num_collect/base/concepts/real_scalar.h"
#include "num_collect/base/get_compile_time_size.h"
#include "num_collect/base/index_type.h"
#include "num_collect/rbf/concepts/length_parameter_calculator.h"
#include "num_collect/rbf/operators/operator_evaluator.h"
#include "num_collect/rbf/polynomial_term.h"
#include "num_collect/rbf/polynomial_term_generator.h"
#include "num_collect/util/vector_view.h"

namespace num_collect::rbf::operators {

template <typename Scalar, typename Operator>
class operator_multiple;

template <typename Operator>
class operator_negation;

template <typename LeftOperator, typename RightOperator>
class operator_sum;

/*!
 * \brief Base class of operators.
 *
 * \tparam Derived Type of derived classes.
 *
 * This class provides following operators as non-member functions:
 *
 * - `scalar * operator`
 * - `operator * scalar`
 * - `operator / scalar`
 * - `-operator`
 * - `operator + operator`
 * - `operator - operator`
 */
template <typename Derived>
class operator_base {
public:
    /*!
     * \brief Get the derived class.
     *
     * \return Derived class.
     */
    [[nodiscard]] auto derived() const -> const Derived& {
        return static_cast<const Derived&>(*this);
    }

    /*!
     * \brief Get the derived class.
     *
     * \return Derived class.
     */
    [[nodiscard]] auto derived() -> Derived& {
        return static_cast<Derived&>(*this);
    }
};

/*!
 * \brief Class of constant multiple of operators.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Operator Type of operators.
 */
template <typename Scalar, typename Operator>
class operator_multiple
    : public operator_base<operator_multiple<Scalar, Operator>> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] scalar Scalar.
     * \param[in] target_operator Operator.
     */
    operator_multiple(Scalar scalar, Operator target_operator)
        : scalar_(scalar), operator_(std::move(target_operator)) {}

    /*!
     * \brief Get the scalar.
     *
     * \return Scalar.
     */
    [[nodiscard]] auto scalar() const -> const Scalar& { return scalar_; }

    /*!
     * \brief Get the operator.
     *
     * \return Operator.
     */
    [[nodiscard]] auto target_operator() const -> const Operator& {
        return operator_;
    }

private:
    //! Scalar.
    Scalar scalar_;

    //! Operator.
    Operator operator_;
};

/*!
 * \brief Class of negation of operators.
 *
 * \tparam Operator Type of operators.
 */
template <typename Operator>
class operator_negation : public operator_base<operator_negation<Operator>> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] target_operator Operator.
     */
    explicit operator_negation(Operator target_operator)
        : operator_(std::move(target_operator)) {}

    /*!
     * \brief Get the operator.
     *
     * \return Operator.
     */
    [[nodiscard]] auto target_operator() const -> const Operator& {
        return operator_;
    }

private:
    //! Operator.
    Operator operator_;
};

/*!
 * \brief Class of sum of operators.
 *
 * \tparam LeftOperator Type of left operators.
 * \tparam RightOperator Type of right operators.
 */
template <typename LeftOperator, typename RightOperator>
class operator_sum
    : public operator_base<operator_sum<LeftOperator, RightOperator>> {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] left_operator Left operator.
     * \param[in] right_operator Right operator.
     */
    operator_sum(LeftOperator left_operator, RightOperator right_operator)
        : left_operator_(std::move(left_operator)),
          right_operator_(std::move(right_operator)) {}

    /*!
     * \brief Get the left operator.
     *
     * \return Left operator.
     */
    [[nodiscard]] auto left_operator() const -> const LeftOperator& {
        return left_operator_;
    }

    /*!
     * \brief Get the right operator.
     *
     * \return Right operator.
     */
    [[nodiscard]] auto right_operator() const -> const RightOperator& {
        return right_operator_;
    }

private:
    //! Left operator.
    LeftOperator left_operator_;

    //! Right operator.
    RightOperator right_operator_;
};

/*!
 * \brief Get a constant multiple of an operator.
 *
 * \tparam Derived Type of derived class of the operator.
 * \tparam Scalar Type of the scalar.
 * \param[in] target_operator Operator.
 * \param[in] scalar Scalar.
 * \return Constant multiple of the operator.
 */
template <typename Derived, base::concepts::real_scalar Scalar>
auto operator*(const operator_base<Derived>& target_operator, Scalar scalar)
    -> operator_multiple<Scalar, Derived> {
    return operator_multiple<Scalar, Derived>(
        scalar, target_operator.derived());
}

/*!
 * \brief Get a constant multiple of an operator.
 *
 * \tparam Scalar Type of the scalar.
 * \tparam Derived Type of derived class of the operator.
 * \param[in] scalar Scalar.
 * \param[in] target_operator Operator.
 * \return Constant multiple of the operator.
 */
template <base::concepts::real_scalar Scalar, typename Derived>
auto operator*(Scalar scalar, const operator_base<Derived>& target_operator)
    -> operator_multiple<Scalar, Derived> {
    return operator_multiple<Scalar, Derived>(
        scalar, target_operator.derived());
}

/*!
 * \brief Divide an operator by a scalar.
 *
 * \tparam Derived Type of derived class of the operator.
 * \tparam Scalar Type of the scalar.
 * \param[in] target_operator Operator.
 * \param[in] scalar Scalar.
 * \return Constant multiple of the operator using the reciprocal of the scalar.
 */
template <typename Derived, base::concepts::real_scalar Scalar>
auto operator/(const operator_base<Derived>& target_operator, Scalar scalar)
    -> operator_multiple<Scalar, Derived> {
    return operator_multiple<Scalar, Derived>(
        static_cast<Scalar>(1) / scalar, target_operator.derived());
}

/*!
 * \brief Negate an operator.
 *
 * \tparam Derived Type of derived class of the operator.
 * \param[in] target_operator Operator.
 * \return Negation of the operator.
 */
template <typename Derived>
auto operator-(const operator_base<Derived>& target_operator)
    -> operator_negation<Derived> {
    return operator_negation<Derived>(target_operator.derived());
}

/*!
 * \brief Add two operators.
 *
 * \tparam LeftDerived Type of derived class of the left operator.
 * \tparam RightDerived Type of derived class of the right operator.
 * \param[in] left_operator Left operator.
 * \param[in] right_operator Right operator.
 * \return Sum of the two operators.
 */
template <typename LeftDerived, typename RightDerived>
auto operator+(const operator_base<LeftDerived>& left_operator,
    const operator_base<RightDerived>& right_operator)
    -> operator_sum<LeftDerived, RightDerived> {
    return operator_sum<LeftDerived, RightDerived>(
        left_operator.derived(), right_operator.derived());
}

/*!
 * \brief Subtract two operators.
 *
 * \tparam LeftDerived Type of derived class of the left operator.
 * \tparam RightDerived Type of derived class of the right operator.
 * \param[in] left_operator Left operator.
 * \param[in] right_operator Right operator.
 * \return Difference of the two operators.
 */
template <typename LeftDerived, typename RightDerived>
auto operator-(const operator_base<LeftDerived>& left_operator,
    const operator_base<RightDerived>& right_operator)
    -> operator_sum<LeftDerived, operator_negation<RightDerived>> {
    return operator_sum<LeftDerived, operator_negation<RightDerived>>(
        left_operator.derived(), -right_operator.derived());
}

/*!
 * \brief Specialization of num_collect::rbf::operators::operator_evaluator for
 * num_collect::rbf::operators::operator_multiple.
 */
template <typename Scalar, typename TargetOperator, concepts::rbf RBF,
    concepts::distance_function DistanceFunction>
struct operator_evaluator<operator_multiple<Scalar, TargetOperator>, RBF,
    DistanceFunction> {
    //! Type of the RBF.
    using rbf_type = RBF;

    //! Type of the distance function.
    using distance_function_type = DistanceFunction;

    //! Type of variables.
    using variable_type = typename DistanceFunction::variable_type;

    //! Type of kernel values.
    using kernel_value_type = typename rbf_type::scalar_type;

    //! Type of the operator.
    using operator_type = operator_multiple<Scalar, TargetOperator>;

    //! Dimensions of the variables.
    static constexpr index_type variable_dimensions =
        get_compile_time_size<variable_type>();

    /*!
     * \brief Evaluate an operator.
     *
     * \tparam LengthParameterCalculator Type of the calculator of length
     * parameters.
     * \tparam KernelCoeffVector Type of the vector of coefficients of kernels.
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     * \param[in] length_parameter_calculator Calculator of length parameters.
     * \param[in] target_operator Operator to evaluate.
     * \param[in] sample_variables Variables of samples.
     * \param[in] kernel_coefficients Coefficients of kernels.
     * \return
     */
    template <concepts::length_parameter_calculator LengthParameterCalculator,
        base::concepts::dense_vector KernelCoeffVector>
    [[nodiscard]] static auto evaluate(
        const distance_function_type& distance_function, const rbf_type& rbf,
        const LengthParameterCalculator& length_parameter_calculator,
        const operator_type& target_operator,
        util::vector_view<const variable_type> sample_variables,
        const KernelCoeffVector& kernel_coefficients) {
        return target_operator.scalar() *
            operator_evaluator<TargetOperator, RBF, DistanceFunction>::evaluate(
                distance_function, rbf, length_parameter_calculator,
                target_operator.target_operator(), sample_variables,
                kernel_coefficients);
    }

    /*!
     * \brief Evaluate an operator for one sample point.
     *
     * \tparam FunctionValue Type of function values.
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     * \param[in] length_parameter Length parameter.
     * \param[in] target_operator Operator to evaluate.
     * \param[in] sample_variable Variable of the sample.
     * \param[in] kernel_coeff Coefficient of the kernel for the sample.
     * \return Evaluated function value.
     */
    template <typename FunctionValue>
    [[nodiscard]] static auto evaluate_for_one_sample(
        const distance_function_type& distance_function, const rbf_type& rbf,
        const kernel_value_type& length_parameter,
        const operator_type& target_operator,
        const variable_type& sample_variable,
        const FunctionValue& kernel_coeff) {
        return target_operator.scalar() *
            operator_evaluator<TargetOperator, RBF,
                DistanceFunction>::evaluate_for_one_sample(distance_function,
                rbf, length_parameter, target_operator.target_operator(),
                sample_variable, kernel_coeff);
    }

    /*!
     * \brief Evaluate a polynomial.
     *
     * \tparam CoeffVector Type of the vector of coefficients of the polynomial.
     * \param[in] target_operator Operator to evaluate.
     * \param[in] term_generator Generator of polynomial terms.
     * \param[in] polynomial_coefficients Coefficients of the polynomial.
     * \return Evaluated polynomial value.
     */
    template <base::concepts::dense_vector CoeffVector>
    [[nodiscard]] static auto evaluate_polynomial(
        const operator_type& target_operator,
        const polynomial_term_generator<variable_dimensions>& term_generator,
        const CoeffVector& polynomial_coefficients) {
        return target_operator.scalar() *
            operator_evaluator<TargetOperator, RBF,
                DistanceFunction>::evaluate_polynomial(target_operator
                                                           .target_operator(),
                term_generator, polynomial_coefficients);
    }

    /*!
     * \brief Evaluate a polynomial term.
     *
     * \param[in] target_operator Operator to evaluate.
     * \param[in] polynomial_term Polynomial term to evaluate.
     * \return Evaluated polynomial value.
     */
    template <int NumDimensions>
    [[nodiscard]] static auto evaluate_polynomial_term(
        const operator_type& target_operator,
        const polynomial_term<NumDimensions>& polynomial_term) {
        return target_operator.scalar() *
            operator_evaluator<TargetOperator, RBF, DistanceFunction>::
                evaluate_polynomial_term(
                    target_operator.target_operator(), polynomial_term);
    }
};

/*!
 * \brief Specialization of num_collect::rbf::operators::operator_evaluator for
 * num_collect::rbf::operators::operator_negation.
 */
template <typename TargetOperator, concepts::rbf RBF,
    concepts::distance_function DistanceFunction>
struct operator_evaluator<operator_negation<TargetOperator>, RBF,
    DistanceFunction> {
    //! Type of the RBF.
    using rbf_type = RBF;

    //! Type of the distance function.
    using distance_function_type = DistanceFunction;

    //! Type of variables.
    using variable_type = typename DistanceFunction::variable_type;

    //! Type of kernel values.
    using kernel_value_type = typename rbf_type::scalar_type;

    //! Type of the operator.
    using operator_type = operator_negation<TargetOperator>;

    //! Dimensions of the variables.
    static constexpr index_type variable_dimensions =
        get_compile_time_size<variable_type>();

    /*!
     * \brief Evaluate an operator.
     *
     * \tparam LengthParameterCalculator Type of the calculator of length
     * parameters.
     * \tparam KernelCoeffVector Type of the vector of coefficients of kernels.
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     * \param[in] length_parameter_calculator Calculator of length parameters.
     * \param[in] target_operator Operator to evaluate.
     * \param[in] sample_variables Variables of samples.
     * \param[in] kernel_coefficients Coefficients of kernels.
     * \return
     */
    template <concepts::length_parameter_calculator LengthParameterCalculator,
        base::concepts::dense_vector KernelCoeffVector>
    [[nodiscard]] static auto evaluate(
        const distance_function_type& distance_function, const rbf_type& rbf,
        const LengthParameterCalculator& length_parameter_calculator,
        const operator_type& target_operator,
        util::vector_view<const variable_type> sample_variables,
        const KernelCoeffVector& kernel_coefficients) {
        return -operator_evaluator<TargetOperator, RBF,
            DistanceFunction>::evaluate(distance_function, rbf,
            length_parameter_calculator, target_operator.target_operator(),
            sample_variables, kernel_coefficients);
    }

    /*!
     * \brief Evaluate an operator for one sample point.
     *
     * \tparam FunctionValue Type of function values.
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     * \param[in] length_parameter Length parameter.
     * \param[in] target_operator Operator to evaluate.
     * \param[in] sample_variable Variable of the sample.
     * \param[in] kernel_coeff Coefficient of the kernel for the sample.
     * \return Evaluated function value.
     */
    template <typename FunctionValue>
    [[nodiscard]] static auto evaluate_for_one_sample(
        const distance_function_type& distance_function, const rbf_type& rbf,
        const kernel_value_type& length_parameter,
        const operator_type& target_operator,
        const variable_type& sample_variable,
        const FunctionValue& kernel_coeff) {
        return -operator_evaluator<TargetOperator, RBF,
            DistanceFunction>::evaluate_for_one_sample(distance_function, rbf,
            length_parameter, target_operator.target_operator(),
            sample_variable, kernel_coeff);
    }

    /*!
     * \brief Evaluate a polynomial.
     *
     * \tparam CoeffVector Type of the vector of coefficients of the polynomial.
     * \param[in] target_operator Operator to evaluate.
     * \param[in] term_generator Generator of polynomial terms.
     * \param[in] polynomial_coefficients Coefficients of the polynomial.
     * \return Evaluated polynomial value.
     */
    template <base::concepts::dense_vector CoeffVector>
    [[nodiscard]] static auto evaluate_polynomial(
        const operator_type& target_operator,
        const polynomial_term_generator<variable_dimensions>& term_generator,
        const CoeffVector& polynomial_coefficients) {
        return -operator_evaluator<TargetOperator, RBF,
            DistanceFunction>::evaluate_polynomial(target_operator
                                                       .target_operator(),
            term_generator, polynomial_coefficients);
    }

    /*!
     * \brief Evaluate a polynomial term.
     *
     * \param[in] target_operator Operator to evaluate.
     * \param[in] polynomial_term Polynomial term to evaluate.
     * \return Evaluated polynomial value.
     */
    template <int NumDimensions>
    [[nodiscard]] static auto evaluate_polynomial_term(
        const operator_type& target_operator,
        const polynomial_term<NumDimensions>& polynomial_term) {
        return -operator_evaluator<TargetOperator, RBF,
            DistanceFunction>::evaluate_polynomial_term(target_operator
                                                            .target_operator(),
            polynomial_term);
    }
};

/*!
 * \brief Specialization of num_collect::rbf::operators::operator_evaluator for
 * num_collect::rbf::operators::operator_sum.
 */
template <typename LeftOperator, typename RightOperator, concepts::rbf RBF,
    concepts::distance_function DistanceFunction>
struct operator_evaluator<operator_sum<LeftOperator, RightOperator>, RBF,
    DistanceFunction> {
    //! Type of the RBF.
    using rbf_type = RBF;

    //! Type of the distance function.
    using distance_function_type = DistanceFunction;

    //! Type of variables.
    using variable_type = typename DistanceFunction::variable_type;

    //! Type of kernel values.
    using kernel_value_type = typename rbf_type::scalar_type;

    //! Type of the operator.
    using operator_type = operator_sum<LeftOperator, RightOperator>;

    //! Dimensions of the variables.
    static constexpr index_type variable_dimensions =
        get_compile_time_size<variable_type>();

    /*!
     * \brief Evaluate an operator.
     *
     * \tparam LengthParameterCalculator Type of the calculator of length
     * parameters.
     * \tparam KernelCoeffVector Type of the vector of coefficients of kernels.
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     * \param[in] length_parameter_calculator Calculator of length parameters.
     * \param[in] target_operator Operator to evaluate.
     * \param[in] sample_variables Variables of samples.
     * \param[in] kernel_coefficients Coefficients of kernels.
     * \return
     */
    template <concepts::length_parameter_calculator LengthParameterCalculator,
        base::concepts::dense_vector KernelCoeffVector>
    [[nodiscard]] static auto evaluate(
        const distance_function_type& distance_function, const rbf_type& rbf,
        const LengthParameterCalculator& length_parameter_calculator,
        const operator_type& target_operator,
        util::vector_view<const variable_type> sample_variables,
        const KernelCoeffVector& kernel_coefficients) {
        return operator_evaluator<LeftOperator, RBF,
                   DistanceFunction>::evaluate(distance_function, rbf,
                   length_parameter_calculator, target_operator.left_operator(),
                   sample_variables, kernel_coefficients) +
            operator_evaluator<RightOperator, RBF, DistanceFunction>::evaluate(
                distance_function, rbf, length_parameter_calculator,
                target_operator.right_operator(), sample_variables,
                kernel_coefficients);
    }

    /*!
     * \brief Evaluate an operator for one sample point.
     *
     * \tparam FunctionValue Type of function values.
     * \param[in] distance_function Distance function.
     * \param[in] rbf RBF.
     * \param[in] length_parameter Length parameter.
     * \param[in] target_operator Operator to evaluate.
     * \param[in] sample_variable Variable of the sample.
     * \param[in] kernel_coeff Coefficient of the kernel for the sample.
     * \return Evaluated function value.
     */
    template <typename FunctionValue>
    [[nodiscard]] static auto evaluate_for_one_sample(
        const distance_function_type& distance_function, const rbf_type& rbf,
        const kernel_value_type& length_parameter,
        const operator_type& target_operator,
        const variable_type& sample_variable,
        const FunctionValue& kernel_coeff) {
        return operator_evaluator<LeftOperator, RBF,
                   DistanceFunction>::evaluate_for_one_sample(distance_function,
                   rbf, length_parameter, target_operator.left_operator(),
                   sample_variable, kernel_coeff) +
            operator_evaluator<RightOperator, RBF,
                DistanceFunction>::evaluate_for_one_sample(distance_function,
                rbf, length_parameter, target_operator.right_operator(),
                sample_variable, kernel_coeff);
    }

    /*!
     * \brief Evaluate a polynomial.
     *
     * \tparam CoeffVector Type of the vector of coefficients of the polynomial.
     * \param[in] target_operator Operator to evaluate.
     * \param[in] term_generator Generator of polynomial terms.
     * \param[in] polynomial_coefficients Coefficients of the polynomial.
     * \return Evaluated polynomial value.
     */
    template <base::concepts::dense_vector CoeffVector>
    [[nodiscard]] static auto evaluate_polynomial(
        const operator_type& target_operator,
        const polynomial_term_generator<variable_dimensions>& term_generator,
        const CoeffVector& polynomial_coefficients) {
        return operator_evaluator<LeftOperator, RBF,
                   DistanceFunction>::evaluate_polynomial(target_operator
                                                              .left_operator(),
                   term_generator, polynomial_coefficients) +
            operator_evaluator<RightOperator, RBF,
                DistanceFunction>::evaluate_polynomial(target_operator
                                                           .right_operator(),
                term_generator, polynomial_coefficients);
    }

    /*!
     * \brief Evaluate a polynomial term.
     *
     * \param[in] target_operator Operator to evaluate.
     * \param[in] polynomial_term Polynomial term to evaluate.
     * \return Evaluated polynomial value.
     */
    template <int NumDimensions>
    [[nodiscard]] static auto evaluate_polynomial_term(
        const operator_type& target_operator,
        const polynomial_term<NumDimensions>& polynomial_term) {
        return operator_evaluator<LeftOperator, RBF, DistanceFunction>::
                   evaluate_polynomial_term(
                       target_operator.left_operator(), polynomial_term) +
            operator_evaluator<RightOperator, RBF, DistanceFunction>::
                evaluate_polynomial_term(
                    target_operator.right_operator(), polynomial_term);
    }
};

}  // namespace num_collect::rbf::operators
