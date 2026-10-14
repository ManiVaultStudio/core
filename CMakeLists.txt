cmake_minimum_required(VERSION 3.22)

set(MV_MAIN "mv-main")
PROJECT(${MV_MAIN})

# -----------------------------------------------------------------------------
# CMake Settings
# -----------------------------------------------------------------------------
set(MV_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(MV_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD ${MV_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH "${MV_ROOT_DIR}/cmake")
set(CMAKE_INSTALL_MESSAGE LAZY)

# -----------------------------------------------------------------------------
# User options
# -----------------------------------------------------------------------------

# Ask the user to set a ManiVault installation directory
if(NOT DEFINED MV_INSTALL_DIR)
    set(MV_INSTALL_DIR "" CACHE PATH "Directory where ManiVault is, or should be installed")
    message(FATAL_ERROR "Please set MV_INSTALL_DIR to the directory where ManiVault is, or should be installed")
endif()
file(TO_CMAKE_PATH ${MV_INSTALL_DIR} MV_INSTALL_DIR)

# Other user-facing options
option(MV_BUILD_TESTING "Build ManiVault tests" OFF)
option(MV_USE_AVX "Use AVX if available - by default OFF" OFF)
option(MV_USE_ERROR_LOGGING "Use error logging with Sentry" OFF)
option(MV_PRECOMPILE_HEADERS "Precompile several headers for faster compilation" ON)
option(MV_UNITY_BUILD "Combine target source files into batches for faster compilation" OFF)
option(MV_WARNINGS_AS_ERRORS "Treat warnings as errors when using MSVC" ON)
option(MV_ENABLE_CPPTRACE "Enable cpptrace stack traces" ON)

# defines MV_VERSION
include(cmake/Utils.cmake)
mv_set_version()

message(STATUS "Building ManiVault ${MV_VERSION_FULL}")
message(STATUS "  Using precompiled headers: ${MV_PRECOMPILE_HEADERS}")
message(STATUS "  Using unity build: ${MV_UNITY_BUILD}")
message(STATUS "  Using AVX: ${MV_USE_AVX}")
message(STATUS "  Using sentry for error logging: ${MV_USE_ERROR_LOGGING}")
message(STATUS "  Install dir: ${MV_INSTALL_DIR}")

# -----------------------------------------------------------------------------
# ManiVault core application, library and system plugins
# -----------------------------------------------------------------------------

add_subdirectory(src)

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------

if (MV_BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()
