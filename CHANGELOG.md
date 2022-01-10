# Change Log

## 0.4.0 (on development)

Better logging.

### Breaking Change

- Changed C++ standard to C++20 ([Issue #105](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/105)).
- Removed the old version of iteration_logger in util module ([Issue #103](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/103)).

### Feature

- Modules
  - concepts (new)
    - Added concepts ([Issue #103](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/103)).
  - logging (new)
    - Implemented logging ([Issue #103](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/103), [Issue #116](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/116)).
  - util
    - Source location in exceptions ([Issue #97](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/97)).
    - Logging in assertions ([Issue #103](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/103)).

### Miscellaneous

- Build system
  - Add option to enable OpenMP ([Issue #110](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/issues/110)).
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
