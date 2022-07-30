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
 * \brief Definition of node class.
 */
#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "num_collect/base/concepts/real_scalar.h"  // IWYU pragma: keep
#include "num_collect/util/assert.h"

namespace num_collect::auto_diff::backward::graph {

template <base::concepts::real_scalar Scalar>
class node;

/*!
 * \brief Type of pointers of nodes.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
using node_ptr = std::shared_ptr<const node<Scalar>>;

/*!
 * \brief class to save information of child nodes.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class child_node {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Construct.
     *
     * \param[in] node Child node.
     * \param[in] sensitivity Partial differential coefficient of the parent
     * node by the child node.
     */
    child_node(node_ptr<scalar_type> node, const scalar_type& sensitivity)
        : node_(std::move(node)), sensitivity_(sensitivity) {
        NUM_COLLECT_ASSERT(node_);
    }

    /*!
     * \brief Get the child node.
     *
     * \return Child node.
     */
    [[nodiscard]] auto node() const noexcept -> const node_ptr<scalar_type>& {
        return node_;
    }

    /*!
     * \brief Get the partial differential coefficient of the parent node by the
     * child node.
     *
     * \return Partial differential coefficient of the parent node by the child
     * node.
     */
    [[nodiscard]] auto sensitivity() const noexcept -> const scalar_type& {
        return sensitivity_;
    }

private:
    //! Child node.
    node_ptr<scalar_type> node_;

    //! Partial differential coefficient of the parent node by the child node.
    scalar_type sensitivity_;
};

/*!
 * \brief Class of nodes in graphs of automatic differentiation.
 *
 * \tparam Scalar Type of scalars.
 */
template <base::concepts::real_scalar Scalar>
class node {
public:
    //! Type of scalars.
    using scalar_type = Scalar;

    /*!
     * \brief Construct.
     *
     * \param[in] children Child nodes.
     */
    explicit node(std::vector<child_node<scalar_type>> children =
                      std::vector<child_node<scalar_type>>())
        : children_(std::move(children)) {}

    /*!
     * \brief Get the child nodes.
     *
     * \return Child nodes.
     */
    [[nodiscard]] auto children() const noexcept
        -> const std::vector<child_node<scalar_type>>& {
        return children_;
    }

private:
    //! Child nodes.
    std::vector<child_node<scalar_type>> children_;
};

namespace impl {

/*!
 * \brief Create a node.
 *
 * \tparam Scalar Type of scalars.
 * \param[in] children Child nodes.
 * \return Created node.
 */
template <base::concepts::real_scalar Scalar>
[[nodiscard]] inline auto create_node_impl(
    std::vector<child_node<Scalar>>& children) -> node_ptr<Scalar> {
    return std::make_shared<node<Scalar>>(std::move(children));
}

/*!
 * \brief Create a node.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Args Types of remaining arguments.
 * \param[in] children Child nodes.
 * \param[in] node Child node.
 * \param[in] sensitivity Partial differential coefficient of the parent node by
 * the child node.
 * \param[in] args Remaining arguments.
 * \return Created node.
 */
template <base::concepts::real_scalar Scalar, typename... Args>
[[nodiscard]] inline auto create_node_impl(
    std::vector<child_node<Scalar>>& children, node_ptr<Scalar> node,
    const Scalar& sensitivity, Args&&... args) -> node_ptr<Scalar> {
    children.emplace_back(std::move(node), sensitivity);
    return create_node_impl(children, std::forward<Args>(args)...);
}

}  // namespace impl

/*!
 * \brief Create a node.
 *
 * \tparam Scalar Type of scalars.
 * \tparam Args Types of arguments.
 * \param[in] args Arguments.
 * \return Created node.
 */
template <base::concepts::real_scalar Scalar, typename... Args>
[[nodiscard]] inline auto create_node(Args&&... args) -> node_ptr<Scalar> {
    std::vector<child_node<Scalar>> children;
    return impl::create_node_impl(children, std::forward<Args>(args)...);
}

}  // namespace num_collect::auto_diff::backward::graph
