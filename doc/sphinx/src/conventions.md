# Conventions

## Tools Used in This Project

### Linters

The following tools are automatically run in DevContainer in VSCode.

| Language | Linter                                                                                                                                       |
| :------- | :------------------------------------------------------------------------------------------------------------------------------------------- |
| C++      | [clang-tidy](https://clang.llvm.org/extra/clang-tidy/)                                                                                       |
| Python   | [flake8](https://flake8.pycqa.org/en/latest/), [Pylint](https://pylint.pycqa.org/en/latest/), [mypy](https://mypy.readthedocs.io/en/stable/) |

### Formatters

The following tools are automatically run in DevContainer in VSCode.

| Language/Format      | Tool                                                                                      |
| :------------------- | :---------------------------------------------------------------------------------------- |
| C++                  | [clang-format](https://clang.llvm.org/docs/ClangFormat.html)                              |
| CMake                | [cmake-format](https://github.com/cheshirekow/cmake_format)                               |
| Python               | [Black](https://black.readthedocs.io/en/stable/), [isort](https://pycqa.github.io/isort/) |
| Markdown, YAML, JSON | [Prettier](https://prettier.io/)                                                          |

### Tools for Documentation

- [Doxygen](https://www.doxygen.nl/index.html)
- [Sphinx](https://www.sphinx-doc.org/)
- [PlantUML](https://plantuml.com/en/)

### Package Managers

| Language | Package Manager                      |
| :------- | :----------------------------------- |
| Python   | [poetry](https://python-poetry.org/) |
| C++      | [vcpkg](https://vcpkg.io)            |

## Naming Conventions

### Naming Conventions for C++

The following table summarizes the C++ naming conventions.
The same conventions are written in the `.clang-tidy` configuration file.

| Kind                                      | Naming Rule   | Notes                                                  |
| :---------------------------------------- | :------------ | :----------------------------------------------------- |
| Namespaces                                | `lower_case`  |                                                        |
| Types (Classes, Structs, Enums)           | `lower_case`  |                                                        |
| Concepts                                  | `lower_case`  |                                                        |
| Template Parameters                       | `CamelCase`   |                                                        |
| Functions                                 | `lower_case`  |                                                        |
| Variables                                 | `lower_case`  |                                                        |
| Constants                                 | `lower_case`  |                                                        |
| Function Parameters                       | `lower_case`  |                                                        |
| Enum Constants                            | `lower_case`  |                                                        |
| Class / Struct Member Variables           | `lower_case`  |                                                        |
| Class / Struct Private Member Variables   | `lower_case_` | Underscore at the end.                                 |
| Class / Struct Protected Member Variables | `lower_case_` | Underscore at the end.                                 |
| Macros                                    | `UPPER_CASE`  | Prefixed by `NUM_COLLECT_` or `INTERNAL_NUM_COLLECT_`. |

### Names of Executables

Executables in this project must be named in `lower_case` format,
and follow the pattern `<kind>_<module>_<name>`,
for example, `ex_interp_interp1d`.

- `<kind>` specifies the kind of the executable as follows:

  | Kind              | Prefix `<kind>` |
  | :---------------- | :-------------- |
  | Unit Tests        | `test_unit`     |
  | Integration Tests | `test_integ`    |
  | Benchmarks        | `bench`         |
  | Experiments       | `experiment`    |
  | Examples          | `example`       |
  | Profiling         | `profile`       |

- `<module>` specifies the module name in this library.
  In most cases, modules use the same name as the directory name,
  but some modules can use shorter names
  due to the limitation of path lengths in some tools used in Windows,
  for example, `integ` for `integration` and `reg` for `regularization`.

- `<name>` specifies what the executable does.
  This can be omitted if the module has only one executable
  as in unit tests.

## Documentation

### Documentation in C++

Documentation in C++ must follow the Doxygen style.

- Use `//!` for single-line comments.
- Use `/*! */` for multi-line comments.
- Commands in Doxygen must be written using backslashes like `\command`.
- `\param` command must be followed by `[in]`, `[out]`, or `[in,out]`.
- `\brief` should be used to write a brief description.
  - For functions, the description should start with a verb in the present tense
    like "Calculate", "Get", etc.
  - For classes, the description should start with "Class", sometimes "Base class".
  - For variables, the description should start with a noun
    like "Coefficient", "Number", etc.
  - For concepts, the description should start with "Concept".
  - In this project, the first word in a description is not written in the third person singular form.

Example of Doxygen comments for a function:

```cpp
/*!
 * \brief This is a brief description of the function.
 *
 * \param[in] param1 This is the first parameter.
 * \param[out] param2 This is the second parameter.
 * \return Return value description.
 *
 * \note If you need to write a note, use the \note command.
 * \warning If you need to write a warning, use the \warning command.
 */
```

Example of Doxygen comments for a class:

```cpp
/*!
 * \brief This is a brief description of the class.
 *
 * \tparam T This is a template parameter.
 *
 * \note If you need to write a note, use the \note command.
 * \warning If you need to write a warning, use the \warning command.
 */
```

Example of Doxygen comments for a variable:

```cpp
//! Single-line comment for a variable.
```

```cpp
/*!
 * \brief This is a brief description of the variable using multi-line comments.
 *
 * \note If you need to write a note, use the \note command.
 * \warning If you need to write a warning, use the \warning command.
 */
```
