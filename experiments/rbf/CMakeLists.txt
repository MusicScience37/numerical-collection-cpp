add_experiment(experiment_rbf_visualize_rbf_in_1d visualize_rbf_in_1d.cpp)
target_link_libraries(experiment_rbf_visualize_rbf_in_1d
                      PRIVATE cpp_plotly_plotter::plotly_plotter)
