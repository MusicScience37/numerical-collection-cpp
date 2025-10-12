add_executable(test_integ_multi_double_calculate_quad calculate_quad.cpp)
target_link_libraries(test_integ_multi_double_calculate_quad
                      PRIVATE num_collect)
target_add_ausan(test_integ_multi_double_calculate_quad)

add_test(
    NAME test_integ_quad_math
    COMMAND
        ${POETRY_EXECUTABLE} run python
        ${CMAKE_CURRENT_SOURCE_DIR}/test_quad_math.py
        "--build_dir=${CMAKE_BINARY_DIR}"
    WORKING_DIRECTORY ${NUM_COLLECT_SOURCE_DIR})
