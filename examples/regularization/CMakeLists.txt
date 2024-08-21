add_example_using_pybind11(num_collect_ex_reg_blur_sine_tikhonov
                           blur_sine_tikhonov.cpp)
add_example(num_collect_ex_sparse_image_regularization
            sparse_image_regularization.cpp)
target_link_libraries(num_collect_ex_sparse_image_regularization
                      PRIVATE pngpp::pngpp)
add_example_using_pybind11(num_collect_ex_reg_plot_gcv_with_fista
                           plot_gcv_with_fista.cpp)
add_example_using_pybind11(num_collect_ex_reg_plot_l_curve_with_fista
                           plot_l_curve_with_fista.cpp)
