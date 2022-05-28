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

namespace num_collect::logging::impl {

/*!
 * \brief Class to handle layers of iterations.
 *
 * \thread_safety Thread safe only for different objects.
 */
class iteration_layer_handler {
public:
    /*!
     * \brief Construct.
     */
    iteration_layer_handler() = default;

    /*!
     * \brief Copy constructor.
     *
     * This actually doesn't copy objects, but initialize this object as in
     * default constructor.
     */
    iteration_layer_handler(const iteration_layer_handler& /*obj*/)
        : iteration_layer_handler() {}

    /*!
     * \brief Move constructor.
     *
     * \param[in] obj Object to move from.
     */
    iteration_layer_handler(iteration_layer_handler&& obj) noexcept
        : node_(std::move(obj.node_)) {
        node_->reset();
    }

    /*!
     * \brief Copy assignment operator.
     *
     * \return This.
     *
     * This actually doesn nothing.
     */
    [[nodiscard]] auto operator=(  // NOLINT
        const iteration_layer_handler& /*obj*/) noexcept
        -> iteration_layer_handler& {
        // No operation.
        return *this;
    }

    /*!
     * \brief Copy assignment operator.
     *
     * \return This.
     *
     * This actually doesn nothing.
     */
    [[nodiscard]] auto operator=(iteration_layer_handler&& /*obj*/) noexcept
        -> iteration_layer_handler& {
        // No operation.
        return *this;
    }

    /*!
     * \brief Destruct.
     */
    ~iteration_layer_handler() noexcept = default;

    /*!
     * \brief Set to the state of iterating.
     */
    void start_iteration() const noexcept { node_->start_iteration(); }

    /*!
     * \brief Set to the state of not iterating.
     */
    void finish_iteration() const noexcept { node_->finish_iteration(); }

    /*!
     * \brief Initialize the lower layer.
     *
     * \param[in] lower_layer Handler of the lower layer.
     */
    void initialize_lower_layer(
        const iteration_layer_handler& lower_layer) const noexcept {
        lower_layer.node_->set_parent(node_);
    }

    /*!
     * \brief Check whether one of upper layers is iterating.
     *
     * \retval true One of upper layers is iterating.
     * \retval false None of upper layers is iterating.
     */
    [[nodiscard]] auto is_upper_layer_iterating() const noexcept -> bool {
        return node_->is_ancestor_node_iterating();
    }

private:
    //! Class of internal nodes.
    class node {
    public:
        /*!
         * \brief Set to the state of iterating.
         */
        void start_iteration() noexcept { is_iterating_ = true; }

        /*!
         * \brief Set to the state of not iterating.
         */
        void finish_iteration() noexcept { is_iterating_ = false; }

        /*!
         * \brief Set the parent node.
         *
         * \param[in] parent Parent node.
         */
        void set_parent(const std::shared_ptr<node>& parent) noexcept {
            parent_ = parent;
        }

        /*!
         * \brief Check whether one of ancestor nodes is iterating.
         *
         * \retval true At least one of ancestor nodes is iterating.
         * \retval false None of ancestor nodes is iterating.
         */
        [[nodiscard]] auto is_ancestor_node_iterating() const noexcept -> bool {
            const auto locked = parent_.lock();
            if (!locked) {
                return false;
            }
            return locked->is_this_or_ancestor_node_iterating();
        }

        /*!
         * \brief Reset the internal state.
         */
        void reset() noexcept {
            is_iterating_ = false;
            parent_.reset();
        }

    private:
        /*!
         * \brief Check whether this node or one of ancestor nodes is iterating.
         *
         * \retval true This node or at least one of ancestor nodes is
         * iterating.
         * \retval false None of this node nor ancestor nodes is iterating.
         */
        [[nodiscard]] auto is_this_or_ancestor_node_iterating() const noexcept
            -> bool {
            if (is_iterating_) {
                return true;
            }
            return is_ancestor_node_iterating();
        }

        //! Whether this layer is iterating.
        std::atomic<bool> is_iterating_{false};

        //! Parent node.
        std::weak_ptr<node> parent_{};
    };

    //! Whether this layer is iterating.
    std::shared_ptr<node> node_{std::make_shared<node>()};
};

}  // namespace num_collect::logging::impl
