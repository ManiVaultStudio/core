# CMakeLists to generate a project that has all subprojects of the plugin system in one.

cmake_minimum_required(VERSION 3.17)

set(PROJECT "hdps-core")

PROJECT(${PROJECT})

add_definitions(-DQT_MESSAGELOGCONTEXT)

option(HDPS_USE_GTEST "Use GoogleTest" OFF)

if (HDPS_USE_GTEST)
    enable_testing()
    add_subdirectory(external/googletest)
endif()

add_subdirectory(HDPS)

