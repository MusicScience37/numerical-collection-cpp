option(NUM_COLLECT_TEST_BENCHMARKS "execute benchmarks in tests" OFF)

set(NUM_COLLECT_BENCH_DIR
    "${CMAKE_BINARY_DIR}/bench"
    CACHE PATH "directory to which benchmark results are outputed" FORCE)
file(MAKE_DIRECTORY ${NUM_COLLECT_BENCH_DIR})

set(NUM_COLLECT_BENCH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

function(target_add_to_benchmark TARGET)
    target_link_libraries(${TARGET} PRIVATE celero::celero num_collect
                                            num_prob_collect)
    target_include_directories(${TARGET}
                               PRIVATE ${NUM_COLLECT_BENCH_INCLUDE_DIR})
    target_check_clang_tidy(${TARGET})

    if(NUM_COLLECT_TEST_BENCHMARKS)
        add_test(
            NAME ${TARGET}
            COMMAND ${TARGET} -t ${TARGET}.csv
            WORKING_DIRECTORY ${NUM_COLLECT_BENCH_DIR})
    endif()
endfunction()

add_subdirectory(opt)
add_subdirectory(multi_double)
add_subdirectory(integration)
add_subdirectory(ode)
