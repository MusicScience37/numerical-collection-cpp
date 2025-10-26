add_executable(test_integ_multi_double_calculate_oct calculate_oct.cpp)
target_link_libraries(test_integ_multi_double_calculate_oct PRIVATE num_collect)
target_add_ausan(test_integ_multi_double_calculate_oct)

add_test(
    NAME test_integ_oct_math
    COMMAND
        ${POETRY_EXECUTABLE} run python
        ${CMAKE_CURRENT_SOURCE_DIR}/test_oct_math.py
        "--build_dir=${CMAKE_BINARY_DIR}"
    WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})
