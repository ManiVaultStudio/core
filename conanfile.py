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
import re
    
class HdpsCoreConan(ConanFile):
    """Class to package hdps-core using conan

    Packages both RELEASE and DEBUG.
    Uses rules_support (github.com/ManiVaultStudio/rulessupport) to derive
    versioninfo based on the branch naming convention
    as described in https://github.com/ManiVaultStudio/core/wiki/Branch-naming-rules
    """

    name = "hdps-core"
    description = (
        "Core libraries and plugins for ManiVault"
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

    requires = ("qt/6.8.2@lkeb/stable")

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

    # Remove runtime and use always default (MD/MDd)
    def configure(self):
        pass
        # Needed for toolchain
        # if self.settings.compiler == "Visual Studio":
        #    del self.settings.compiler.runtime

    def system_requirements(self):
        if tools.os_info.is_linux:
            if not tools.os_info.with_apt:
                print("Cannot install linux dependencies - no apt available")
                return

            linux_requirements = [
                "mesa-common-dev", 
                "libgl1-mesa-dev",
                "libxcomposite-dev",
                "libxkbcommon-dev",
                "libxkbcommon-x11-dev",
                "libxcursor-dev",
                "libxi-dev",
                "libnss3-dev",
                "libnspr4-dev",
                "libfreetype6-dev",
                "libfontconfig1-dev",
                "libxtst-dev",
                "libasound2-dev",
                "libdbus-1-dev",
                "libcups2-dev",
                "libicu-dev",
                "libcurl4-openssl-dev"      # for sentry
                ]
            
            installer = tools.SystemPackageTool()

            for package in linux_requirements:
                installer.install(package)
            
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def generate(self):
        generator = None
        if self.settings.os == "Macos":
            generator = "Xcode"
        if self.settings.os == "Linux":
            generator = "Ninja Multi-Config"

        tc = CMakeToolchain(self, generator=generator)

        tc.variables["CMAKE_CXX_STANDARD_REQUIRED"] = "ON"

        # Use the Qt provided .cmake files
        qt_path = pathlib.Path(self.deps_cpp_info["qt"].rootpath)
        qt_cfg = list(qt_path.glob("**/Qt6Config.cmake"))[0]
        qt_dir = qt_cfg.parents[0].as_posix()
        qt_root = qt_cfg.parents[2].as_posix()

        # for qt & ads
        tc.variables["Qt6_ROOT"] = qt_root
        tc.variables["Qt6_DIR"] = qt_dir
        tc.variables["QT_DIR"] = qt_dir
        tc.variables["CMAKE_PREFIX_PATH"] = f"{qt_root}"

        # Set the installation directory for ManiVault based on the MV_INSTALL_DIR environment variable
        # or if none is specified, set it to the build/install dir.
        if not os.environ.get("MV_INSTALL_DIR", None):
            os.environ["MV_INSTALL_DIR"] = os.path.join(self.build_folder, "install")
        print("MV_INSTALL_DIR: ", os.environ["MV_INSTALL_DIR"])
        self.install_dir = pathlib.Path(os.environ["MV_INSTALL_DIR"]).as_posix()

        # Give the installation directory to CMake
        tc.variables["MV_INSTALL_DIR"] = self.install_dir

        # Set some build options
        tc.variables["MV_PRECOMPILE_HEADERS"] = "ON"
        tc.variables["MV_UNITY_BUILD"] = "ON"

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
        print("**** Build RELWITHDEBINFO *****")
        cmake.build(build_type="RelWithDebInfo")
        print("**** Install RELWITHDEBINFO *****")
        cmake.install(build_type="RelWithDebInfo")

        print("**** Build RELEASE *****")
        cmake.build(build_type="Release")
        print("**** Install RELEASE *****")
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
        if self.settings.os == "Macos": 
            #git = tools.Git()
            #branch_name = str(git.get_branch())
            #release_tag = re.search(r"^release-|release\/(.*)$", branch_name)
            #if (self.settings.os == "Macos") and (release_tag is not None):
            self.options.macos_bundle = True
        print("Options macos: ", self.options.macos_bundle)
        if self.options.macos_bundle:
            print("Macos including bundle")
        else:
            print("No macos_bundle in package")

        if False == self.options.macos_bundle and self.settings.os == "Macos":
            # remove the bundle before packaging -
            # it contains the complete QtWebEngine > 1GB
            shutil.rmtree(str(pathlib.Path(self.install_dir, "RelWithDebInfo/ManiVault Studio.app")))
            shutil.rmtree(str(pathlib.Path(self.install_dir, "Release/ManiVault Studio.app")))
        elif self.settings.os == "Macos":
            # also remove Release even in bundle build to keep package size down
            shutil.rmtree(str(pathlib.Path(self.install_dir, "Release/ManiVault Studio.app")))

        # Add the pdb files next to the libs for RelWithDebInfo linking
        if tools.os_info.is_windows:
            pdb_dest = pathlib.Path(self.install_dir, "RelWithDebInfo/lib")
            # pdb_dest.mkdir()
            pdb_files = pathlib.Path(self.build_folder).glob("hdps/RelWithDebInfo/*.pdb")
            for pfile in pdb_files:
                shutil.copy(pfile, pdb_dest)

        self.copy(pattern="*", src=self.install_dir, symlinks=True)

    def package_info(self):
        self.cpp_info.relwithdebinfo.libdirs = ["RelWithDebInfo/lib"]
        self.cpp_info.relwithdebinfo.bindirs = ["RelWithDebInfo/Plugins", "RelWithDebInfo"]
        self.cpp_info.relwithdebinfo.includedirs = ["RelWithDebInfo/include", "RelWithDebInfo"]
        self.cpp_info.release.libdirs = ["Release/lib"]
        self.cpp_info.release.bindirs = ["Release/Plugins", "Release"]
        self.cpp_info.release.includedirs = ["Release/include", "Release"]
