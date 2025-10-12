add_executable(test_integ_multi_double_calculate_quad calculate_quad.cpp)
target_link_libraries(test_integ_multi_double_calculate_quad
                      PRIVATE num_collect)
target_add_ausan(test_integ_multi_double_calculate_quad)
