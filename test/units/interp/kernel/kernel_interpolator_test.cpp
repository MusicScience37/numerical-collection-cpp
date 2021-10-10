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
 * \brief Test of kernel_interpolator class.
 */
#include "num_collect/interp/kernel/kernel_interpolator.h"

#include <Eigen/Core>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating.hpp>

#include "eigen_approx.h"
#include "is_finite.h"
#include "num_collect/interp/kernel/calc_kernel_mat.h"
#include "num_collect/interp/kernel/euclidean_distance.h"
#include "num_collect/interp/kernel/gaussian_rbf.h"
#include "num_collect/interp/kernel/rbf_kernel.h"

TEST_CASE("num_collect::interp::kernel::kernel_interpolator") {
    using num_collect::interp::kernel::euclidean_distance;
    using num_collect::interp::kernel::gaussian_rbf;
    using num_collect::interp::kernel::kernel_interpolator;
    using num_collect::interp::kernel::rbf_kernel;

    const auto vars = std::vector<double>{0.0, 0.1, 0.2, 0.4, 0.6, 1.0};
    const auto data = Eigen::VectorXd{{0.0, 0.2, 0.4, 0.7, 1.0, 2.0}};

    using kernel_type =
        rbf_kernel<euclidean_distance<double>, gaussian_rbf<double>>;

    SECTION("interpolate using defaults") {
        auto interpolator = kernel_interpolator<kernel_type>();

        interpolator.compute(vars, data);
        REQUIRE(interpolator.reg_param() == 0.0);  // NOLINT
        REQUIRE(interpolator.kernel().len_param() > 0.0);

        constexpr double tol_error = 1e-8;
        for (std::size_t i = 0; i < vars.size(); ++i) {
            INFO("i = " << i);
            REQUIRE_THAT(interpolator(vars[i]),
                Catch::Matchers::WithinAbs(
                    data(static_cast<num_collect::index_type>(i)), tol_error));
        }
    }

    SECTION("interpolate with fixed kernel parameters") {
        auto interpolator = kernel_interpolator<kernel_type>();
        constexpr double len_param = 0.1;
        interpolator.fix_kernel_param(std::log10(len_param));

        interpolator.compute(vars, data);
        REQUIRE(interpolator.reg_param() == 0.0);                 // NOLINT
        REQUIRE(interpolator.kernel().len_param() == len_param);  // NOLINT

        constexpr double tol_error = 1e-4;
        for (std::size_t i = 0; i < vars.size(); ++i) {
            INFO("i = " << i);
            REQUIRE_THAT(interpolator(vars[i]),
                Catch::Matchers::WithinAbs(
                    data(static_cast<num_collect::index_type>(i)), tol_error));
        }
    }

    SECTION("interpolate with a fixed regularization parameter") {
        auto interpolator = kernel_interpolator<kernel_type>();
        constexpr double len_param = 0.1;
        interpolator.fix_kernel_param(std::log10(len_param));
        constexpr double reg_param = 1e-4;
        interpolator.regularize_with(reg_param);

        interpolator.compute(vars, data);
        REQUIRE(interpolator.reg_param() == reg_param);
        REQUIRE(interpolator.kernel().len_param() > 0.0);

        constexpr double tol_error = 1e-2;
        for (std::size_t i = 0; i < vars.size(); ++i) {
            INFO("i = " << i);
            REQUIRE_THAT(interpolator(vars[i]),
                Catch::Matchers::WithinAbs(
                    data(static_cast<num_collect::index_type>(i)), tol_error));
        }
    }

    SECTION("interpolate with automatic regularization") {
        auto interpolator = kernel_interpolator<kernel_type>();
        constexpr double len_param = 0.1;
        interpolator.fix_kernel_param(std::log10(len_param));
        interpolator.regularize_automatically();

        interpolator.compute(vars, data);
        REQUIRE(interpolator.reg_param() > 0.0);
        REQUIRE(interpolator.kernel().len_param() > 0.0);

        constexpr double tol_error = 1e-2;
        for (std::size_t i = 0; i < vars.size(); ++i) {
            INFO("i = " << i);
            REQUIRE_THAT(interpolator(vars[i]),
                Catch::Matchers::WithinAbs(
                    data(static_cast<num_collect::index_type>(i)), tol_error));
        }
    }

    SECTION("interpolate with automatic regularization and kernel parameters") {
        auto interpolator = kernel_interpolator<kernel_type>();
        interpolator.regularize_automatically();

        interpolator.compute(vars, data);
        REQUIRE(interpolator.reg_param() > 0.0);
        REQUIRE(interpolator.kernel().len_param() > 0.0);

        constexpr double tol_error = 1e-2;
        for (std::size_t i = 0; i < vars.size(); ++i) {
            INFO("i = " << i);
            REQUIRE_THAT(interpolator(vars[i]),
                Catch::Matchers::WithinAbs(
                    data(static_cast<num_collect::index_type>(i)), tol_error));
        }
    }

    SECTION("evaluate variance without regularization") {
        auto interpolator = kernel_interpolator<kernel_type>();
        constexpr double len_param = 0.1;
        interpolator.fix_kernel_param(std::log10(len_param));
        interpolator.disable_regularization();

        interpolator.compute(vars, data);

        constexpr double tol_error = 1e-4;
        for (std::size_t i = 0; i < vars.size(); ++i) {
            INFO("i = " << i);
            const auto [mean, variance] =
                interpolator.evaluate_mean_and_variance_on(vars[i]);
            REQUIRE_THAT(mean,
                Catch::Matchers::WithinAbs(
                    data(static_cast<num_collect::index_type>(i)), tol_error));

            REQUIRE(variance >= 0.0);
            REQUIRE_THAT(variance, Catch::Matchers::WithinAbs(0.0, tol_error));
        }
    }

    SECTION("evaluate variance with regularization") {
        auto interpolator = kernel_interpolator<kernel_type>();
        constexpr double len_param = 0.1;
        interpolator.fix_kernel_param(std::log10(len_param));
        constexpr double reg_param = 1e-4;
        interpolator.regularize_with(reg_param);

        interpolator.compute(vars, data);

        constexpr double tol_error = 1e-2;
        for (std::size_t i = 0; i < vars.size(); ++i) {
            INFO("i = " << i);
            const auto [mean, variance] =
                interpolator.evaluate_mean_and_variance_on(vars[i]);
            REQUIRE_THAT(mean,
                Catch::Matchers::WithinAbs(
                    data(static_cast<num_collect::index_type>(i)), tol_error));

            REQUIRE(variance > 0.0);
            REQUIRE_THAT(variance,
                Catch::Matchers::WithinAbs(
                    interpolator.common_coeff() * reg_param, tol_error));
        }
    }
}
