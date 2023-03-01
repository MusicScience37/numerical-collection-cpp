from conans import ConanFile


class NumCollectConan(ConanFile):
    name = "num_collect"
    version = "0.6.0"
    description = "A collection of algorithms in numerical analysis implemented in C++"
    homepage = "https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp"
    url = "https://gitlab.com/MusicScience37Projects/numerical-analysis/numerical-collection-cpp.git"
    license = "Apache-2.0"
    author = "Kenta Kabashima (kenta_program37@hotmail.co.jp)"
    topics = "numerical-analysis"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "requirements_for_tests": [True, False],
    }
    default_options = {
        "requirements_for_tests": False,
        "boost:header_only": True,
    }
    exports_sources = "include/*"
    no_copy_source = True
    generators = "cmake", "cmake_find_package"

    def requirements(self):
        self.requires("fmt/9.1.0")
        self.requires("eigen/3.4.0")
        self.requires("cpp_hash_tables/0.4.1@MusicScience37/stable")
        self.requires("tomlplusplus/3.3.0")

    def build_requirements(self):
        if self.options.requirements_for_tests:
            self.build_requires("catch2/3.3.1")
            self.build_requires("trompeloeil/43")
            self.build_requires("cpp_stat_bench/0.10.0@MusicScience37/stable")
            self.build_requires("pybind11/2.10.1")
            self.build_requires("approvaltests.cpp/10.12.2")
            self.build_requires("pngpp/0.2.10")
            self.build_requires("lyra/1.6.1")

    def package(self):
        self.copy("*.h")

    def package_id(self):
        self.info.header_only()
