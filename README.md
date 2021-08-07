# numerical-collection-cpp

A collection of algorithms in numerical analysis implemented in C++.

[![pipeline status](https://gitlab.com/MusicScience37/numerical-collection-cpp/badges/develop/pipeline.svg)](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/commits/develop)
[![coverage report](https://gitlab.com/MusicScience37/numerical-collection-cpp/badges/develop/coverage.svg)](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/commits/develop)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit&logoColor=white)](https://github.com/pre-commit/pre-commit)

## Documentation

- [Documentation built on develop branch with Sphinx](https://musicscience37.gitlab.io/numerical-collection-cpp/sphinx/)
- [Documentation built on develop branch with Doxygen](https://musicscience37.gitlab.io/numerical-collection-cpp/doxygen/)

## Requirements

- [Python](https://www.python.org/) 3.9
- [pipenv](https://pipenv.pypa.io/en/latest/)
  - Required Python packages can be installed using pipenv.
    - For user: `pipenv install` on this directory.
    - For developers: `pipenv install --dev` on this directory.
- [CMake](https://cmake.org/)
- C++ 17 Compiler
  - Following compilers are tested in CI:
    - GCC 10
    - Clang 11

### Additional Requirements for Developers

- [Doxygen](https://www.doxygen.nl/index.html)
- [Graphviz](https://graphviz.org/)
- [PlantUML](https://plantuml.com)
- Java runtime environment (JRE) for PlantUML

## Build

Build with the following commands:

```bash
cd <this-directory>
pipenv shell
conan remote add gitlab https://gitlab.com/api/v4/packages/conan
mkdir build
cd build
conan install --build missing ..
cmake ..
cmake --build .
```
