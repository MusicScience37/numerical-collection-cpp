add_example_using_pybind11(num_collect_ex_laplacian_2d_grid
                           laplacian_2d_grid.cpp)

add_example(num_collect_ex_sparse_ordering_methods ordering_methods.cpp)
target_link_libraries(num_collect_ex_sparse_ordering_methods
                      PRIVATE pngpp::pngpp)
