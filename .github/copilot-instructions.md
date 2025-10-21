# Instructions

## Commands

- In this project, all source code and documentation must be written in English.
- To build the project, use the command `cmake --build .` in the build directory.
  Build directory is at `(project root directory)/build/Debug`.
- To test the project, use the command `ctest -V` in the build directory.

### Update Python Packages

To update Python packages, follow the following instructions:

1. First update Python packages using `poetry update` command.
2. Update Python packages in pyproject.toml file
   using `poetry add -G <group> <package>@latest` command
   for all packages in all groups in pyproject.toml file.
3. Finally update the remaining Python packages using `poetry update` command.

## Conventions

- Conventions are written in [conventions.md](../doc/sphinx/src/conventions.md).
- All source codes in C++ will be compiled with C++23 standard.
