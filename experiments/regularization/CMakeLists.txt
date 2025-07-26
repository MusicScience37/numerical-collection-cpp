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
