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
 * \brief Example of 1-dimensional RBF interpolation.
 */
#include <array>
#include <cmath>
#include <functional>
#include <vector>

#include <Eigen/Core>
#include <pybind11/eigen.h>  // IWYU pragma: keep
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // IWYU pragma: keep

#include "num_collect/base/index_type.h"
#include "num_collect/interp/kernel/euclidean_distance.h"
#include "num_collect/interp/kernel/gaussian_rbf.h"
#include "num_collect/interp/kernel/kernel_interpolator.h"
#include "num_collect/interp/kernel/rbf_kernel.h"

auto main() -> int {
    using num_collect::interp::kernel::euclidean_distance;
    using num_collect::interp::kernel::gaussian_rbf;
    using num_collect::interp::kernel::kernel_interpolator;
    using num_collect::interp::kernel::rbf_kernel;

    using kernel_type =
        rbf_kernel<euclidean_distance<double>, gaussian_rbf<double>>;

    const auto vars = std::vector<double>{0.0, 0.1, 0.5, 0.4, 1.2, 1.0};
    const auto data = Eigen::VectorXd{{0.0, 0.2, 0.5, 0.7, 1.0, 2.0}};

    auto interpolator = kernel_interpolator<kernel_type>();
    interpolator.compute(vars, data);

    constexpr num_collect::index_type num_samples = 201;
    const Eigen::VectorXd sample_vars =
        Eigen::VectorXd::LinSpaced(num_samples, -0.1, 1.3);
    Eigen::VectorXd sample_data = Eigen::VectorXd::Zero(num_samples);
    Eigen::VectorXd sample_upper = Eigen::VectorXd::Zero(num_samples);
    Eigen::VectorXd sample_lower = Eigen::VectorXd::Zero(num_samples);
    for (num_collect::index_type i = 0; i < num_samples; ++i) {
        const auto [mean, var] =
            interpolator.evaluate_mean_and_variance_on(sample_vars(i));
        const auto err = 3.0 * std::sqrt(var);
        sample_data(i) = mean;
        sample_upper(i) = mean + err;
        sample_lower(i) = mean - err;
    }

    pybind11::scoped_interpreter interpreter;
    auto go = pybind11::module::import("plotly.graph_objects");
    auto fig = go.attr("Figure")();

    // upper and lower limits
    fig.attr("add_trace")(go.attr("Scatter")(pybind11::arg("x") = sample_vars,
        pybind11::arg("y") = sample_lower, pybind11::arg("mode") = "lines",
        pybind11::arg("name") = "Lower bound (3 sigma)"));
    fig.attr("add_trace")(go.attr("Scatter")(pybind11::arg("x") = sample_vars,
        pybind11::arg("y") = sample_upper, pybind11::arg("mode") = "lines",
        pybind11::arg("name") = "Uppper bound (3 sigma)",
        pybind11::arg("fill") = "tonexty"));

    // interpolated line
    fig.attr("add_trace")(go.attr("Scatter")(pybind11::arg("x") = sample_vars,
        pybind11::arg("y") = sample_data, pybind11::arg("mode") = "lines",
        pybind11::arg("name") = "Interpolation"));

    // inputs
    fig.attr("add_trace")(go.attr("Scatter")(pybind11::arg("x") = vars,
        pybind11::arg("y") = data, pybind11::arg("mode") = "markers",
        pybind11::arg("name") = "Input data"));

    fig.attr("update_layout")(pybind11::arg("title") = "RBF Interpolation",
        pybind11::arg("xaxis_title") = "x", pybind11::arg("yaxis_title") = "y");

    fig.attr("write_html")("interp_rbf_1dim.html");
    fig.attr("write_image")("interp_rbf_1dim.png");
}
