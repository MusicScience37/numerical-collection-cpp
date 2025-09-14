add_example(example_reg_image_denoising_fista image_denoising_fista.cpp)
target_link_libraries(example_reg_image_denoising_fista
                      PRIVATE bfg::lyra cpp_plotly_plotter::plotly_plotter)

add_example(example_reg_image_denoising_tv_admm image_denoising_tv_admm.cpp)
target_link_libraries(example_reg_image_denoising_tv_admm
                      PRIVATE bfg::lyra cpp_plotly_plotter::plotly_plotter)

add_example(example_reg_image_denoising_tgv2_admm image_denoising_tgv2_admm.cpp)
target_link_libraries(
    example_reg_image_denoising_tgv2_admm
    PRIVATE bfg::lyra cpp_plotly_plotter::plotly_plotter pngpp::pngpp)

add_example(example_reg_upsampling_tv_admm upsampling_tv_admm.cpp)
target_link_libraries(example_reg_upsampling_tv_admm
                      PRIVATE bfg::lyra cpp_plotly_plotter::plotly_plotter)

add_example(example_reg_upsampling_tgv2_admm upsampling_tgv2_admm.cpp)
target_link_libraries(example_reg_upsampling_tgv2_admm
                      PRIVATE bfg::lyra cpp_plotly_plotter::plotly_plotter)

add_example(example_reg_ct_simulation_fista ct_simulation_fista.cpp)
target_link_libraries(example_reg_ct_simulation_fista
                      PRIVATE bfg::lyra cpp_plotly_plotter::plotly_plotter)

add_example(example_reg_ct_simulation_tv_admm ct_simulation_tv_admm.cpp)
target_link_libraries(example_reg_ct_simulation_tv_admm
                      PRIVATE bfg::lyra cpp_plotly_plotter::plotly_plotter)

add_example(example_reg_ct_simulation_tgv2_admm ct_simulation_tgv2_admm.cpp)
target_link_libraries(example_reg_ct_simulation_tgv2_admm
                      PRIVATE bfg::lyra cpp_plotly_plotter::plotly_plotter)
