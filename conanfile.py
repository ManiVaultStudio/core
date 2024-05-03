# -*- coding: utf-8 -*-

from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMake, CMakeToolchain
from conans.tools import save, load
from conans import tools
import os
import sys
import pathlib
import shutil
from rules_support import CoreBranchInfo
import subprocess
import traceback

class HdpsCoreConan(ConanFile):
    """Class to package hdps-core using conan

    Packages both RELEASE and DEBUG.
    Uses rules_support (github.com/hdps/rulessupport) to derive
    versioninfo based on the branch naming convention
    as described in https://github.com/ManiVaultStudio/core/wiki/Branch-naming-rules
    """

    name = "hdps-core"
    description = (
        "Core libraries and plugind for the High Dimensional Plugin System a.k.a. TBD"
    )
    # topics can get used for searches, GitHub topics, Bintray tags etc.
    # Add here keywords about the library
    topics = ("conan", "analysis", "n-dimensional", "plugin")
    url = "https://github.com/ManiVaultStudio/core"
    branch = "develop"  # should come from profile
    author = "B. van Lew <b.van_lew@lumc.nl>"
    license = (
        "LGPL-3.0-or-later"  # Indicates license: use SPDX Identifiers https://spdx.org/licenses/
    )
    short_paths = True
    generators = "CMakeDeps"
    force_macbundle = False

    # Options may need to change depending on the packaged library
    settings = {"os": None, "build_type": None, "compiler": None, "arch": None}
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "macos_bundle": [True, False],
    }
    default_options = {"shared": True, "fPIC": True, "macos_bundle": False}

    # Custom attributes for Bincrafters recipe conventions
    install_dir = None
    this_dir = os.path.dirname(os.path.realpath(__file__))

    requires = ("qt/6.3.2@lkeb/stable", "zlib/1.3")

    scm = {"type": "git", "subfolder": "hdps/core", "url": "auto", "revision": "auto"}

    def __get_git_path(self):
        path = load(
            pathlib.Path(pathlib.Path(__file__).parent.resolve(), "__gitpath.txt")
        )
        print(f"git info from {path}")
        return path

    def export(self):
        print("In export")
        # save the original source path to the directory used to build the package
        save(
            pathlib.Path(self.export_folder, "__gitpath.txt"),
            str(pathlib.Path(__file__).parent.resolve()),
        )

    def set_version(self):
        # Assign a version from the branch name
        branch_info = CoreBranchInfo(self.recipe_folder)
        self.version = branch_info.version
        # for release versions force the macos_bundle option to on
        if branch_info.release_status:
            self.force_macbundle = True
            
    # Remove runtime and use always default (MD/MDd)
    def configure(self):
        pass
        # Needed for toolchain
        # if self.settings.compiler == "Visual Studio":
        #    del self.settings.compiler.runtime

    def system_requirements(self):
        if tools.os_info.is_linux:
            if tools.os_info.with_apt:
                installer = tools.SystemPackageTool()
                installer.install("mesa-common-dev")
                installer.install("libgl1-mesa-dev")
                installer.install("libxcomposite-dev")
                installer.install("libxcursor-dev")
                installer.install("libxi-dev")
                installer.install("libnss3-dev")
                installer.install("libnspr4-dev")
                installer.install("libfreetype6-dev")
                installer.install("libfontconfig1-dev")
                installer.install("libxtst-dev")
                installer.install("libasound2-dev")
                installer.install("libdbus-1-dev")
                min_cmake_version = os.environ.get("CONAN_MINIMUM_CMAKE_VERSION")
                if min_cmake_version is not None:
                    subprocess.run(f"pip3 install cmake>={min_cmake_version}".split())
                    print("Path is: ", os.environ["PATH"])
                    result = subprocess.run(
                        "which cmake".split(),
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                    )
                    os.environ["CONAN_CMAKE_PROGRAM"] = result.stdout.decode(
                        "utf-8"
                    ).rstrip()
                    print(f'Cmake at {os.environ["CONAN_CMAKE_PROGRAM"]}')
        # if tools.os_info.is_macos:
        #    installer = tools.SystemPackageTool()
        #    installer.install("libomp", update=False)

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def generate(self):
        # This prevents overlap between the hdps/core (source folder)
        # and the HDPS (build) folder. This happens in the Macos build
        # Build folder can't be set here since conan 1.50
        # possibly via CMakeDeps and CMakeToolchain
        # self.build_folder = self.build_folder + '/hdps-common'
        deps = CMakeDeps(self)
        deps.generate()

        generator = None
        # TODO Generators can be moved to profiles
        if self.settings.os == "Macos":
            generator = "Xcode"
        if self.settings.os == "Linux":
            generator = "Ninja Multi-Config"
        tc = CMakeToolchain(self, generator=generator)
        if self.settings.os == "Windows" and self.options.shared:
            tc.variables["CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS"] = True
        if self.settings.os == "Linux" or self.settings.os == "Macos":
            tc.variables["CMAKE_CXX_STANDARD_REQUIRED"] = "ON"
        # Use the Qt provided .cmake files
        qtpath = pathlib.Path(self.deps_cpp_info["qt"].rootpath)
        qt_root = str(list(qtpath.glob("**/Qt6Config.cmake"))[0].parents[3].as_posix())
        tc.variables["CMAKE_PREFIX_PATH"] = f"{qt_root}"
        #tc.variables["Qt6_ROOT"] = qt_root
        
        # Set the installation directory for ManiVault based on the MV_INSTALL_DIR environment variable
        # or if none is specified, set it to the build/install dir.
        if not os.environ.get("MV_INSTALL_DIR", None):
            os.environ["MV_INSTALL_DIR"] = os.path.join(self.build_folder, "install")
        print("MV_INSTALL_DIR: ", os.environ["MV_INSTALL_DIR"])
        self.install_dir = pathlib.Path(os.environ["MV_INSTALL_DIR"]).as_posix()
        # Give the installation directory to CMake
        tc.variables["MV_INSTALL_DIR"] = self.install_dir
        
        zlibpath = pathlib.Path(self.deps_cpp_info["zlib"].rootpath).as_posix()
        tc.variables["ZLIB_ROOT"] = zlibpath

        # Set some build options
        tc.variables["MV_PRECOMPILE_HEADERS"] = "ON"
        tc.variables["MV_UNITY_BUILD"] = "ON"

        # OS specific settings 
        if self.settings.os == "Linux":
            tc.variables["CMAKE_CONFIGURATION_TYPES"] = "Debug;Release"
        try:
            tc.generate()
        except KeyError as e:
            print("Exception!", sys.exc_info()[0])
            print(e)
            traceback.print_exc()
            raise e

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.verbose = True
        cmake.configure(build_script_folder="hdps/core")
        return cmake

    def build(self):
        print("Build OS is : ", self.settings.os)
        print(f"In build, build folder {self.build_folder}")
        # save the last build folder to allow double packaging with export-pkg
        save(
            pathlib.Path(self.__get_git_path(), "__last_build_folder.txt"),
            self.build_folder,
        )

        cmake = self._configure_cmake()
        cmake.build(build_type="Debug")
        cmake.install(build_type="Debug")

        # cmake_release = self._configure_cmake()
        cmake.build(build_type="Release")
        cmake.install(build_type="Release")

    def package(self):
        # if just running package
        if self.install_dir is None:
            if not os.environ.get("MV_INSTALL_DIR", None):
                os.environ["MV_INSTALL_DIR"] = os.path.join(
                    self.build_folder, "install"
                )
            self.install_dir = os.environ["MV_INSTALL_DIR"]

        print("Packaging install dir: ", self.install_dir)
        print("Options macos: ", self.options.macos_bundle)
        if self.options.macos_bundle or self.force_macbundle:
            print("Macos including bundle")
        else:
            print("No macos_bundle in package")

        if self.settings.os == "Macos" and False == self.options.macos_bundle and False == self.force_macbundle:
            # remove the bundle before packaging -
            # it contains the complete QtWebEngine > 1GB
            shutil.rmtree(str(pathlib.Path(self.install_dir, "Debug/ManiVault Studio.app")))
            shutil.rmtree(str(pathlib.Path(self.install_dir, "Release/ManiVault Studio.app")))

        # Add the pdb files next to the libs for debug linking
        if tools.os_info.is_windows:
            pdb_dest = pathlib.Path(self.install_dir, "Debug/lib")
            # pdb_dest.mkdir()
            pdb_files = pathlib.Path(self.build_folder).glob("hdps/Debug/*.pdb")
            for pfile in pdb_files:
                shutil.copy(pfile, pdb_dest)

        self.copy(pattern="*", src=self.install_dir)

    def package_info(self):
        self.cpp_info.debug.libdirs = ["Debug/lib"]
        self.cpp_info.debug.bindirs = ["Debug/Plugins", "Debug"]
        self.cpp_info.debug.includedirs = ["Debug/include", "Debug"]
        self.cpp_info.release.libdirs = ["Release/lib"]
        self.cpp_info.release.bindirs = ["Release/Plugins", "Release"]
        self.cpp_info.release.includedirs = ["Release/include", "Release"]
