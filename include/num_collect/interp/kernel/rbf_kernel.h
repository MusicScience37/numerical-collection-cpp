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
 * \brief Definition of rbf_kernel class
 */
#pragma once

#include <cmath>
#include <cstddef>
#include <limits>
#include <type_traits>  // IWYU pragma: keep
#include <utility>

#include "num_collect/interp/kernel/concepts/distance.h"  // IWYU pragma: keep
#include "num_collect/interp/kernel/concepts/rbf.h"       // IWYU pragma: keep
#include "num_collect/util/assert.h"

namespace num_collect::interp::kernel {

/*!
 * \brief Class of kernels using radial basis functions (RBF).
 *
 * \tparam Distance Type of calculators of distances.
 * \tparam Rbf Type of RBF.
 */
template <concepts::distance Distance, concepts::rbf Rbf>
class rbf_kernel {
public:
    //! Type of calculators of distances.
    using distance_type = Distance;

    //! Type of RBF.
    using rbf_type = Rbf;

    //! Type of variables.
    using variable_type = typename distance_type::variable_type;

    //! Type of kernel values.
    using value_type = typename rbf_type::value_type;

    static_assert(std::is_same_v<typename distance_type::value_type,
        typename rbf_type::arg_type>);

    //! Type of length parameters.
    using len_param_type = typename distance_type::value_type;

    //! Type of kernel parameters.
    using kernel_param_type = len_param_type;

    /*!
     * \brief Constructor.
     *
     * \param[in] dist Calculator of distances.
     * \param[in] rbf RBF.
     * \param[in] len_param Length parameter.
     */
    explicit rbf_kernel(const distance_type& dist = distance_type(),
        const rbf_type& rbf = rbf_type(),
        const len_param_type& len_param = static_cast<len_param_type>(1))
        : dist_(dist), rbf_(rbf), len_param_(len_param) {}

    /*!
     * \brief Calculate a kernel value.
     *
     * \param[in] var1 Variable.
     * \param[in] var2 Variable.
     * \return Kernel value.
     */
    [[nodiscard]] auto operator()(const variable_type& var1,
        const variable_type& var2) const -> value_type {
        return rbf_(dist_(var1, var2) / len_param_);
    }

    /*!
     * \brief Get the length parameter.
     *
     * \return Length parameter.
     */
    [[nodiscard]] auto len_param() const noexcept -> const len_param_type& {
        return len_param_;
    }

    /*!
     * \brief Set the length parameter.
     *
     * \param[in] value Value.
     * \return This.
     */
    auto len_param(const len_param_type& value) -> rbf_kernel& {
        NUM_COLLECT_ASSERT(value > static_cast<len_param_type>(0));
        len_param_ = value;
        return *this;
    }

    /*!
     * \brief Get the kernel parameter (logarithm of length parameter).
     *
     * \return Kernel parameter.
     */
    [[nodiscard]] auto kernel_param() const noexcept -> kernel_param_type {
        using std::log10;
        return log10(len_param());
    }

    /*!
     * \brief Set the kernel parameter (logarithm of length parameter).
     *
     * \param[in] value Value.
     * \return This.
     */
    auto kernel_param(const kernel_param_type& value) -> rbf_kernel& {
        using std::pow;
        static const auto base = static_cast<kernel_param_type>(10);
        len_param(pow(base, value));
        return *this;
    }

    /*!
     * \brief Determine search region of kernel parameters.
     *
     * \tparam Container Type of list.
     * \param[in] list List of variables.
     * \return Search region (lower and upper bounds).
     */
    template <typename Container>
    [[nodiscard]] auto kernel_param_search_region(const Container& list) const
        -> std::pair<kernel_param_type, kernel_param_type> {
        NUM_COLLECT_ASSERT(list.size() > 1);

        len_param_type max_min_dist =
            std::numeric_limits<len_param_type>::min();
        for (std::size_t i = 0; i < list.size(); ++i) {
            len_param_type min_dist =
                std::numeric_limits<len_param_type>::max();
            for (std::size_t j = 0; j < list.size(); ++j) {
                if (i == j) {
                    continue;
                }
                const auto dist = dist_(list[i], list[j]);
                if (dist < min_dist) {
                    min_dist = dist;
                }
            }
            if (min_dist > max_min_dist) {
                max_min_dist = min_dist;
            }
        }

        using std::log10;
        constexpr auto coeff_lower = static_cast<len_param_type>(1e-3);
        constexpr auto coeff_upper = static_cast<len_param_type>(1e+3);
        return {log10(coeff_lower * max_min_dist),
            log10(coeff_upper * max_min_dist)};
    }

private:
    //! Calculator of distances.
    distance_type dist_;

    //! RBF.
    rbf_type rbf_;

    //! Length parameter.
    len_param_type len_param_;
};

}  // namespace num_collect::interp::kernel
