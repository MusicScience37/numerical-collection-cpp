add_example(num_collect_ex_reg_blur_sine_tikhonov blur_sine_tikhonov.cpp)
target_link_libraries(num_collect_ex_reg_blur_sine_tikhonov
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

add_example(num_collect_ex_sparse_image_regularization
            sparse_image_regularization.cpp)
target_link_libraries(num_collect_ex_sparse_image_regularization
                      PRIVATE pngpp::pngpp)

add_example(num_collect_ex_reg_plot_gcv_with_fista plot_gcv_with_fista.cpp)
target_link_libraries(num_collect_ex_reg_plot_gcv_with_fista
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

add_example(num_collect_ex_reg_plot_l_curve_with_fista
            plot_l_curve_with_fista.cpp)
target_link_libraries(num_collect_ex_reg_plot_l_curve_with_fista
                      PRIVATE cpp_plotly_plotter::plotly_plotter)
