/*
 * Copyright 2025 MusicScience37 (Kenta Kabashima)
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
 * \brief Experiment to calculate the derivatives of 2D images used in 2nd order
 * TGV regularization.
 *
 * This experiment aims to check the correctness of the implementation
 * of the derivatives of 2D images.
 */
#include <Eigen/Core>
#include <fmt/format.h>
#include <lyra/lyra.hpp>
#include <plotly_plotter/color_scales.h>
#include <plotly_plotter/eigen.h>
#include <plotly_plotter/figure.h>
#include <plotly_plotter/write_html.h>

#include "num_collect/base/index_type.h"
#include "num_prob_collect/regularization/generate_sparse_sample_image.h"
#include "num_prob_collect/regularization/sparse_diff_matrix_2d.h"
#include "num_prob_collect/regularization/tgv2_second_derivative_matrix_2d.h"

constexpr num_collect::index_type rows = 40;
constexpr num_collect::index_type cols = 40;

static void visualize_derivatives(const Eigen::MatrixXd& origin,
    const Eigen::SparseMatrix<double>& first_derivative_matrix,
    const Eigen::SparseMatrix<double>& second_derivative_matrix,
    const std::string& file_name) {
    const Eigen::VectorXd origin_vec = origin.reshaped<Eigen::ColMajor>();
    const Eigen::VectorXd derivative1_vec =
        first_derivative_matrix * origin_vec;
    const Eigen::VectorXd derivative2_vec =
        second_derivative_matrix * derivative1_vec;

    const Eigen::MatrixXd derivative1_x =
        derivative1_vec.head(rows * (cols - 1))
            .reshaped<Eigen::ColMajor>(rows, cols - 1);
    const Eigen::MatrixXd derivative1_y =
        derivative1_vec.tail((rows - 1) * cols)
            .reshaped<Eigen::ColMajor>(rows - 1, cols);
    const Eigen::MatrixXd derivative2_xx =
        derivative2_vec.head((cols - 2) * rows)
            .reshaped<Eigen::ColMajor>(rows, cols - 2);
    const Eigen::MatrixXd derivative2_yy =
        derivative2_vec.segment((cols - 2) * rows, cols * (rows - 2))
            .reshaped<Eigen::ColMajor>(rows - 2, cols);
    const Eigen::MatrixXd derivative2_xy =
        derivative2_vec.tail((cols - 1) * (rows - 1))
            .reshaped<Eigen::ColMajor>(rows - 1, cols - 1);

    plotly_plotter::figure figure;

    figure.layout().grid().rows(2);
    figure.layout().grid().columns(3);
    figure.layout().grid().pattern("coupled");

    constexpr double annotation_x = 0.5;
    constexpr double annotation_y = 1.0;
    constexpr double annotation_y_shift = 30.0;

    auto heatmap = figure.add_heatmap();
    heatmap.z(origin);
    heatmap.xaxis("x");
    heatmap.yaxis("y");
    heatmap.color_axis("coloraxis");
    auto annotation = figure.layout().add_annotation();
    annotation.x_ref("x domain");
    annotation.y_ref("y domain");
    annotation.x(annotation_x);
    annotation.y(annotation_y);
    annotation.y_shift(annotation_y_shift);
    annotation.show_arrow(false);
    annotation.align("center");
    annotation.text("Original Image");

    heatmap = figure.add_heatmap();
    heatmap.z(derivative1_x);
    heatmap.xaxis("x2");
    heatmap.yaxis("y");
    heatmap.color_axis("coloraxis");
    annotation = figure.layout().add_annotation();
    annotation.x_ref("x2 domain");
    annotation.y_ref("y domain");
    annotation.x(annotation_x);
    annotation.y(annotation_y);
    annotation.y_shift(annotation_y_shift);
    annotation.show_arrow(false);
    annotation.align("center");
    annotation.text("1st Derivative (x-direction)");

    heatmap = figure.add_heatmap();
    heatmap.z(derivative1_y);
    heatmap.xaxis("x3");
    heatmap.yaxis("y");
    heatmap.color_axis("coloraxis");
    annotation = figure.layout().add_annotation();
    annotation.x_ref("x3 domain");
    annotation.y_ref("y domain");
    annotation.x(annotation_x);
    annotation.y(annotation_y);
    annotation.y_shift(annotation_y_shift);
    annotation.show_arrow(false);
    annotation.align("center");
    annotation.text("1st Derivative (y-direction)");

    heatmap = figure.add_heatmap();
    heatmap.z(derivative2_xx);
    heatmap.xaxis("x");
    heatmap.yaxis("y2");
    heatmap.color_axis("coloraxis");
    annotation = figure.layout().add_annotation();
    annotation.x_ref("x domain");
    annotation.y_ref("y2 domain");
    annotation.x(annotation_x);
    annotation.y(annotation_y);
    annotation.y_shift(annotation_y_shift);
    annotation.show_arrow(false);
    annotation.align("center");
    annotation.text("2nd Derivative (xx-direction)");

    heatmap = figure.add_heatmap();
    heatmap.z(derivative2_yy);
    heatmap.xaxis("x2");
    heatmap.yaxis("y2");
    heatmap.color_axis("coloraxis");
    annotation = figure.layout().add_annotation();
    annotation.x_ref("x2 domain");
    annotation.y_ref("y2 domain");
    annotation.x(annotation_x);
    annotation.y(annotation_y);
    annotation.y_shift(annotation_y_shift);
    annotation.show_arrow(false);
    annotation.align("center");
    annotation.text("2nd Derivative (yy-direction)");

    heatmap = figure.add_heatmap();
    heatmap.z(derivative2_xy);
    heatmap.xaxis("x3");
    heatmap.yaxis("y2");
    heatmap.color_axis("coloraxis");
    annotation = figure.layout().add_annotation();
    annotation.x_ref("x3 domain");
    annotation.y_ref("y2 domain");
    annotation.x(annotation_x);
    annotation.y(annotation_y);
    annotation.y_shift(annotation_y_shift);
    annotation.show_arrow(false);
    annotation.align("center");
    annotation.text("2nd Derivative (xy-direction)");

    figure.layout().xaxis(2).matches("x");
    figure.layout().yaxis().scale_anchor("x");
    figure.layout().yaxis(2).matches("y");

    const double max_abs_value = std::max(
        {origin.cwiseAbs().maxCoeff(), derivative1_x.cwiseAbs().maxCoeff(),
            derivative1_y.cwiseAbs().maxCoeff(),
            derivative2_xx.cwiseAbs().maxCoeff(),
            derivative2_yy.cwiseAbs().maxCoeff(),
            derivative2_xy.cwiseAbs().maxCoeff()});

    figure.layout().color_axis().color_scale(
        plotly_plotter::color_scales::blue_to_red());
    figure.layout().color_axis().c_min(-max_abs_value);
    figure.layout().color_axis().c_max(max_abs_value);

    plotly_plotter::write_html(file_name, figure);
}

