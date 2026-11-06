cmake_minimum_required(VERSION 3.22)

set(MV_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(MV_CXX_STANDARD 20)

set(MV_APPLICATION_NAME "ManiVault Studio")  # Name of the executable
set(MV_EXE MV_Application)                   # The ManiVault executable target
set(MV_PUBLIC_LIB Core)                      # A public shared library used by plugin builders and the ManiVault executable

# defines MV_VERSION
include(cmake/Utils.cmake)
mv_set_version()

# Set OS specific compiler and build settings
include(cmake/mv_project_defaults.cmake)
mv_project_defaults()

PROJECT(mv-core 
        VERSION ${MV_VERSION} 
        DESCRIPTION "Core application and library of ManiVault Studio, a flexible and extensible visual analytics framework for high-dimensional data"
        LANGUAGES CXX C)

if(APPLE)
  enable_language(OBJC)
  enable_language(OBJCXX)
endif()

# -----------------------------------------------------------------------------
# CMake Settings
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD ${MV_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH "${MV_ROOT_DIR}/cmake")
set(CMAKE_INSTALL_MESSAGE LAZY)

# include CMakePackageConfigHelpers macro
include(CMakePackageConfigHelpers)

# Test hardware avx capabilities
include(mv_check_and_set_AVX)

# Helper function to install runtime dependencies
include(mv_install_dependencies_utils)

# Helper function to handle plugin meta data
include(mv_plugin_meta_data)

# Generate a header file that contains the EXPORT macro for public libraries
include(GenerateExportHeader)

# CMake package manager, downloads dependencies during configuration
include(CPM)

# create a file that defines MV_VERSION_MAJOR, etc
configure_file("${MV_ROOT_DIR}/src/lib/ManiVaultVersion.h.in" "${MV_ROOT_DIR}/src/lib/ManiVaultVersion.h" @ONLY)

if(MV_USE_ERROR_LOGGING)
    # Check for size of long (used in sentry)
    include(CheckTypeSize)
    check_type_size("long" CMAKE_SIZEOF_LONG)
endif()

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
option(MV_BUILD_BENCHMARKS "Build ManiVault benchmarks" OFF)
option(MV_USE_AVX "Use AVX if available - by default OFF" OFF)
option(MV_USE_ERROR_LOGGING "Use error logging with Sentry" OFF)
option(MV_PRECOMPILE_HEADERS "Precompile several headers for faster compilation" ON)
option(MV_UNITY_BUILD "Combine target source files into batches for faster compilation" OFF)
option(MV_WARNINGS_AS_ERRORS "Treat warnings as errors when using MSVC" ON)
option(MV_ENABLE_CPPTRACE "Enable cpptrace stack traces" ON)

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
