add_executable(
    num_collect_test_units_auto_diff
    forward/variable_test.cpp backward/graph/node_test.cpp
    backward/graph/node_differentiator_test.cpp backward/variable_test.cpp)
target_add_catch2(num_collect_test_units_auto_diff)
