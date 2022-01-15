option(NUM_COLLECT_TEST_BENCHMARKS "execute benchmarks in tests" OFF)

set(NUM_COLLECT_BENCH_DIR
    "${CMAKE_BINARY_DIR}/bench"
    CACHE PATH "directory to which benchmark results are outputed" FORCE)
file(MAKE_DIRECTORY ${NUM_COLLECT_BENCH_DIR})

set(NUM_COLLECT_BENCH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

function(target_add_to_benchmark TARGET)
    target_link_libraries(${TARGET} PRIVATE cpp_stat_bench::cpp_stat_bench
                                            num_collect num_prob_collect)
    target_include_directories(${TARGET}
                               PRIVATE ${NUM_COLLECT_BENCH_INCLUDE_DIR})

    if(NUM_COLLECT_TEST_BENCHMARKS)
        add_test(
            NAME ${TARGET}
            COMMAND ${TARGET} --plot "${TARGET}/" --json "${TARGET}/result.json"
            WORKING_DIRECTORY ${NUM_COLLECT_BENCH_DIR})
    endif()
endfunction()

add_subdirectory(util)
add_subdirectory(logging)
add_subdirectory(auto_diff)
add_subdirectory(opt)
add_subdirectory(multi_double)
add_subdirectory(integration)
add_subdirectory(ode)
add_subdirectory(regularization)
