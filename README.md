# numerical-collection-cpp

[![GitLab Release](https://img.shields.io/gitlab/v/release/25109105?sort=semver)](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp/-/releases)
[![C++ standard](https://img.shields.io/badge/standard-C%2B%2B23-blue?logo=c%2B%2B)](https://en.cppreference.com/w/cpp/compiler_support/23)
[![pipeline status](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp/badges/main/pipeline.svg)](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp/-/commits/main)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit&logoColor=white)](https://github.com/pre-commit/pre-commit)
[![coverage report](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp/badges/main/coverage.svg)](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp/-/commits/main)
![Scc Count Badge](https://sloc.xyz/github/MusicScience37/numerical-collection-cpp/?lower=true&category=lines)

A collection of algorithms in numerical analysis implemented in C++.

## Supported Compilers

This library should compile with most C++ 23 compliant compilers.

Following compilers are tested in CI:

- GCC 12
- GCC 13
- Clang 19
- Clang 20
- Visual Studio 2022

## Installation

See [Installation](doc/sphinx/src/installation.md).

## Documentation

Documentation built in CI:

| Version     | Documentation                                                   | Full API Reference (Doxygen)                                        |
| :---------- | :-------------------------------------------------------------- | :------------------------------------------------------------------ |
| main branch | [Documentation](https://numcollect.musicscience37.com/)         | [API Reference](https://numcollect.musicscience37.com/api/)         |
| v0.11.0     | [Documentation](https://numcollect.musicscience37.com/v0.11.0/) | [API Reference](https://numcollect.musicscience37.com/v0.11.0/api/) |

## Dependencies (for users)

- [fmt](https://github.com/fmtlib/fmt)
- [Eigen](https://gitlab.com/libeigen/eigen)
- [cpp-hash-tables](https://gitlab.com/MusicScience37Projects/utility-libraries/cpp-hash-tables)
- [toml++](https://github.com/marzer/tomlplusplus)
- [concurrentqueue](https://github.com/cameron314/concurrentqueue)
- [nanoflann](https://github.com/jlblancoc/nanoflann)

## Repositories

- Main in GitLab: [https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp)
- Mirror in GitHub: [https://github.com/MusicScience37/numerical-collection-cpp](https://github.com/MusicScience37/numerical-collection-cpp)

## For Developers

- [How to Build in the Repository](doc/sphinx/src/how_to_build_in_repository.md)

### Profiling

This project uses gperftools for profiling.
See
[documentation of profiler in gperftools](http://gperftools.github.io/gperftools/cpuprofile.html)
for usage of the profiler.

## License

This project is licensed under [Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0).
