option(NUM_COLLECT_WRITE_JUNIT
       "write JUnit XML files in tests of ${FULL_PROJECT_NAME}" OFF)
set(NUM_COLLECT_JUNIT_DIR
    "${CMAKE_BINARY_DIR}/junit"
    CACHE PATH "directory to which JUnit XML files are written" FORCE)
if(NUM_COLLECT_WRITE_JUNIT)
    file(MAKE_DIRECTORY ${NUM_COLLECT_JUNIT_DIR})
endif()

set(NUM_COLLECT_TEMP_TEST_DIR
    "${CMAKE_BINARY_DIR}/temp_test"
    CACHE PATH "directory used in test" FORCE)
file(MAKE_DIRECTORY ${NUM_COLLECT_TEMP_TEST_DIR})

add_subdirectory(utils)

macro(target_add_catch2 TARGET)
    target_link_libraries(
        ${TARGET}
        PRIVATE Catch2::Catch2WithMain trompeloeil::trompeloeil
                ApprovalTests::ApprovalTests num_collect num_prob_collect
                num_collect_test_utils)

    if(NUM_COLLECT_WRITE_JUNIT)
        set(CATCH2_REPORT_OPTIONS -r junit -o
                                  ${NUM_COLLECT_JUNIT_DIR}/${TARGET}.xml)
    else()
        set(CATCH2_REPORT_OPTIONS -r console)
    endif()

    add_test(
        NAME ${TARGET}
        COMMAND ${TARGET} ${CATCH2_REPORT_OPTIONS}
        WORKING_DIRECTORY ${NUM_COLLECT_TEMP_TEST_DIR})
endmacro()

option(NUM_COLLECT_ENABLE_UNIT_TESTS
       "enable unit tests of ${FULL_PROJECT_NAME}" ON)
if(NUM_COLLECT_ENABLE_UNIT_TESTS)
    add_subdirectory(units)
endif()

option(NUM_COLLECT_ENABLE_INTEG_TESTS
       "enable integration tests of ${FULL_PROJECT_NAME}" ON)
if(NUM_COLLECT_ENABLE_INTEG_TESTS)
    add_subdirectory(integ)
endif()

option(NUM_COLLECT_ENABLE_BENCH "enable benchmarks of ${FULL_PROJECT_NAME}" OFF)
if(NUM_COLLECT_ENABLE_BENCH)
    add_subdirectory(bench)
endif()
