/*
 * Copyright 2026 MusicScience37 (Kenta Kabashima)
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
 * \brief Definition of write_vtp_file_for_comparison function.
 */
#pragma once

#include <string>

#include <Eigen/Core>
#include <vtkDelaunay2D.h>
#include <vtkDoubleArray.h>
#include <vtkNew.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataWriter.h>

#include "num_collect/util/vector_view.h"

/*!
 * \brief Write a VTP file for visualization in ParaView with true values and
 * errors for comparison.
 *
 * \param[in] file_path File path for the VTP file.
 * \param[in] nodes Nodes. Generated in generate_nodes function. First
 * num_interior_nodes nodes are in the interior, and the rest are on the
 * boundary in the counter-clockwise order.
 * \param[in] solution Solution values at the nodes.
 * \param[in] true_values True values at the nodes.
 * \param[in] errors Errors at the nodes.
 */
inline void write_vtp_file_for_comparison(const std::string& file_path,
    num_collect::util::vector_view<const Eigen::Vector2d> nodes,
    const Eigen::VectorXd& solution, const Eigen::VectorXd& true_values,
    const Eigen::VectorXd& errors) {
    vtkNew<vtkPoints> points;
    for (const auto& node : nodes) {
        points->InsertNextPoint(node.x(), node.y(), 0.0);
    }

    vtkNew<vtkPolyData> poly_data_of_points;
    poly_data_of_points->SetPoints(points);

    vtkNew<vtkDelaunay2D> delaunay;
    delaunay->SetInputData(poly_data_of_points);
    delaunay->Update();

    vtkPolyData* poly_data = delaunay->GetOutput();

    vtkNew<vtkDoubleArray> true_values_array;
    true_values_array->SetName("True Values");
    true_values_array->SetNumberOfComponents(1);
    for (double value : true_values) {
        true_values_array->InsertNextValue(value);
    }
    poly_data->GetPointData()->AddArray(true_values_array);

    vtkNew<vtkDoubleArray> solution_array;
    solution_array->SetName("Solution");
    solution_array->SetNumberOfComponents(1);
    for (double value : solution) {
        solution_array->InsertNextValue(value);
    }
    poly_data->GetPointData()->AddArray(solution_array);

    vtkNew<vtkDoubleArray> error_array;
    error_array->SetName("Error");
    error_array->SetNumberOfComponents(1);
    for (double value : errors) {
        error_array->InsertNextValue(value);
    }
    poly_data->GetPointData()->AddArray(error_array);

    vtkNew<vtkXMLPolyDataWriter> writer;
    writer->SetFileName(file_path.c_str());
    writer->SetInputData(poly_data);
    writer->SetDataModeToBinary();
    writer->SetCompressorTypeToZLib();
    writer->SetCompressionLevel(9);
    writer->Write();
}
