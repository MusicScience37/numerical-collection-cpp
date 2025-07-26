add_executable(bench_rbf_interp interp_1d.cpp interp_2d.cpp interp_3d.cpp)
target_add_to_benchmark(bench_rbf_interp)
