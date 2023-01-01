# How to Use

## Via Conan

This library is packaged with [Conan](https://conan.io/),
and available via
[package registry in GitLab](https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp/-/packages).

To use this library,
add the package
`num_collect/<version>@MusicScience37/stable`
with a version you want
to your `conanfile.py` or `conanfile.txt`,
and add the remote
`https://gitlab.com/api/v4/projects/35726343/packages/conan`,
`https://gitlab.com/api/v4/projects/25109105/packages/conan`
to conan command.

## Simply Including Headers

This library is header-only,
so you can simply include headers
setting `include` directory to an include directory of compilers.
Note that you must install additional dependencies,
which are installed automatically when using Conan:

- [fmt](https://fmt.dev/)
- [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)
- [cpp-hash-tables](https://gitlab.com/MusicScience37Projects/utility-libraries/cpp-hash-tables)
- [toml++](https://marzer.github.io/tomlplusplus/)
