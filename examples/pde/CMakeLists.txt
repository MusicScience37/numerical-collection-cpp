add_example(example_pde_fd_rodasp_wave1d fd_rodasp_wave1d.cpp)
target_link_libraries(example_pde_fd_rodasp_wave1d
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

add_example(example_pde_rbf_fd_wave_equation_1d_string
            rbf_fd_wave_equation_1d_string.cpp)
target_link_libraries(example_pde_rbf_fd_wave_equation_1d_string
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

add_example(example_pde_rbf_fd_advection_equation_1d
            rbf_fd_advection_equation_1d.cpp)
target_link_libraries(example_pde_rbf_fd_advection_equation_1d
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

add_example(example_pde_rbf_fd_advection_diffusion_equation_1d
            rbf_fd_advection_diffusion_equation_1d.cpp)
target_link_libraries(example_pde_rbf_fd_advection_diffusion_equation_1d
                      PRIVATE cpp_plotly_plotter::plotly_plotter)

if(VTK_FOUND)
    add_example(example_pde_rbf_fd_diffusion_equation_2d_dirichlet
                rbf_fd_diffusion_equation_2d_dirichlet.cpp)
    target_link_libraries(example_pde_rbf_fd_diffusion_equation_2d_dirichlet
                          PRIVATE ${VTK_LIBRARIES})
    vtk_module_autoinit(
        TARGETS example_pde_rbf_fd_diffusion_equation_2d_dirichlet MODULES
        ${VTK_LIBRARIES})

    add_example(example_pde_rbf_fd_diffusion_equation_2d_neumann
                rbf_fd_diffusion_equation_2d_neumann.cpp)
    target_link_libraries(example_pde_rbf_fd_diffusion_equation_2d_neumann
                          PRIVATE ${VTK_LIBRARIES})
    vtk_module_autoinit(
        TARGETS example_pde_rbf_fd_diffusion_equation_2d_neumann MODULES
        ${VTK_LIBRARIES})

    add_example(example_pde_rbf_fd_advection_diffusion_equation_2d
                rbf_fd_advection_diffusion_equation_2d.cpp)
    target_link_libraries(example_pde_rbf_fd_advection_diffusion_equation_2d
                          PRIVATE ${VTK_LIBRARIES})
    vtk_module_autoinit(
        TARGETS example_pde_rbf_fd_advection_diffusion_equation_2d MODULES
        ${VTK_LIBRARIES})
endif()
