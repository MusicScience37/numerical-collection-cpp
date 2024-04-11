/*
 * Copyright 2024 MusicScience37 (Kenta Kabashima)
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
 * \brief Implementation of gzip_msgpack_output_stream class.
 */
#include "gzip_msgpack_output_stream.h"

#include <stdexcept>
#include <string>

#include <fmt/core.h>
#include <msgpack_light/output_stream.h>
#include <zlib.h>

gzip_msgpack_output_stream::gzip_msgpack_output_stream(
    const std::string& file_path)
    : file_(gzopen(file_path.c_str(), "wb9")) {
    if (file_ == nullptr) {
        throw std::runtime_error(fmt::format("Failed to open {}.", file_path));
    }
}

gzip_msgpack_output_stream::~gzip_msgpack_output_stream() {
    (void)gzclose(file_);
}

void gzip_msgpack_output_stream::write(
    const unsigned char* data, std::size_t size) {
    const auto size_uint = static_cast<unsigned int>(size);
    const auto size_int = static_cast<int>(size);
    if (gzwrite(file_, data, size_uint) != size_int) {
        throw std::runtime_error("Failed to write data.");
    }
}
