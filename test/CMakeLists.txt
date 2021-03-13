option(NUM_COLLECT_WRITE_JUNIT
       "write JUnit XML files in tests of numeric-collection-cpp" OFF)
if(NUM_COLLECT_WRITE_JUNIT)
    set(NUM_COLLECT_JUNIT_DIR
        "${CMAKE_BINARY_DIR}/junit"
        CACHE PATH "directory to which JUnit XML files are outputed" FORCE)
    file(MAKE_DIRECTORY ${NUM_COLLECT_JUNIT_DIR})
endif()

set(NUM_COLLECT_TEMP_TEST_DIR
    "${CMAKE_BINARY_DIR}/temp_test"
    CACHE PATH "directory used in test" FORCE)
file(MAKE_DIRECTORY ${NUM_COLLECT_TEMP_TEST_DIR})

macro(target_add_catch2 TARGET)
    target_link_libraries(${TARGET} PRIVATE Catch2::Catch2WithMain num_collect)
    target_check_clang_tidy(${TARGET})

    if(NUM_COLLECT_WRITE_JUNIT)
        set(CATCH2_REPORT_OPTIONS -r junit -o
                                  ${NUM_COLLECT_JUNIT_DIR}/${TARGET}.xml)
    else()
        set(CATCH2_REPORT_OPTIONS -r compact)
    endif()

    add_test(
        NAME ${TARGET}
        COMMAND ${TARGET} ${CATCH2_REPORT_OPTIONS}
        WORKING_DIRECTORY ${NUM_COLLECT_TEMP_TEST_DIR})
endmacro()

add_subdirectory(units)
