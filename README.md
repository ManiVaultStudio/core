# HDPS Core

# CI-CD

Linux & Macos (Travis) | Windows (Appveyor)
--- | ---
[![Build Status](https://travis-ci.com/bldrvnlw/conan-hdps-core.svg?branch=master)](https://travis-ci.com/bldrvnlw/conan-hdps-core) | [![Build status](https://ci.appveyor.com/api/projects/status/wmef9pb28ma6gv79?svg=true)](https://ci.appveyor.com/project/bldrvnlw/conan-hdps-core)



Pushing to the hdps/core develop wil trigger the conan CI_CD to start a build via the configured Webhook.

Currently the following build marix is performed

OS | Architecture | Compiler
--- | --- | ---
Windows | x64 | MSVC 2017 
Linux | x86_64 | gcc 9
Macos | x86_64 | clang 10

On success the binary and include package is available at [hdps-core in the LKEB Artifactory database](http://cytosplore.lumc.nl:8081/artifactory/webapp/#/artifacts/browse/tree/General/conan-local/bvanlew/hdps-core)

# Plugin State Table
See the current state of available plugins at: https://github.com/hdps/PublicWiki/wiki#plugin-state-table

# Building
### Requirements
* Git (https://git-scm.com/)
* CMake 3.1+ (https://cmake.org/)
* Qt 5.13 (https://www.qt.io/download)
* Visual Studio 2017 (Windows https://visualstudio.microsoft.com/downloads/)

### Setting up
The first step is to pull the repository from GitHub. You can do this in any way you like, but here is one possible way:

1. Create a local folder for the project on your computer
2. Go into the folder and open GitBash or a terminal with git enabled
3. Execute the command `git init` to initialize the Git repository
4. Execute the command `git remote add origin git@github.com:hdps/core.git` to link your local repository to the remote GitHub one.
5. Fetch all branches from the repository using `git fetch -a`
5. Now you can pull in the source files from the `develop` branch by executing `git pull origin develop`
6. If you get a permission denied message on the last step be sure that you have an SSH-key pair and have added the public one to your GitHub account. See: https://help.github.com/en/github/authenticating-to-github/connecting-to-github-with-ssh

### Building
1. Launch CMake (available here: https://cmake.org/)
2. In the source code field browse to the local folder from the last section (which contains CMakeLists.txt).
3. In the build field copy the source code field but append `/Build` at the end. This will put all files necessary for building in the `Build` folder (it will create it if it doesn't exist).
4. Press `Configure` and select the generator for your IDE of choice with the platform of `x64`. Press Finish to configure the project.
5. A lot of red paths should now appear. Check that the ones pointing to Qt directories seem correct and then press `Generate` to generate the solution for your given IDE.
6. Press `Open Project` to launch the IDE and the project.

# Compilation
## Environment variables
Building the plugin system library requires setting an environment variable to the directory where you would like the executable and library to be built.

**On windows:**
Press WindowsKey + Break to open the System window, and click on "Advanced system settings"
Click on "Environment variables" in the Advanced tab.
Add a new user variable by pressing the "New" button at the top.
Fill in `HDPS_INSTALL_DIR` for the Variable name and the path to the directory in the Variable value and press OK.
Press OK in all windows.

**On linux:**
In terminal enter: `sudo -H gedit /etc/environment`
Add a line into this file saying `HDPS_INSTALL_DIR="path/to/your/hdps/folder"`
Save, log out from the current user and log in again to have the changes take effect.

**On mac:**
In a terminal enter: `nano .bash_profile`
Move to the end of the file and add `export HDPS_INSTALL_DIR="path/to/your/hdps/folder"`
Press `Ctrl+X` to exit, `Y` for saving the buffer, and restart the terminal.

## Windows
1. At the top of Visual Studio set the build mode (where it says `Debug`) to `Release`.
2. Right click the project `HDPS` in the Solution Explorer and select Set as StartUp Project.
3. Right click the project again and select Properties.
4. In the Configuration Properties -> Debugging set the `Command` by browsing to your `HDPS.exe` file. Set the `Working Directory` field by browsing to the folder containing the `HDPS.exe` file.
5. Press Apply and OK and right click the project in the Solution Explorer and press Build.
6. If all is well the build should succeed and if you run the project with Ctrl+F5 it will launch `HDPS.exe`.

## Linux | Mac
This depends on your IDE / compiler. Follow their instructions for compiling source code.

## Notes
After first time compiling on macOS it might be necessary to manually moc the MainWindow.ui file:

`uic MainWindow.ui -o ui_MainWindow.h`  
`mv ui_MainWindow.h ../Build/`
