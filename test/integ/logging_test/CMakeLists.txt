add_executable(num_collect_test_integ_logging_write_log_writer writer.cpp)
target_link_libraries(num_collect_test_integ_logging_write_log_writer
                      PRIVATE num_collect bfg::lyra)
target_add_ausan(num_collect_test_integ_logging_write_log_writer)

add_test(
    NAME num_collect_test_integ_logging
    COMMAND
        ${PIPENV_EXECUTABLE} run pytest ${CMAKE_CURRENT_SOURCE_DIR}
        "--build=${CMAKE_BINARY_DIR}"
        "--junit-xml=${NUM_COLLECT_JUNIT_DIR}/num_collect_test_integ_logging.xml"
    WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})
