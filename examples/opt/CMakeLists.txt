add_executable(num_collect_ex_golden_section_search golden_section_search.cpp)
target_link_libraries(num_collect_ex_golden_section_search
                      PRIVATE num_collect num_prob_collect)

add_executable(num_collect_ex_steepest_descent steepest_descent.cpp)
target_link_libraries(num_collect_ex_steepest_descent PRIVATE num_collect
                                                              num_prob_collect)

add_executable(num_collect_ex_downhill_simplex downhill_simplex.cpp)
target_link_libraries(num_collect_ex_downhill_simplex PRIVATE num_collect
                                                              num_prob_collect)

add_executable(num_collect_ex_dividing_rectangles dividing_rectangles.cpp)
target_link_libraries(num_collect_ex_dividing_rectangles
                      PRIVATE num_collect num_prob_collect)
