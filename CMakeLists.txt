cmake_minimum_required(VERSION 3.17)

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
option(MV_PRECOMPILE_HEADERS "Precompile several headers for faster compilation" ON)
option(MV_UNITY_BUILD "Combine target source files into batches for faster compilation" OFF)

if(MV_PRECOMPILE_HEADERS)
    message(STATUS "Using precompiled headers: ON")
endif()

if(MV_UNITY_BUILD)
    message(STATUS "Using unity build: ON")
endif()

if(MV_USE_AVX)
    message(STATUS "Using AVX: ON")
endif()

# defines MV_VERSION
include(ManiVault/cmake/Utils.cmake)
mv_set_version()

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
