# CMakeLists to generate a project that has all subprojects of the plugin system in one.

cmake_minimum_required(VERSION 3.17)

set(MV_MAIN "mv-main")
PROJECT(${MV_MAIN})

# Ask the user to set a ManiVault installation directory
if(NOT DEFINED MV_INSTALL_DIR)
    set(MV_INSTALL_DIR "" CACHE PATH "Directory where ManiVault is, or should be installed")
    message(FATAL_ERROR "Please set MV_INSTALL_DIR to the directory where ManiVault is, or should be installed")
endif()
file(TO_CMAKE_PATH ${MV_INSTALL_DIR} MV_INSTALL_DIR)

# Other user-facing options
option(HDPS_USE_GTEST "Use GoogleTest" OFF)
option(MV_USE_AVX "Use AVX if available - by default OFF" OFF)
option(MV_PRECOMPILE_HEADERS "Precompile several headers for faster compilation" ON)
option(MV_UNITY_BUILD "Combine target source files into batches for faster compilation" OFF)

if (HDPS_USE_GTEST)
    enable_testing()
    add_subdirectory(external/googletest)
endif()

add_subdirectory(HDPS)
