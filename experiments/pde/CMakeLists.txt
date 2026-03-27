add_experiment(experiment_pde_rbf_fd_weights_2d rbf_fd_weights_2d.cpp)
if(VTK_FOUND)
    add_experiment(experiment_pde_rbf_fd_laplacian_matrix_2d
                   rbf_fd_laplacian_matrix_2d.cpp)
    target_link_libraries(experiment_pde_rbf_fd_laplacian_matrix_2d
                          PRIVATE ${VTK_LIBRARIES})
    vtk_module_autoinit(TARGETS experiment_pde_rbf_fd_laplacian_matrix_2d
                        MODULES ${VTK_LIBRARIES})

    add_experiment(experiment_pde_rbf_fd_poisson_equation_2d
                   rbf_fd_poisson_equation_2d.cpp)
    target_link_libraries(
        experiment_pde_rbf_fd_poisson_equation_2d
        PRIVATE ${VTK_LIBRARIES} cpp_plotly_plotter::plotly_plotter)
    vtk_module_autoinit(TARGETS experiment_pde_rbf_fd_poisson_equation_2d
                        MODULES ${VTK_LIBRARIES})
endif()
