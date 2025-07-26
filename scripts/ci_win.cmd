@REM Below is used in GitLab runner in gitlab.com
@REM call "C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools\\VC\\Auxiliary\\Build\\vcvarsall.bat" x86_x64

@REM Below is used in my runner.
call "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" x86_x64

cmake .. ^
    -G Ninja ^
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache ^
    -DCMAKE_TOOLCHAIN_FILE=..\vcpkg\scripts\buildsystems\vcpkg.cmake ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DNUM_COLLECT_TESTING:BOOL=ON ^
    -DNUM_COLLECT_ENABLE_UNIT_TESTS=ON ^
    -DNUM_COLLECT_ENABLE_INTEG_TESTS=ON ^
    -DNUM_COLLECT_ENABLE_BENCH=OFF ^
    -DNUM_COLLECT_TEST_BENCHMARKS=OFF ^
    -DNUM_COLLECT_BUILD_EXAMPLES=OFF ^
    -DNUM_COLLECT_BUILD_EXPERIMENTS=OFF ^
    -DNUM_COLLECT_WRITE_JUNIT:BOOL=ON ^
    -DBUILD_SHARED_LIBS:BOOL=ON

cmake --build . --config Release --parallel

ctest -V --build-config Release
