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
 * \brief Example of tikhonov class with blurred sine test problem.
 */
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include <Eigen/Core>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>  // IWYU pragma: keep

#include "num_collect/base/index_type.h"
#include "num_collect/regularization/explicit_l_curve.h"
#include "num_collect/regularization/tikhonov.h"
#include "num_prob_collect/regularization/blur_sine.h"

auto main() -> int {
    constexpr int prec = 15;
    std::cout << std::setprecision(prec);

    using coeff_type = Eigen::MatrixXd;
    using data_type = Eigen::VectorXd;
    static constexpr num_collect::index_type solution_size = 60;
    static constexpr num_collect::index_type data_size = 60;
    constexpr double error_rate = 0.01;

    // create test problem
    const auto prob =
        num_prob_collect::regularization::blur_sine(data_size, solution_size);
    std::mt19937 engine;  // NOLINT
    std::normal_distribution<double> dist{0.0,
        std::sqrt(prob.data().squaredNorm() /
            static_cast<double>(prob.data().size()) * error_rate)};
    auto data_with_error = prob.data();
    for (num_collect::index_type i = 0; i < data_with_error.size(); ++i) {
        data_with_error(i) += dist(engine);
    }

    // solve
    using solver_type =
        num_collect::regularization::tikhonov<coeff_type, data_type>;
    solver_type tikhonov;
    tikhonov.compute(prob.coeff(), data_with_error);

    using searcher_type =
        num_collect::regularization::explicit_l_curve<solver_type>;
    searcher_type searcher{tikhonov};
    searcher.search();
    Eigen::VectorXd solution;
    searcher.solve(solution);
    std::cout << "Optimal parameter: " << searcher.opt_param() << std::endl;
    std::cout << "Error rate: "
              << (solution - prob.solution()).squaredNorm() /
            prob.solution().squaredNorm()
              << std::endl;

    // plot graphs
    std::vector<double> param_list;
    std::vector<std::string> type_list;
    std::vector<double> value_list;
    std::vector<double> residual_norm_list;
    std::vector<double> regularization_term_list;
    const auto [min_param, max_param] = tikhonov.param_search_region();
    constexpr std::size_t num_samples = 101;
    for (std::size_t i = 0; i < num_samples; ++i) {
        const double rate =
            static_cast<double>(i) / static_cast<double>(num_samples - 1);
        const double param = min_param * std::pow(max_param / min_param, rate);

        const double residual_norm = tikhonov.residual_norm(param);
        param_list.push_back(param);
        type_list.emplace_back("residual norm");
        value_list.push_back(residual_norm);
        residual_norm_list.push_back(residual_norm);

        const double regularization_term = tikhonov.regularization_term(param);
        param_list.push_back(param);
        type_list.emplace_back("regularization term");
        value_list.push_back(regularization_term);
        regularization_term_list.push_back(regularization_term);

        const double curvature_value = tikhonov.l_curve_curvature(param);
        param_list.push_back(param);
        type_list.emplace_back("curvature");
        value_list.push_back(curvature_value);

        tikhonov.solve(param, solution);
        const double error = (solution - prob.solution()).squaredNorm() /
            prob.solution().squaredNorm();
        param_list.push_back(param);
        type_list.emplace_back("error rate");
        value_list.push_back(error);
    }

    pybind11::scoped_interpreter interpreter;
    auto pd = pybind11::module::import("pandas");
    auto px = pybind11::module::import("plotly.express");

    std::unordered_map<std::string, pybind11::object> data;
    data.try_emplace("param", pybind11::cast(param_list));
    data.try_emplace("type", pybind11::cast(type_list));
    data.try_emplace("value", pybind11::cast(value_list));

    auto fig = px.attr("line")(pybind11::arg("data_frame") = data,
        pybind11::arg("x") = "param", pybind11::arg("y") = "value",
        pybind11::arg("color") = "type", pybind11::arg("log_x") = true,
        pybind11::arg("log_y") = true);

    fig.attr("write_html")("blur_sine_tikhonov_norms.html");
    fig.attr("write_image")("blur_sine_tikhonov_norms.png");

    fig = px.attr("line")(pybind11::arg("x") = residual_norm_list,
        pybind11::arg("y") = regularization_term_list,
        pybind11::arg("log_x") = true, pybind11::arg("log_y") = true,
        pybind11::arg("labels") = std::unordered_map<std::string, std::string>{
            {"x", "Residual Norm"}, {"y", "Regularization Term"}});

    fig.attr("write_html")("blur_sine_tikhonov_l_curve.html");
    fig.attr("write_image")("blur_sine_tikhonov_l_curve.png");
}
