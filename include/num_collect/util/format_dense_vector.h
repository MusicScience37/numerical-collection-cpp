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
 * \brief Definition of format_dense_vector function.
 */
#pragma once

#include <Eigen/Core>
#include <fmt/base.h>

#include "num_collect/base/exception.h"
#include "num_collect/base/precondition.h"
#include "num_collect/logging/logging_macros.h"

namespace num_collect::util {

namespace impl {

/*!
 * \brief Class of expressions to format dense vectors.
 *
 * \tparam Vector Type of the vector.
 */
template <typename Vector>
class dense_vector_format_view {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] vec Vector.
     */
    explicit dense_vector_format_view(const Vector& vec) : vec_(&vec) {}

    /*!
     * \brief Get the vector.
     *
     * \return Vector.
     */
    [[nodiscard]] auto vec() const noexcept -> const Vector& { return *vec_; }

private:
    //! Vector.
    const Vector* vec_;
};

}  // namespace impl

/*!
 * \brief Format a dense vector.
 *
 * \tparam Vector Type of the vector.
 * \param[in] vec Vector.
 * \return Expression for formatting using fmt library.
 *
 * \note Format of each element can be specified in fmt's format strings.
 */
template <typename Vector>
[[nodiscard]] inline auto format_dense_vector(
    const Eigen::DenseBase<Vector>& vec) {
    NUM_COLLECT_PRECONDITION(
        vec.cols() == 1, "format_dense_vector requires a vector.");
    return impl::dense_vector_format_view<Vector>(vec.derived());
}

}  // namespace num_collect::util

namespace fmt {

/*!
 * \brief fmt::formatter for num_collect::util::impl::dense_vector_format_view.
 *
 * \tparam Vector Type of the vector.
 */
template <typename Vector>
struct formatter<num_collect::util::impl::dense_vector_format_view<Vector>>
    : public formatter<typename Vector::Scalar> {
public:
    /*!
     * \brief Format a value.
     *
     * \param[in] val Value.
     * \param[in] context Context.
     * \return Output iterator after formatting.
     */
    auto format(
        const num_collect::util::impl::dense_vector_format_view<Vector>& val,
        format_context& context) const -> decltype(context.out()) {
        const auto& vec = val.vec();
        return format_impl(vec, context);
    }

private:
    /*!
     * \brief Format a vector.
     *
     * \param[in] vec Vector.
     * \param[in] context Context.
     * \return Output iterator after formatting.
     */
    auto format_impl(const Vector& vec, format_context& context) const
        -> decltype(context.out()) {
        const Eigen::Index size = vec.size();
        auto out = context.out();
        *out = '[';
        ++out;
        for (Eigen::Index i = 0; i < size; ++i) {
            if (i != 0) {
                out = write_comma(out);
            }
            context.advance_to(out);
            out = formatter<typename Vector::Scalar>::format(vec(i), context);
        }
        *out = ']';
        ++out;
        return out;
    }

    /*!
     * \brief Write a comma.
     *
     * \param[in] out Output iterator.
     * \return Output iterator.
     */
    static auto write_comma(format_context::iterator out)
        -> format_context::iterator {
        *out = ',';
        ++out;
        *out = ' ';
        ++out;
        return out;
    }
};

}  // namespace fmt
