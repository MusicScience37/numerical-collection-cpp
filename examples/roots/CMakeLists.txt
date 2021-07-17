add_executable(num_collect_ex_newton_raphson newton_raphson.cpp)
target_link_libraries(num_collect_ex_newton_raphson PRIVATE num_collect
                                                            num_prob_collect)
