cmake_minimum_required(VERSION 3.22)

set(MV_MAIN "mv-main")
PROJECT(${MV_MAIN})

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
option(MV_USE_GTEST "Use GoogleTest" OFF)
option(MV_USE_AVX "Use AVX if available - by default OFF" OFF)
option(MV_USE_ERROR_LOGGING "Use error logging with Sentry" OFF)
option(MV_PRECOMPILE_HEADERS "Precompile several headers for faster compilation" ON)
option(MV_UNITY_BUILD "Combine target source files into batches for faster compilation" OFF)

# defines MV_VERSION
include(ManiVault/cmake/Utils.cmake)
mv_set_version()

message(STATUS "Building ManiVault ${MV_VERSION_FULL}")
message(STATUS "  Using precompiled headers: ${MV_PRECOMPILE_HEADERS}")
message(STATUS "  Using unity build: ${MV_UNITY_BUILD}")
message(STATUS "  Using AVX: ${MV_USE_AVX}")
message(STATUS "  Using sentry for error logging: ${MV_USE_ERROR_LOGGING}")
message(STATUS "  Install dir: ${MV_INSTALL_DIR}")

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------

if (MV_USE_GTEST)
    message(WARNING "MV_USE_GTEST: currently no test are available")
    #enable_testing()
    #add_subdirectory(external/googletest)
endif()

# -----------------------------------------------------------------------------
# Main project
# -----------------------------------------------------------------------------

# ManiVault core system
add_subdirectory(ManiVault)
