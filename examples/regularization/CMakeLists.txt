add_executable(num_collect_ex_reg_blur_sine_tikhonov blur_sine_tikhonov.cpp)
target_link_libraries(num_collect_ex_reg_blur_sine_tikhonov
                      PRIVATE num_collect num_prob_collect pybind11::embed)
