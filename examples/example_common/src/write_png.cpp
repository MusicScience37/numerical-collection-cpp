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
 * \brief Implementation of write_png function.
 */
#include "write_png.h"

#include <algorithm>
#include <cmath>
#include <csetjmp>
#include <cstdio>
#include <memory>
#include <vector>

#include <png.h>

#include "num_collect/base/exception.h"
#include "num_collect/logging/logger.h"
#include "num_collect/logging/logging_macros.h"

namespace {

void handle_libpng_warning(png_structp /* png_ptr */, png_const_charp message) {
    if (message != nullptr) {
        num_collect::logging::logger logger;
        NUM_COLLECT_LOG_WARNING(logger, "Warning from libpng: {}", message);
    }
}

struct png_writer_guard {
    png_structp png_ptr{nullptr};
    png_infop info_ptr{nullptr};

    png_writer_guard() = default;

    ~png_writer_guard() {
        if (png_ptr != nullptr) {
            png_destroy_write_struct(
                &png_ptr, info_ptr != nullptr ? &info_ptr : nullptr);
        }
    }

    png_writer_guard(const png_writer_guard&) = delete;
    png_writer_guard(png_writer_guard&&) = delete;
    auto operator=(const png_writer_guard&) -> png_writer_guard& = delete;
    auto operator=(png_writer_guard&&) -> png_writer_guard& = delete;
};

}  // namespace

void write_png(const Eigen::MatrixXd& matrix, const std::string& file_path,
    const double max_value, const double min_value) {
    if (matrix.rows() <= 0 || matrix.cols() <= 0) {
        NUM_COLLECT_LOG_AND_THROW(num_collect::invalid_argument,
            "Matrix dimensions must be positive.");
    }
    if (max_value <= min_value) {
        NUM_COLLECT_LOG_AND_THROW(num_collect::invalid_argument,
            "max_value must be larger than min_value.");
    }

    const auto width = static_cast<png_uint_32>(matrix.cols());
    const auto height = static_cast<png_uint_32>(matrix.rows());
    if (static_cast<Eigen::Index>(width) != matrix.cols() ||
        static_cast<Eigen::Index>(height) != matrix.rows()) {
        NUM_COLLECT_LOG_AND_THROW(num_collect::out_of_range,
            "Matrix dimensions are too large to write PNG.");
    }

    std::FILE* const raw_file = std::fopen(file_path.c_str(), "wb");
    if (raw_file == nullptr) {
        NUM_COLLECT_LOG_AND_THROW(num_collect::file_error,
            "Failed to open file '{}' to write PNG.", file_path);
    }
    const std::unique_ptr<std::FILE, decltype(&std::fclose)> file(
        raw_file, &std::fclose);

    png_writer_guard writer{};
    writer.png_ptr = png_create_write_struct(
        PNG_LIBPNG_VER_STRING, nullptr, nullptr, &handle_libpng_warning);
    if (writer.png_ptr == nullptr) {
        NUM_COLLECT_LOG_AND_THROW(num_collect::num_collect_exception,
            "Failed to create libpng write struct.");
    }
    writer.info_ptr = png_create_info_struct(writer.png_ptr);
    if (writer.info_ptr == nullptr) {
        NUM_COLLECT_LOG_AND_THROW(num_collect::num_collect_exception,
            "Failed to create libpng info struct.");
    }

    png_set_error_fn(writer.png_ptr, nullptr, nullptr, &handle_libpng_warning);
    // NOLINTNEXTLINE(cert-err52-cpp): libpng uses setjmp/longjmp for error handling.
    if (setjmp(png_jmpbuf(writer.png_ptr))) {
        NUM_COLLECT_LOG_AND_THROW(num_collect::num_collect_exception,
            "An error occurred in libpng while writing PNG.");
    }

    png_init_io(writer.png_ptr, file.get());
    png_set_IHDR(writer.png_ptr, writer.info_ptr, width, height, 8,
        PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);
    png_write_info(writer.png_ptr, writer.info_ptr);

    constexpr double max_pixel_value = 255.0;
    const double scale = max_pixel_value / (max_value - min_value);
    std::vector<png_byte> row_buffer(static_cast<std::size_t>(width));
    for (png_uint_32 row_index = 0; row_index < height; ++row_index) {
        for (png_uint_32 column_index = 0; column_index < width;
            ++column_index) {
            const double value = (matrix(static_cast<Eigen::Index>(row_index),
                                      static_cast<Eigen::Index>(column_index)) -
                                     min_value) *
                scale;
            const auto clipped_value = static_cast<unsigned int>(
                std::lround(std::clamp(value, 0.0, max_pixel_value)));
            row_buffer[column_index] = static_cast<png_byte>(clipped_value);
        }
        png_write_row(writer.png_ptr, row_buffer.data());
    }
    png_write_end(writer.png_ptr, writer.info_ptr);

    num_collect::logging::logger logger;
    NUM_COLLECT_LOG_INFO(logger, "Wrote {}.", file_path);
}
