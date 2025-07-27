add_example(example_reg_image_denoising_fista image_denoising_fista.cpp)
target_link_libraries(example_reg_image_denoising_fista
                      PRIVATE bfg::lyra cpp_plotly_plotter::plotly_plotter)

add_example(example_reg_image_denoising_tv_admm image_denoising_tv_admm.cpp)
target_link_libraries(example_reg_image_denoising_tv_admm
                      PRIVATE bfg::lyra cpp_plotly_plotter::plotly_plotter)

add_example(example_reg_image_denoising_tgv2_admm image_denoising_tgv2_admm.cpp)
target_link_libraries(example_reg_image_denoising_tgv2_admm
                      PRIVATE bfg::lyra cpp_plotly_plotter::plotly_plotter)
