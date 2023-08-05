# numerical-collection-cpp

A collection of algorithms in numerical analysis implemented in C++.

[![GitLab Release](https://img.shields.io/gitlab/v/release/25109105?sort=semver)](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp/-/releases)
[![C++ standard](https://img.shields.io/badge/standard-C%2B%2B20-blue?logo=c%2B%2B)](https://en.cppreference.com/w/cpp/compiler_support/20)
[![pipeline status](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp/badges/develop/pipeline.svg)](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp/-/commits/develop)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit&logoColor=white)](https://github.com/pre-commit/pre-commit)
[![coverage report](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp/badges/develop/coverage.svg)](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp/-/commits/develop)

## Repositories

- Main in GitLab: [https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp)
- Mirror in GitHub: [https://github.com/MusicScience37/numerical-collection-cpp](https://github.com/MusicScience37/numerical-collection-cpp)

## Documentation

- [Documentation built on develop branch](https://numcollect.musicscience37.com/)

## How to Use

This library can be used via [vcpkg](https://vcpkg.io/) using following configurations:

- Add a vcpkg registry
  `https://gitlab.com/MusicScience37Projects/vcpkg-registry`
  in `vcpkg-configuration.json`.

  Example:

  ```json
  {
    "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg-configuration.schema.json",
    "default-registry": {
      "kind": "git",
      "repository": "https://github.com/Microsoft/vcpkg",
      "baseline": "78b61582c9e093fda56a01ebb654be15a0033897"
    },
    "registries": [
      {
        "kind": "git",
        "repository": "https://gitlab.com/MusicScience37Projects/vcpkg-registry",
        "baseline": "f7fcc04f34f351a91796eaa0b386017e961a51b4",
        "packages": ["cpp-hash-tables", "numerical-collection-cpp"]
      }
    ]
  }
  ```

- Add `numerical-collection-cpp` in `vcpkg.json`

  Example:

  ```json
  {
    "$schema": "https://raw.githubusercontent.com/microsoft/vcpkg-tool/main/docs/vcpkg.schema.json",
    "dependencies": ["numerical-collection-cpp"]
  }
  ```

## How to Build in the Repository

### Dependencies

A development environment including dependencies can be created using
[Devcontainer of VSCode](https://code.visualstudio.com/docs/remote/containers).

To build this repository,
you will require following dependencies:

- [Python](https://www.python.org/) 3.11
  - You may want to use [pyenv](https://github.com/pyenv/pyenv).
- [poetry](https://python-poetry.org/)
  - Required Python packages can be installed using poetry.
    Execute the command `poetry install` on this directory.
  - [pre-commit](https://pre-commit.com/)
    will be installed via poetry, and used for some checks.
    Execute `poetry run pre-commit install` on this directory
    if you develop this library.
- [CMake](https://cmake.org/)
- C++ 20 Compiler
  - Following compilers are tested in CI:
    - GCC 10
    - GCC 12
    - Clang 14
    - ~~MSVC 19~~ (Currently not tested due to an [Issue in CI](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp/-/issues/195).)

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

1. Start a shell of poetry.

   ```bash
   cd <this-directory>
   poetry install
   poetry shell
   ```

2. Download vcpkg if you haven't already.

   ```bash
   git submodule update --init
   ```

3. Build vcpkg.

   - On Windows:

     ```bat
     .\vcpkg\bootstrap-vcpkg.bat
     ```

   - On Linux:

     ```bash
     ./vcpkg/bootstrap-vcpkg.sh
     ```

4. Configure.

   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
   ```

5. Optionally edit options.

   ```bash
   <some-editor> CMakeCache.txt
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
