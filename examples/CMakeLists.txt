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

add_subdirectory(util)
add_subdirectory(logging)
add_subdirectory(linear)
add_subdirectory(auto_diff)
add_subdirectory(opt)
add_subdirectory(regularization)
add_subdirectory(rbf)
add_subdirectory(roots)
add_subdirectory(ode)
add_subdirectory(pde)
