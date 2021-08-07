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

# Function target_link_system_library
#
# link a library with its include directories included as a system include
# directory
#
# Arguments
#
# * _TARGET: target to which link a library
# * _TYPE: type of the linked library (PRIVATE, PUBLIC, INTERFACE)
# * _LIB: library to link
#
function(target_link_system_library _TARGET _TYPE _LIB)
    get_target_property(LIB_INCLUDE_DIRECTORIES ${_LIB}
                        INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${_TARGET} SYSTEM ${_TYPE}
                               ${_LIB_INCLUDE_DIRECTORIES})
    target_link_libraries(${_TARGET} ${_TYPE} ${_LIB})
endfunction()
