from conans import ConanFile


class NumCollectConan(ConanFile):
    name = "num_collect"
    version = "0.0.0"
    description = "A collection of algorithms in numerical analysis implemented in C++"
    homepage = "https://gitlab.com/MusicScience37/numerical-collection-cpp"
    url = "https://gitlab.com/MusicScience37/numerical-collection-cpp.git"
    license = "Apache-2.0"
    author = "Kenta Kabashima (kenta_program37@hotmail.co.jp)"
    topics = ("numerical-analysis")
    settings = None
    exports_sources = "include/*"
    no_copy_source = True
    requires = "fmt/[>=8.0.0]", "eigen/[>=3.3.9]"
    build_requires = (
        'catch2/3.0.0@MusicScience37+conan-extra-packages/stable',
        'celero/2.8.2@MusicScience37+conan-extra-packages/stable',
    )
    generators = "cmake_find_package"

    def package(self):
        self.copy("*.h")

    def package_id(self):
        self.info.header_only()
