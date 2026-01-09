option(NUM_COLLECT_TEST_EXPERIMENTS
       "test execution of experiments in Numerical Collection C++" OFF)
function(add_experiment TARGET)
    add_executable(${TARGET} ${ARGN})
    target_link_libraries(${TARGET} PRIVATE num_collect num_prob_collect
                                            example_common)
    if(NUM_COLLECT_TEST_EXPERIMENTS)
        add_test(
            NAME ${TARGET}
            COMMAND $<TARGET_FILE:${TARGET}>
            WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})
    endif()
endfunction()

add_subdirectory(linear)
add_subdirectory(rbf)
add_subdirectory(regularization)
add_subdirectory(pde)
