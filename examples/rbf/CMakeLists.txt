add_example(example_rbf_generate_2d_halton_nodes generate_2d_halton_nodes.cpp)
target_link_libraries(example_rbf_generate_2d_halton_nodes
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

add_example(example_rbf_interpolate_2d interpolate_2d.cpp)
target_link_libraries(example_rbf_interpolate_2d
                      PRIVATE cpp_plotly_plotter::plotly_plotter)
