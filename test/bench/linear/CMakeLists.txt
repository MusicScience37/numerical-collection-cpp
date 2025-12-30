add_executable(
    bench_linear_laplacian_2d_grid
    laplacian_2d_grid_iterative_test.cpp
    laplacian_2d_grid_sparse_direct_test.cpp laplacian_2d_grid_dense_test.cpp
    laplacian_2d_grid_main.cpp)
target_add_to_benchmark(bench_linear_laplacian_2d_grid)

add_executable(bench_linear_csrbf_2d csrbf_2d_test.cpp)
target_add_to_benchmark(bench_linear_csrbf_2d)
target_link_libraries(bench_linear_csrbf_2d PRIVATE metis)
