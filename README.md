# numerical-collection-cpp

A collection of algorithms in numerical analysis implemented in C++.

[![GitLab Release](https://img.shields.io/gitlab/v/release/25109105?sort=semver)](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/releases)
[![C++ standard](https://img.shields.io/badge/standard-C%2B%2B20-blue?logo=c%2B%2B)](https://en.cppreference.com/w/cpp/compiler_support/20)
[![pipeline status](https://gitlab.com/MusicScience37/numerical-collection-cpp/badges/develop/pipeline.svg)](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/commits/develop)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit&logoColor=white)](https://github.com/pre-commit/pre-commit)
[![codecov](https://codecov.io/gl/MusicScience37/numerical-collection-cpp/branch/develop/graph/badge.svg?token=HRS501E1UU)](https://codecov.io/gl/MusicScience37/numerical-collection-cpp)

## Repositories

- Main in GitLab: [https://gitlab.com/MusicScience37/numerical-collection-cpp](https://gitlab.com/MusicScience37/numerical-collection-cpp)
- Mirror in GitHub: [https://github.com/MusicScience37/numerical-collection-cpp](https://github.com/MusicScience37/numerical-collection-cpp)

## Documentation

- [Documentation built on develop branch](https://numcollect.musicscience37.com/)

## How to Use

### Via Conan

This library is packaged with [Conan](https://conan.io/),
and available via
[package registry in GitLab](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/packages).

To use this library,
add the package
`num_collect/<version>@MusicScience37+numerical-collection-cpp/stable`
with a version you want
to your `conanfile.py` or `conanfile.txt`,
and add the remote
`https://gitlab.com/api/v4/packages/conan`
to conan command.

### Simply Including Headers

This library is header-only,
so you can simply include headers
setting `include` directory to an include directory of compilers.
Note that you must install additional dependencies,
which are installed automatically when using Conan:

- [fmt](https://fmt.dev/)
- [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)

## How to Build in the Repository

### Dependencies

A development environment including dependencies can be created using
[Devcontainer of VSCode](https://code.visualstudio.com/docs/remote/containers).

To build this repository,
you will require following dependencies:

- [Python](https://www.python.org/) 3.9
  - You may want to use [pyenv](https://github.com/pyenv/pyenv).
- [pipenv](https://pipenv.pypa.io/en/latest/)
  - Required Python packages can be installed using pipenv.
    Execute the command `pipenv install --dev` on this directory.
  - [pre-commit](https://pre-commit.com/)
    will be installed via pipenv, and used for some checks.
    Execute `pipenv run pre-commit install` on this directory
    if you develop this library.
- [CMake](https://cmake.org/)
- C++ 20 Compiler
  - Following compilers are tested in CI:
    - GCC 10
    - Clang 14
    - MSVC 19

Additionally, you will require following dependencies
to build documentation.

- [Doxygen](https://www.doxygen.nl/index.html)
- [Graphviz](https://graphviz.org/)
- [PlantUML](https://plantuml.com)
  - Set `PLANTUML_JAR_PATH` environment variable to specify the place of `plantuml.jar` file.
- Java runtime environment (JRE) for PlantUML.

### Build commands

After installation of dependencies,
execute the following commands to build:

1. Start a shell of pipenv.

   ```bash
   cd <this-directory>
   pipenv sync --dev
   pipenv shell
   ```

2. Add the Conan remote in GitLab. (Once in an environment. Already done in devcontainer of VSCode.)

   ```bash
   conan remote add gitlab https://gitlab.com/api/v4/packages/conan
   ```

3. Download and install required Conan packages.

   ```bash
   python3 ./scripts/install_conan_dependencies.py <build_type>
   ```

   `build_type` can be `Debug`, `RelWithDebInfo`, or `Release`.

4. Configure.

   ```bash
   cd build
   cmake ..
   ```

5. Optionally edit options.

   ```bash
   <some_editor> CMakeCache.txt
   ```

6. Build.

   ```bash
   cmake --build .
   ```

7. Optionally run tests.

   ```bash
   ctest
   ```

## Profiling

This project uses gperftools for profiling.
See
[documentation of profiler in gperftools](http://gperftools.github.io/gperftools/cpuprofile.html)
for usage of the profiler.

## License

This project is licensed under [Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0).
