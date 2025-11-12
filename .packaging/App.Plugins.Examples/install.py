import os
import argparse

from pathlib import Path

from common import *

if __name__ == "__main__":
    """
    Com.ManiVault.Plugins.Examples install script

    This script is executed in <installer>/temp/download directory
    """
    
    try:
        parser = argparse.ArgumentParser()
    
        parser.add_argument("operating_system", type=str, help="Type of operating system: Windows, Linux or MacOS")
        parser.add_argument("hdps_version", type=str, help="HDPS version e.g. 0.1, 0.2, 1.0")
        parser.add_argument("compiler", type=str, help="Compiler: Visual Studio or GCC")
        parser.add_argument("compiler_version", type=int, help="Compiler version")
        parser.add_argument("cache_dir", type=str, help="Directory where downloads are cached")
        
        args                = parser.parse_args()
        installer_package   = InstallerPackage(os.path.dirname(__file__))

        installer_package.install_file_in_plugins_dir(Path(os.path.dirname(__file__), "Release", "Plugins"), "ExampleAnalysisPlugin", args.operating_system)
        installer_package.install_file_in_plugins_dir(Path(os.path.dirname(__file__), "Release", "Plugins"), "ExampleDataPlugin", args.operating_system)
        installer_package.install_file_in_plugins_dir(Path(os.path.dirname(__file__), "Release", "Plugins"), "ExampleLoaderPlugin", args.operating_system)
        installer_package.install_file_in_plugins_dir(Path(os.path.dirname(__file__), "Release", "Plugins"), "ExampleTransformationPlugin", args.operating_system)
        installer_package.install_file_in_plugins_dir(Path(os.path.dirname(__file__), "Release", "Plugins"), "ExampleViewJSPlugin", args.operating_system)
        installer_package.install_file_in_plugins_dir(Path(os.path.dirname(__file__), "Release", "Plugins"), "ExampleViewOpenGLPlugin", args.operating_system)
        installer_package.install_file_in_plugins_dir(Path(os.path.dirname(__file__), "Release", "Plugins"), "ExampleViewPlugin", args.operating_system)
        installer_package.install_file_in_plugins_dir(Path(os.path.dirname(__file__), "Release", "Plugins"), "ExampleWriterPlugin", args.operating_system)
                                                      

    except Exception as e:
        print(e)