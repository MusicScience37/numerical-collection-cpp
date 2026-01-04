add_experiment(experiment_linear_laplacian_2d_grid laplacian_2d_grid.cpp)
target_link_libraries(experiment_linear_laplacian_2d_grid
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

add_experiment(experiment_linear_sparse_ordering_methods ordering_methods.cpp)
