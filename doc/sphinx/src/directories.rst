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
    ++ adr                      | Architectural Decision Records.
    ++ doxygen                  | Configuration of documentation generated using Doxygen.
    ++ sphinx                   | Documentation generated using Sphinx.
    +++ src                     | Source of documentation generated using Sphinx.
    ++ examples                 | Examples.
    ++ include                  | Include directory of this library itself.
    ++ problems                 | Test problems.
    +++ doc                     | Files for documentation of test problems.
    +++ include                 | Include directory of test problems.
    ++ profiling                | Source codes for profiling.
    ++ scripts                  | Helper scripts in development.
    ++ test                     | Tests.
    +++ bench                   | Benchmarks.
    +++ include                 | Include directory of common headers in tests.
    +++ units                   | Unit tests.
    +++ integ                   | Integration tests.
    ++ vcpkg                    | Package manager vcpkg (submodule).
    ++ vcpkg_conf               | Configuration files for vcpkg.
    }
    }
    @endsalt
