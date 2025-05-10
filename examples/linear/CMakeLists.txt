add_example(num_collect_ex_laplacian_2d_grid laplacian_2d_grid.cpp)
target_link_libraries(num_collect_ex_laplacian_2d_grid
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

add_example(num_collect_ex_sparse_ordering_methods ordering_methods.cpp)
target_link_libraries(num_collect_ex_sparse_ordering_methods
                      PRIVATE pngpp::pngpp)
