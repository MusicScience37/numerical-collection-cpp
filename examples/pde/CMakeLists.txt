add_example(example_pde_fd_rodasp_wave1d fd_rodasp_wave1d.cpp)
target_link_libraries(example_pde_fd_rodasp_wave1d
                      PRIVATE cpp_plotly_plotter::plotly_plotter)