auto main() -> int {
    const auto first_derivative_matrix =
        num_prob_collect::regularization::sparse_diff_matrix_2d<
            Eigen::SparseMatrix<double>>(cols, rows);
    const auto second_derivative_matrix =
        num_prob_collect::regularization::tgv2_second_derivative_matrix_2d<
            Eigen::SparseMatrix<double>>(cols, rows);

    Eigen::MatrixXd origin;
    num_prob_collect::regularization::
        generate_sparse_sample_image_with_one_constant_circle(
            origin, rows, cols);
    visualize_derivatives(origin, first_derivative_matrix,
        second_derivative_matrix,
        "./calculate_derivatives_of_2d_images_one_constant_circle.html");

    num_prob_collect::regularization::
        generate_sparse_sample_image_with_two_constant_circles(
            origin, rows, cols);
    visualize_derivatives(origin, first_derivative_matrix,
        second_derivative_matrix,
        "./calculate_derivatives_of_2d_images_two_constant_circles.html");

    num_prob_collect::regularization::
        generate_sparse_sample_image_with_one_quadratic_circle(
            origin, rows, cols);
    visualize_derivatives(origin, first_derivative_matrix,
        second_derivative_matrix,
        "./calculate_derivatives_of_2d_images_one_quadratic_circle.html");

    num_prob_collect::regularization::
        generate_sparse_sample_image_with_one_smooth_circle(origin, rows, cols);
    visualize_derivatives(origin, first_derivative_matrix,
        second_derivative_matrix,
        "./calculate_derivatives_of_2d_images_one_smooth_circle.html");

    return 0;
}
