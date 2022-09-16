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
 * \brief Definition of iteration_layer_handler class.
 */
#pragma once

#include <atomic>
#include <memory>
#include <type_traits>
#include <utility>

namespace num_collect::logging::impl {

/*!
 * \brief Class to handle layers of iterations.
 *
 * \thread_safety Thread safe only for different objects.
 */
class iteration_layer_handler {
public:
    /*!
     * \brief Constructor.
     */
    iteration_layer_handler() = default;

    iteration_layer_handler(const iteration_layer_handler&) = delete;
    auto operator=(const iteration_layer_handler&) = delete;

    /*!
     * \brief Move constructor.
     */
    iteration_layer_handler(
        iteration_layer_handler&& /*obj*/) noexcept = default;

    /*!
     * \brief Copy assignment operator.
     *
     * \return This.
     */
    auto operator=(iteration_layer_handler&& /*obj*/) noexcept
        -> iteration_layer_handler& = default;

    /*!
     * \brief Destructor.
     */
    ~iteration_layer_handler() noexcept = default;

    /*!
     * \brief Set this node to an iterative algorithm.
     */
    void set_iterative() const noexcept { node_->set_iterative(); }

    /*!
     * \brief Initialize the lower layer.
     *
     * \param[in] lower_layer Handler of the lower layer.
     */
    void initialize_lower_layer(iteration_layer_handler& lower_layer) noexcept {
        lower_layer.node_->set_parent(node_);
    }

    /*!
     * \brief Check whether one of upper layers is iterative.
     *
     * \retval true One of upper layers is iterative.
     * \retval false None of upper layers is iterative.
     */
    [[nodiscard]] auto is_upper_layer_iterative() const noexcept -> bool {
        return node_->is_ancestor_node_iterative();
    }

private:
    //! Class of internal nodes.
    class node {
    public:
        /*!
         * \brief Set this node to an iterative algorithm.
         */
        void set_iterative() noexcept { is_iterative_ = true; }

        /*!
         * \brief Set the parent node.
         *
         * \param[in] parent Parent node.
         */
        void set_parent(const std::shared_ptr<node>& parent) noexcept {
            parent_ = parent;
        }

        /*!
         * \brief Check whether one of ancestor nodes is iterative.
         *
         * \retval true At least one of ancestor nodes is iterative.
         * \retval false None of ancestor nodes is iterative.
         */
        [[nodiscard]] auto is_ancestor_node_iterative() const noexcept -> bool {
            const auto locked = parent_.lock();
            if (!locked) {
                return false;
            }
            return locked->is_this_or_ancestor_node_iterative();
        }

        /*!
         * \brief Reset the internal state.
         */
        void reset() noexcept {
            is_iterative_ = false;
            parent_.reset();
        }

    private:
        /*!
         * \brief Check whether this node or one of ancestor nodes is iterative.
         *
         * \retval true This node or at least one of ancestor nodes is
         * iterative.
         * \retval false None of this node nor ancestor nodes is iterative.
         */
        [[nodiscard]] auto is_this_or_ancestor_node_iterative() const noexcept
            -> bool {
            if (is_iterative_) {
                return true;
            }
            return is_ancestor_node_iterative();
        }

        //! Whether this layer is iterative.
        std::atomic<bool> is_iterative_{false};

        //! Parent node.
        std::weak_ptr<node> parent_{};
    };

    //! This node.
    std::shared_ptr<node> node_{std::make_shared<node>()};
};

}  // namespace num_collect::logging::impl
