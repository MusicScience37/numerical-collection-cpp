add_executable(
    num_collect_bench_laplacian_2d_grid
    laplacian_2d_grid_iterative_test.cpp
    laplacian_2d_grid_sparse_direct_test.cpp laplacian_2d_grid_dense_test.cpp)
target_add_to_benchmark(num_collect_bench_laplacian_2d_grid)
