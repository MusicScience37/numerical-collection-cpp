# numerical-collection-cpp

A collection of algorithms in numerical analysis implemented in C++ (under development)

## Documentation

- [Documentation built on develop branch with Sphinx](https://musicscience37.gitlab.io/numerical-collection-cpp/sphinx/)
- [Documentation built on develop branch with Doxygen](https://musicscience37.gitlab.io/numerical-collection-cpp/doxygen/)

## Requirements

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
  - PlantUML requires a Java runtime environment.

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
