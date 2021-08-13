include(${CMAKE_CURRENT_SOURCE_DIR}/source_list.cmake)
add_executable(num_collect_test_units ${SOURCE_FILES})
target_add_catch2(num_collect_test_units)

add_executable(num_collect_test_units_unity EXCLUDE_FROM_ALL unity_source.cpp)
target_link_libraries(
    num_collect_test_units_unity PRIVATE Catch2::Catch2WithMain num_collect
                                         num_prob_collect)
target_include_directories(num_collect_test_units_unity
                           PRIVATE ${NUM_COLLECT_TEST_INCLUDE_DIR})
target_check_clang_tidy(num_collect_test_units_unity)
