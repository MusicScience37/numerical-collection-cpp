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
add_subdirectory(logging)
add_subdirectory(auto_diff)
add_subdirectory(opt)
add_subdirectory(regularization)
add_subdirectory(interp/kernel)
add_subdirectory(roots)
add_subdirectory(ode)
add_subdirectory(pde)
