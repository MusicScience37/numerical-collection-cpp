# numerical-collection-cpp

A collection of algorithms in numerical analysis implemented in C++ (under development)

## Documentation

- [Documentation built on develop branch with Sphinx](https://musicscience37.gitlab.io/numerical-collection-cpp/sphinx/)
- [Documentation built on develop branch with Doxygen](https://musicscience37.gitlab.io/numerical-collection-cpp/doxygen/)

## Build

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
