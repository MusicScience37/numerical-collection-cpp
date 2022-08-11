# Change Log

## 0.5.0 (on development)

### Breaking Change

- Changed the name of class `heuristic_1dim_optimizer` to `heuristic_global_optimizer` ([Issue #151](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/merge_requests/151)).
- Refactored logging ([Issue #178](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/178), [Issue #179](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/179), [Issue #164](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/164), [Issue #166](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/166), [Issue #182](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/182), [Issue #181](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/181)).
- Refactored ode module for implementation of new algorithms ([Issue #192](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/192), [Issue #200](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/200), [Issue #208](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/208), [Issue #209](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/209), [Issue #212](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/212), [Issue #216](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/216), [Issue #218](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/218)).

### Feature

- Modules
  - util
    - Format Eigen's dense matrices, dense vectors, and sparse matrices ([Issue #142](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/142)).
  - logging
    - Function to load logging configuration from files ([Issue #166](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/166)).
  - regularization
    - Implemented FISTA for L1 regularization ([Issue #147](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/147)).
  - opt
    - Heuristic global optimizer for multi-variate problems ([Issue #151](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/merge_requests/151)).
  - ode
    - Automatic computation of the initial step size in ODE solver ([Issue #203](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/203)).
    - New algorithm to automatically compute the next step size in ODE solver ([Issue #202](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/202)).
    - RODASP formula ([Issue #199](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/199)).
    - RODASPR formula ([Issue #213](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/213)).

### Miscellaneous

- Introduced [Approval Tests](https://approvaltests.com/) for tests ([Issue #139](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/139), [Issue #204](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/204)).
- Update issue templates ([Issue #157](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/157)).
- Fixed warnings on MSVC ([Issue #134](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/134)).
- Set the default value of requirements_for_tests in conanfile.py to False ([Issue #159](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/159)).
- Introduced profiling ([Issue #168](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/168)).
- Introduced ccache ([Issue #177](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/177)).
- Added concept of optimizers ([Issue #193](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/193)).
- Added more tests of legendre function ([Issue #194](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/194)).
- Changed CI configuration to upload coverage report ([Issue #196](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/196), [Issue #205](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/205), [Issue #206](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/206)).
- Introduced [IWYU](https://github.com/include-what-you-use/include-what-you-use/) ([Issue #9](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/9)).
- Decreased the size of artifacts in CI ([Issue #201](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/201)).
- More example problems of ODEs ([Issue #218](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/218)).

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
