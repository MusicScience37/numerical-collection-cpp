add_experiment(experiment_reg_blur_sine_tikhonov blur_sine_tikhonov.cpp)
target_link_libraries(experiment_reg_blur_sine_tikhonov
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

add_experiment(experiment_reg_plot_gcv_with_fista plot_gcv_with_fista.cpp)
target_link_libraries(experiment_reg_plot_gcv_with_fista
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

add_experiment(experiment_reg_plot_l_curve_with_fista
               plot_l_curve_with_fista.cpp)
target_link_libraries(experiment_reg_plot_l_curve_with_fista
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

add_experiment(experiment_reg_calculate_derivatives_of_2d_images
               calculate_derivatives_of_2d_images.cpp)
target_link_libraries(experiment_reg_calculate_derivatives_of_2d_images
                      PRIVATE cpp_plotly_plotter::plotly_plotter bfg::lyra)

add_experiment(experiment_reg_calculate_tgv2_derivatives_of_2d_images
               calculate_tgv2_derivatives_of_2d_images.cpp)
target_link_libraries(experiment_reg_calculate_tgv2_derivatives_of_2d_images
                      PRIVATE cpp_plotly_plotter::plotly_plotter bfg::lyra)

add_experiment(experiment_reg_approx_l_curve approx_l_curve.cpp)
target_link_libraries(experiment_reg_approx_l_curve
                      PRIVATE cpp_plotly_plotter::plotly_plotter bfg::lyra)
