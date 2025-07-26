add_executable(test_integ_assert_integer_failure test_integer_failure.cpp)
target_link_libraries(test_integ_assert_integer_failure PRIVATE num_collect)
target_add_ausan(test_integ_assert_integer_failure)

add_executable(test_integ_assert_integer_success test_integer_success.cpp)
target_link_libraries(test_integ_assert_integer_success PRIVATE num_collect)
target_add_ausan(test_integ_assert_integer_success)

add_executable(test_integ_assert_constexpr_success test_constexpr_success.cpp)
target_link_libraries(test_integ_assert_constexpr_success PRIVATE num_collect)
target_add_ausan(test_integ_assert_constexpr_success)

add_test(
    NAME test_integ_assertions
    COMMAND
        ${POETRY_EXECUTABLE} run pytest ${CMAKE_CURRENT_SOURCE_DIR}
        "--build=${CMAKE_BINARY_DIR}"
        "--junit-xml=${NUM_COLLECT_JUNIT_DIR}/test_integ_assertions.xml"
    WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})
