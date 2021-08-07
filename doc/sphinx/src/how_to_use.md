# How to Use

## Via Conan

This library is packaged with [Conan](https://conan.io/),
and available via
[package registry in GitLab](https://gitlab.com/MusicScience37/numerical-collection-cpp/-/packages).

To use this library,
add the package
`num_collect/<version>@MusicScience37+numerical-collection-cpp/stable`
with a version you want
to your `conanfile.py` or `conanfile.txt`,
and add the remote
`https://gitlab.com/api/v4/packages/conan`
to conan command.

## Simply Including Headers

This library is header-only,
so you can simply include headers
setting `include` directory to an include directory of compilers.
Note that you must install additional dependencies,
which are installed automatically when using Conan:

- [fmt](https://fmt.dev/)
- [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)
