Directory Structure
======================

.. uml::

    @startsalt
    {
    {T
    + numerical-collection-cpp  | Root directory of this project.
    ++ cmake                    | CMake scripts.
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
    +++ scripts                 | Helper scripts in development.
    +++ test                    | Tests.
    ++++ bench                  | Benchmarks.
    ++++ include                | Include directory of common headers in tests.
    ++++ units                  | Unit tests.
    }
    }
    @endsalt
