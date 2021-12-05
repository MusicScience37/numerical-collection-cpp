from conans import ConanFile


class NumCollectConan(ConanFile):
    name = "num_collect"
    version = "0.2.0"
    description = "A collection of algorithms in numerical analysis implemented in C++"
    homepage = "https://gitlab.com/MusicScience37/numerical-collection-cpp"
    url = "https://gitlab.com/MusicScience37/numerical-collection-cpp.git"
    license = "Apache-2.0"
    author = "Kenta Kabashima (kenta_program37@hotmail.co.jp)"
    topics = ("numerical-analysis")
    settings = None
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
    generators = "cmake_find_package"

    def requirements(self):
        self.requires("fmt/8.0.1")
        self.requires("eigen/3.4.0")

    def build_requirements(self):
        if self.options.requirements_for_tests:
            self.build_requires(
                "catch2/3.0.0@MusicScience37+conan-extra-packages/stable")
            self.build_requires("celero/2.8.2")
            self.build_requires("pybind11/2.7.1")

    def package(self):
        self.copy("*.h")

    def package_id(self):
        self.info.header_only()
