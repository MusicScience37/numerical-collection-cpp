add_executable(
    num_collect_test_units_opt
    # 1-dim deterministic methods
    golden_section_search_test.cpp
    # descent methods
    backtracking_line_searcher_test.cpp
    steepest_descent_test.cpp
    # etc
    downhill_simplex_test.cpp)
target_add_catch2(num_collect_test_units_opt)
