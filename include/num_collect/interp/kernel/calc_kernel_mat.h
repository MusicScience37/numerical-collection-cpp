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
 * \brief Definition of calc_kernel_mat function.
 */
#pragma once

#include <Eigen/Core>

#include "num_collect/base/assert.h"
#include "num_collect/base/index_type.h"

namespace num_collect::interp::kernel {

namespace impl {

/*!
 * \brief Class of functor to calculate kernel matrices.
 *
 * \tparam Kernel Type of the kernel.
 * \tparam Container Type of the list of variables.
 */
template <typename Kernel, typename Container>
class calc_kernel_mat_functor {
public:
    //! Type of the kernel.
    using kernel_type = Kernel;

    //! Type of the list of variables.
    using container_type = Container;

    //! Type of values.
    using value_type = typename kernel_type::value_type;

    //! Type of resulting matrix.
    using result_type = Eigen::MatrixX<value_type>;

    /*!
     * \brief Construct.
     *
     * \param[in] kernel Kernel.
     * \param[in] list List of variables.
     */
    calc_kernel_mat_functor(
        const kernel_type& kernel, const container_type& list)
        : kernel_(kernel), list_(list) {}

    /*!
     * \brief Get an element of the vector.
     *
     * \param[in] row Row index.
     * \param[in] col Column index.
     * \return Element.
     */
    [[nodiscard]] auto operator()(index_type row, index_type col) const
        -> value_type {
        return kernel_(list_[static_cast<std::size_t>(row)],
            list_[static_cast<std::size_t>(col)]);
    }

private:
    //! Kernel.
    const kernel_type& kernel_;

    //! List of variables.
    const container_type& list_;
};

}  // namespace impl

/*!
 * \brief Calculate kernel matrix.
 *
 * \tparam Kernel Type of the kernel.
 * \tparam Container Type of the list of variables.
 * \param[in] kernel Kernel.
 * \param[in] list List of variables.
 * \return Expression of kernel matrix.
 */
template <typename Kernel, typename Container>
[[nodiscard]] inline auto calc_kernel_mat(
    const Kernel& kernel, const Container& list)
    -> Eigen::CwiseNullaryOp<impl::calc_kernel_mat_functor<Kernel, Container>,
        typename impl::calc_kernel_mat_functor<Kernel,
            Container>::result_type> {
    NUM_COLLECT_ASSERT(!list.empty());

    using result_type =
        typename impl::calc_kernel_mat_functor<Kernel, Container>::result_type;
    return result_type::NullaryExpr(static_cast<index_type>(list.size()),
        static_cast<index_type>(list.size()),
        impl::calc_kernel_mat_functor<Kernel, Container>(kernel, list));
}

}  // namespace num_collect::interp::kernel
