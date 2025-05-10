add_example(num_collect_ex_fd_rodasp_wave1d fd_rodasp_wave1d.cpp)
target_link_libraries(num_collect_ex_fd_rodasp_wave1d
                      PRIVATE cpp_plotly_plotter::plotly_plotter)
