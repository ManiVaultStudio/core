# CMakeLists to generate a project that has all subprojects of the plugin system in one.

cmake_minimum_required(VERSION 3.17)

set(HDPS_MAIN "hdps-main")
PROJECT(${HDPS_MAIN})

# Get the env path and allow the user to change it
set(INSTALL_DIR $ENV{HDPS_INSTALL_DIR} CACHE PATH "The HDPS Install Directoy")

# Other user-facing options
option(HDPS_USE_GTEST "Use GoogleTest" OFF)
option(HDPS_USE_AVX "Use AVX if available - by default OFF" OFF)

if (HDPS_USE_GTEST)
    enable_testing()
    add_subdirectory(external/googletest)
endif()

add_subdirectory(HDPS)
