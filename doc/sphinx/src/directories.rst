Directory Structure
======================

.. uml::

    @startsalt
    {
    {T
    + numerical-collection-cpp  | Root directory of this project.
    ++ cmake                    | CMake scripts.
    ++ config_schema            | Schema of configuration files.
    ++ doc                      | Documentation.
    +++ doxygen                 | Configuration of documentation generated using Doxygen.
    +++ sphinx                  | Documentation generated using Sphinx.
    ++ examples                 | Examples.
    ++ experiments              | Experiments which are not tests, but not examples for users.
    ++ include                  | Include directory of this library itself.
    ++ problems                 | Test problems.
    +++ doc                     | Files for documentation of test problems.
    +++ include                 | Include directory of test problems.
    ++ profiling                | Source codes for profiling.
    ++ scripts                  | Helper scripts in development.
    ++ src                      | Source codes of this library.
    ++ test                     | Tests.
    +++ bench                   | Benchmarks.
    +++ include                 | Include directory of common headers in tests.
    +++ integ                   | Integration tests.
    +++ units                   | Unit tests.
    ++ vcpkg                    | Package manager vcpkg (submodule).
    ++ vcpkg_conf               | Configuration files for vcpkg.
    }
    }
    @endsalt
