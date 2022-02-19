from conans import ConanFile


class NumCollectConan(ConanFile):
    name = "num_collect"
    version = "0.4.1"
    description = "A collection of algorithms in numerical analysis implemented in C++"
    homepage = "https://gitlab.com/MusicScience37/numerical-collection-cpp"
    url = "https://gitlab.com/MusicScience37/numerical-collection-cpp.git"
    license = "Apache-2.0"
    author = "Kenta Kabashima (kenta_program37@hotmail.co.jp)"
    topics = ("numerical-analysis")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "requirements_for_tests": [True, False],
        "stack_traces_support": [True, False],
    }
    default_options = {
        "requirements_for_tests": True,
        "stack_traces_support": True,
    }
    exports_sources = "include/*"
    no_copy_source = True
    generators = "cmake", "cmake_find_package"

    def requirements(self):
        self.requires("fmt/8.1.1")
        self.requires("eigen/3.4.0")

    def build_requirements(self):
        if self.options.requirements_for_tests:
            self.build_requires(
                "catch2/3.0.0pre4@MusicScience37+conan-extra-packages/stable")
            self.build_requires("trompeloeil/42")
            self.build_requires(
                "cpp_stat_bench/0.4.1@MusicScience37+cpp-stat-bench/stable")
            self.build_requires("pybind11/2.8.1")
            self.build_requires("approvaltests.cpp/10.12.1")

    def package(self):
        self.copy("*.h")

    def package_id(self):
        self.info.header_only()
