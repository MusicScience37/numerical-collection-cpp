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
 * \brief Definition of para_view_time_steps_data_file_writer class.
 */
#pragma once

#include <iostream>
#include <string>
#include <string_view>

#include "num_collect/util/file_wrapper.h"

/*!
 * \brief Class to write ParaView data files for time steps.
 */
class para_view_time_steps_data_file_writer {
public:
    /*!
     * \brief Constructor.
     *
     * \param[in] file_path Path to the ParaView data file.
     */
    explicit para_view_time_steps_data_file_writer(const std::string& file_path)
        : file_(file_path, "w") {
        file_.write(R"(<?xml version="1.0"?>
<VTKFile type="Collection" version="0.1" byte_order="LittleEndian">
  <Collection>
)");
    }

    para_view_time_steps_data_file_writer(
        const para_view_time_steps_data_file_writer&) = delete;
    para_view_time_steps_data_file_writer(
        para_view_time_steps_data_file_writer&&) = delete;
    auto operator=(const para_view_time_steps_data_file_writer&)
        -> para_view_time_steps_data_file_writer& = delete;
    auto operator=(para_view_time_steps_data_file_writer&&)
        -> para_view_time_steps_data_file_writer& = delete;

    /*!
     * \brief Destructor.
     */
    ~para_view_time_steps_data_file_writer() {
        try {
            file_.write(R"(  </Collection>
</VTKFile>)");
        } catch (const std::exception& e) {
            std::cerr
                << "Failed to write the footer of the ParaView data file: "
                << e.what() << "\n";
        }
    }

    /*!
     * \brief Add a time step.
     *
     * \param[in] time Time of the time step.
     * \param[in] file_name Name of the file for the time step.
     */
    void add_time_step(double time, std::string_view file_name) {
        file_.write_with_format(R"(    <DataSet timestep="{:.2e}" file="{}"/>
)",
            time, file_name);
    }

private:
    //! File.
    num_collect::util::file_wrapper file_;
};
