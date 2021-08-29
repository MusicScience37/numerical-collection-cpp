option(NUM_COLLECT_TEST_EXAMPLES
       "test execution of examples in numerical-collection-cpp" OFF)
function(add_example TARGET)
    add_executable(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PRIVATE num_collect num_prob_collect)
    if(NUM_COLLECT_TEST_EXAMPLES)
        add_test(
            NAME ${TARGET}
            COMMAND $<TARGET_FILE:${TARGET}>
            WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})
    endif()
endfunction()

find_program(
    PYTHON_EXECUTABLE
    NAMES python3 python
    PATHS ${PIPENV_VENV_DIR}/bin ${PIPENV_VENV_DIR}/Scripts
    NO_DEFAULT_PATH)
if(PYTHON_EXECUTABLE)
    message(STATUS "Found Python: ${PYTHON_EXECUTABLE}")
    find_package(pybind11 REQUIRED)
else()
    message(STATUS "Python not found for Python interface")
endif()

function(add_example_using_pybind11 TARGET)
    if(PYTHON_EXECUTABLE)
        add_executable(${TARGET} ${ARGN})
        target_link_libraries(${TARGET} PRIVATE num_collect num_prob_collect
                                                pybind11::embed)
        if(NUM_COLLECT_TEST_EXAMPLES)
            add_test(
                NAME ${TARGET}
                COMMAND ${PIPENV_EXECUTABLE} run $<TARGET_FILE:${TARGET}>
                WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})
        endif()
    endif()
endfunction()

add_subdirectory(util)
add_subdirectory(opt)
add_subdirectory(roots)
add_subdirectory(ode)
add_subdirectory(auto_diff)
add_subdirectory(regularization)
