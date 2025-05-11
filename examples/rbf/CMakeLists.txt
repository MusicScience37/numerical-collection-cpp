add_example(num_collect_ex_rbf_generate_2d_halton_nodes
            generate_2d_halton_nodes.cpp)
target_link_libraries(num_collect_ex_rbf_generate_2d_halton_nodes
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

add_example_using_pybind11(num_collect_ex_rbf_visualize_rbf_in_1d
                           visualize_rbf_in_1d.cpp)

add_example(num_collect_ex_rbf_interpolate_2d interpolate_2d.cpp)
target_link_libraries(num_collect_ex_rbf_interpolate_2d
                      PRIVATE cpp_plotly_plotter::plotly_plotter)
