# Change Log

## 0.5.0 (on development)

### Feature

- Format Eigen's dense matrices, dense vectors, and sparse matrices ([Issue #142](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/142)).
- Implemented FISTA for L1 regularization ([Issue #147](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/147)).

### Miscellaneous

- Introduced [Approval Tests](https://approvaltests.com/) for tests ([Issue #139](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/139)).
- Update issue templates ([Issue #157](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/157)).
- Fixed warnings on MSVC ([Issue #134](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/134)).
- Set the default value of requirements_for_tests in conanfile.py to False ([Issue #159](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/159)).

## 0.4.1 (2022/2/19)

Correct upload of Conan packages and some improvements in CI ([Issue #146](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/146)).

## 0.4.0 (2022/2/19)

Better logging and use of C++ concepts in C++20.

### Breaking Change

- Changed C++ standard to C++20 ([Issue #105](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/105)).
- Removed the old version of iteration_logger in util module ([Issue #103](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/103)).
- Refactor namespaces ([Issue #104](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/104)).
- Use of C++ 20 concepts ([Issue #104](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/104), [Issue #119](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/119), [Issue #120](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/120), [Issue #123](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/123), [Issue #124](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/124)).
- Change API of integrators ([Issue 130](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/130)).

### Feature

- Modules
  - base (new)
    - Added concepts ([Issue #103](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/103), ([Issue #104](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/104)).
  - logging (new)
    - Implemented logging ([Issue #103](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/103), [Issue #116](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/116)).
  - util
    - Source location in exceptions ([Issue #97](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/97), [Issue #128](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/128)).
    - Logging in assertions ([Issue #103](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/103)).

### Miscellaneous

- Build system
  - Add option to enable OpenMP ([Issue #110](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/110)).
  - Proper configuration of C++ standard libraries ([Issue #127](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/127)).
  - Compatibility with libc++ 12 ([Issue #129](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/129)).
- Benchmarks
  - Change benchmark library ([Issue #108](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/108)).
- Miscellaneous
  - Add GitLab Issue templates ([Issue #109](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/109)).
  - Add .gitmessage for writing Git commit messages ([Issue #109](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/109)).

## 0.3.0 (2021/12/6)

- Modules
  - regularization (new)
    - Tikhonov regularization.
    - L-curve.
    - GCV.
  - interp (new)
    - Kernel interpolation.
  - util
    - Remove lambda functions in macro definition.
    - Helper function to cast integers safely.
  - opt
    - Wrapper class to make ordinary function objects to objective functions in optimization.
    - 1-dimensional global optimization using sampling.
  - numbers
    - Class of floating numbers in a custom number system with given radix.
- Build system
  - Check execution of examples in CI.
- Documentation
  - Check warnings in documentation.
  - Add documentation of some namespaces without documentation comments.
  - Documentation of directory structure.
- Miscellaneous
  - Change cmake-format to cmakelang in Pipfile.
  - Check encoding of source codes.
  - Upgrade Eigen to 3.4.0.
  - Dependency scanning in CI.

## 0.2.0 (2021/8/16)

- Modules
  - auto_diff (new)
    - Implemented forward-mode automatic differentiation.
    - Implemented backward-mode automatic differentiation.
- Build system
  - Refactored documentation.
  - Merged all unit tests to one binary.
- Miscellaneous
  - Started to write change log.
  - Introduced architectural decision records.

## v0.1.0 (2021/8/8)

First release with following modules:

- constants
- functions
- integration
- multi_double
- ode
- opt
- roots
- util
