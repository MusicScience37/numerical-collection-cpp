#[[
MIT License

Copyright (c) 2020 Kenta Kabashima

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
]]
string(TOUPPER ${PROJECT_NAME} UPPER_PROJECT_NAME)
option(${UPPER_PROJECT_NAME}_ENABLE_AUSAN
       "enable address sanitizer and undefined behaivior sanitizer" OFF)

# Function target_add_ausan
#
# Add configuration of address sanitizer and undefined behaivior sanitizer.
#
function(target_add_ausan _TARGET)
    if(${UPPER_PROJECT_NAME}_ENABLE_AUSAN)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            target_compile_options(${_TARGET}
                                   PUBLIC -fsanitize=address,undefined)
            target_link_options(${_TARGET} PUBLIC -fsanitize=address,undefined)
        endif()
    endif()
endfunction()
