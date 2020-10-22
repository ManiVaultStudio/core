# -*- coding: utf-8 -*-

from conans import ConanFile, CMake, tools
import os
import json
import subprocess

#Packages both RELEASE and DEBUG

class HdpsCoreConan(ConanFile):
    name = "hdps-core"
    version = "latest"
    description = "Core libraries and plugind for the High Dimensional Plugin System a.k.a. TBD"
    # topics can get used for searches, GitHub topics, Bintray tags etc. Add here keywords about the library
    topics = ("conan", "analysis", "n-dimensional", "plugin")
    url = "https://github.com/hdps/core"
    branch = "develop"  # should come from profile
    author = "B. van Lew <b.van_lew@lumc.nl>"
    license = "MIT"  # Indicates license type of the packaged library; please use SPDX Identifiers https://spdx.org/licenses/
    exports = ["LICENSE.md"]      # Packages the license for the conanfile.py
    # Remove following lines if the target lib does not use cmake.
    exports_sources = "*"
    generators = "cmake"

    # Options may need to change depending on the packaged library
    settings = {"os": None, "build_type": None, "compiler": None, "arch": None}
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}

    # Custom attributes for Bincrafters recipe conventions
    install_dir = None
    this_dir = os.path.dirname(os.path.realpath(__file__))

    requires = (
        "qt/5.14.2@lkeb/stable",
        "bzip2/1.0.8@conan/stable"
    )

    # Remove runtime and use always default (MD/MDd)
    def configure(self):
        if self.settings.compiler == "Visual Studio":
            del self.settings.compiler.runtime
            
    def system_requirements(self):
        if tools.os_info.is_linux:
            if tools.os_info.with_apt:
                installer = tools.SystemPackageTool()
                installer.install('mesa-common-dev')
                installer.install('libgl1-mesa-dev')
                installer.install('libxcomposite-dev')
                installer.install('libxcursor-dev')
                installer.install('libxi-dev')
                installer.install('libnss3-dev')
                installer.install('libnspr4-dev')
                installer.install('libfreetype6-dev')
                installer.install('libfontconfig1-dev')
                installer.install('libxtst-dev')
                installer.install('libasound2-dev')
                installer.install('libdbus-1-dev')
                min_cmake_version = os.environ.get('CONAN_MINIMUM_CMAKE_VERSION')
                if min_cmake_version is not None:
                    subprocess.run(f"pip3 install cmake>={min_cmake_version}".split())
                    print('Path is: ', os.environ['PATH'])
                    result = subprocess.run("which cmake".split(), stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    os.environ['CONAN_CMAKE_PROGRAM'] = result.stdout.decode('utf-8').rstrip()
                    print(f'Cmake at {os.environ["CONAN_CMAKE_PROGRAM"]}')
        if tools.os_info.is_macos: 
            installer = tools.SystemPackageTool()    
            installer.install('libomp', update=False)              
                
    def config_options(self):
        if self.settings.os == 'Windows':
            del self.options.fPIC

    def _configure_cmake(self, build_type):
        cmake = CMake(self, build_type=build_type)
        if self.settings.os == "Windows" and self.options.shared:
            cmake.definitions["CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS"] = True
        if self.settings.os == "Linux" or self.settings.os == "Macos":
            # cmake.definitions["CMAKE_CXX_STANDARD"] = 14
            cmake.definitions["CMAKE_CXX_STANDARD_REQUIRED"] = "ON"
        # print("Source folder {}".format(self.source_folder))
        cmake.configure(source_folder=self.source_folder)  # (source_folder=self._source_subfolder)
        cmake.verbose = True
        return cmake

    def build(self):
        # If the user has no preference in HDPS_INSTALL_DIR simply set the install dir
        if not os.environ.get('HDPS_INSTALL_DIR', None):
            os.environ['HDPS_INSTALL_DIR'] = os.path.join(self.build_folder, "install")
        print('HDPS_INSTALL_DIR: ', os.environ['HDPS_INSTALL_DIR']) 
        self.install_dir = os.environ['HDPS_INSTALL_DIR']
        cmake_debug = self._configure_cmake('Debug')
        cmake_debug.build()
        
        cmake_debug = self._configure_cmake('Release')
        cmake_debug.build()

    def package(self):
        self.copy(pattern="LICENSE", dst="licenses")  #, src=self._source_subfolder
        # If the CMakeLists.txt has a proper install method, the steps below may be redundant
        # If so, you can just remove the lines below
        self.copy(pattern="*", src=self.install_dir)


    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
