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
 * \brief Definition of file_wrapper class.
 */
#pragma once

#include <cerrno>
#include <cstdio>
#include <string>
#include <string_view>
#include <utility>

#include "num_collect/base/exception.h"
#include "num_collect/util/format_errno.h"

namespace num_collect::logging::sinks {

/*!
 * \brief Class to wrap file pointer.
 *
 * \thread_safety Not thread-safe.
 */
class file_wrapper {
public:
    /*!
     * \brief Constructor.
     */
    file_wrapper() noexcept = default;

    /*!
     * \brief Constructor.
     *
     * \param[in] file File pointer.
     * \param[in] close_on_destruction Whether to close the file when
     * destructed.
     */
    file_wrapper(std::FILE* file, bool close_on_destruction)
        : file_(file), close_on_destruction_(close_on_destruction) {}

    /*!
     * \brief Constructor to open a file.
     *
     * \param[in] filepath Filepath.
     * \param[in] mode Mode to open. (As in `std::fopen`.)
     */
    file_wrapper(const std::string& filepath, const char* mode) {
        open(filepath, mode);
    }

    file_wrapper(const file_wrapper&) = delete;
    auto operator=(const file_wrapper&) = delete;

    /*!
     * \brief Move constructor.
     *
     * \param[in] obj Object to move from.
     */
    file_wrapper(file_wrapper&& obj) noexcept
        : file_(std::exchange(obj.file_, nullptr)),
          close_on_destruction_(
              std::exchange(obj.close_on_destruction_, false)) {}

    /*!
     * \brief Move assignment operator.
     *
     * \param[in] obj Object to move from.
     * \return This.
     */
    auto operator=(file_wrapper&& obj) noexcept -> file_wrapper& {
        swap(obj);
        return *this;
    }

    /*!
     * \brief Destructor.
     */
    ~file_wrapper() noexcept { close(); }

    /*!
     * \brief Swap with another object.
     *
     * \param[in] obj Object to swap with.
     */
    void swap(file_wrapper& obj) noexcept {
        std::swap(file_, obj.file_);
        std::swap(close_on_destruction_, obj.close_on_destruction_);
    }

    /*!
     * \brief Open a file.
     *
     * \param[in] filepath Filepath.
     * \param[in] mode Mode to open. (As in `std::fopen`.)
     */
    void open(const std::string& filepath, const char* mode) {
        close();
        errno = 0;
        file_ = std::fopen(filepath.c_str(), mode);
        if (file_ == nullptr) {
            throw file_error(util::format_errno(
                "Failed to open {} with mode \"{}\"", filepath, mode));
        }
        close_on_destruction_ = true;
    }

    /*!
     * \brief Set this file to standard output.
     */
    void set_stdout() {
        close();
        file_ = stdout;
    }

    /*!
     * \brief Set this file to standard error.
     */
    void set_stderr() {
        close();
        file_ = stderr;
    }

    /*!
     * \brief Close this file.
     */
    void close() noexcept {
        if (file_ != nullptr && close_on_destruction_) {
            (void)std::fclose(file_);
        }
        file_ = nullptr;
        close_on_destruction_ = false;
    }

    /*!
     * \brief Write data.
     *
     * \param[in] data Data.
     */
    void write(std::string_view data) {
        if (file_ == nullptr) [[unlikely]] {
            throw file_error("Failed to write to file: file is not opened.");
        }

        errno = 0;
        const std::size_t written_size =
            std::fwrite(data.data(), 1, data.size(), file_);
        if (written_size != data.size()) [[unlikely]] {
            throw file_error(util::format_errno("Failed to write to file"));
        }
    }

    /*!
     * \brief Flush buffer.
     */
    void flush() {
        if (file_ == nullptr) [[unlikely]] {
            throw file_error("Failed to write to file: file is not opened.");
        }

        errno = 0;
        const int result = std::fflush(file_);
        if (result != 0) [[unlikely]] {
            throw file_error(util::format_errno("Failed to write to file"));
        }
    }

    /*!
     * \brief Get the file pointer.
     *
     * \return File pointer.
     */
    [[nodiscard]] auto file() const noexcept -> std::FILE* { return file_; }

private:
    //! File pointer.
    std::FILE* file_{nullptr};

    //! Whether to close the file when destructed.
    bool close_on_destruction_{false};
};

}  // namespace num_collect::logging::sinks
