option(NUM_COLLECT_TEST_BENCHMARKS "execute benchmarks in tests" OFF)

set(NUM_COLLECT_BENCH_DIR
    "${CMAKE_BINARY_DIR}/bench"
    CACHE PATH "directory to which benchmark results are written" FORCE)
file(MAKE_DIRECTORY ${NUM_COLLECT_BENCH_DIR})

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(NUM_COLLECT_ENABLE_HEAVY_BENCH_DEFAULT OFF)
else()
    set(NUM_COLLECT_ENABLE_HEAVY_BENCH_DEFAULT ON)
endif()
option(NUM_COLLECT_ENABLE_HEAVY_BENCH "enable heavy benchmarks"
       ${NUM_COLLECT_ENABLE_HEAVY_BENCH_DEFAULT})

add_library(num_collect_bench_common INTERFACE)
if(NUM_COLLECT_ENABLE_HEAVY_BENCH)
    target_compile_definitions(num_collect_bench_common
                               INTERFACE NUM_COLLECT_ENABLE_HEAVY_BENCH=1)
endif()

function(target_add_to_benchmark TARGET)
    target_link_libraries(
        ${TARGET}
        PRIVATE cpp_stat_bench::stat_bench num_collect num_prob_collect
                num_collect_test_utils num_collect_bench_common)

    if(NUM_COLLECT_TEST_BENCHMARKS)
        add_test(
            NAME ${TARGET}
            COMMAND ${TARGET} --plot "${TARGET}/" --json "${TARGET}/result.json"
            WORKING_DIRECTORY ${NUM_COLLECT_BENCH_DIR})
    endif()
endfunction()

function(add_bench_using_pybind11 TARGET)
    if(PYTHON_EXECUTABLE)
        target_link_libraries(
            ${TARGET}
            PRIVATE num_collect num_prob_collect num_collect_test_utils
                    num_collect_bench_common pybind11::embed)
        if(NUM_COLLECT_TEST_BENCHMARKS)
            add_test(
                NAME ${TARGET}
                COMMAND ${POETRY_EXECUTABLE} run $<TARGET_FILE:${TARGET}>
                        ${NUM_COLLECT_BENCH_DIR}
                WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})
        endif()
    endif()
endfunction()

add_subdirectory(util)
add_subdirectory(logging)
add_subdirectory(linear)
add_subdirectory(auto_diff)
add_subdirectory(rbf)
add_subdirectory(opt)
add_subdirectory(multi_double)
add_subdirectory(integration)
add_subdirectory(ode)
add_subdirectory(regularization)
