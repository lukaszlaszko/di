from conans import ConanFile, CMake


class DiConan(ConanFile):
    name = "di"
    version = "1.0"
    license = "Apache-2.0"
    author = "Lukasz Laszko lukaszlaszko@gmail.com"
    url = "https://github.com/lukaszlaszko/di"
    description = "Compile and runtime dependency injection for C++ 14"
    topics = ("dependency injection", "di", "c++", "cpp", "", "cxx")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    generators = "cmake"
    exports_sources = ["CMakeLists.txt", "src/*", "external/*", "tests/*"]

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build(target="di")

    def package(self):
        self.copy("*.hpp", dst="include", src="src")
        self.copy("*.ipp", dst="include", src="src")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["di"]
