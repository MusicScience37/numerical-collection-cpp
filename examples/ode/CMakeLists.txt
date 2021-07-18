add_executable(num_collect_ex_rk4_solver rk4_solver.cpp)
target_link_libraries(num_collect_ex_rk4_solver PRIVATE num_collect
                                                        num_prob_collect)

add_executable(num_collect_ex_rkf45_solver rkf45_solver.cpp)
target_link_libraries(num_collect_ex_rkf45_solver PRIVATE num_collect
                                                          num_prob_collect)
