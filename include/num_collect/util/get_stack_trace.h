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
 * \brief Definition of get_stack_trace class.
 */
#pragma once

#include <sstream>
#include <string>

#if !__has_include(<backward.hpp>)
#error "This header requires backward-cpp library"
#endif

#include <backward.hpp>

namespace num_collect {

/*!
 * \brief Get the stack trace.
 *
 * \return Stack trace.
 */
[[nodiscard]] inline auto get_stack_trace() -> std::string {
    std::ostringstream stream;
    backward::StackTrace st;
    st.load_here();
    backward::Printer p;
    p.object = false;
    p.color_mode = backward::ColorMode::never;
    p.address = true;
    p.print(st, stream);
    return stream.str();
}

}  // namespace num_collect
