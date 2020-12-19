# CMakeLists to generate a project that has all subprojects of the plugin system in one.

cmake_minimum_required(VERSION 3.17)

set(PROJECT "HDPS")

PROJECT(${PROJECT})

add_definitions(-DQT_MESSAGELOGCONTEXT)

option(HDPS_USE_GTEST "Use GoogleTest" OFF)

if (HDPS_USE_GTEST)
    enable_testing()
    add_subdirectory(external/googletest)
endif()

add_subdirectory(FcsLoader)
add_subdirectory(BinLoader)
add_subdirectory(HDPS)
add_subdirectory(Scatterplot)

add_dependencies(PointData HDPS)
add_dependencies(ScatterplotPlugin HDPS)
add_dependencies(ScatterplotPlugin PointData)
add_dependencies(ScatterplotPlugin ClusterData)
add_dependencies(FcsLoader HDPS)
add_dependencies(FcsLoader PointData)
add_dependencies(BinLoader HDPS)
add_dependencies(BinLoader PointData)

add_dependencies(ImageData HDPS)
add_dependencies(ImageData PointData)
# Note: The plugins BasicClustering and ImageView are not in used (as Julian Thijssen commented,
# June 24, 2019), so this CMakeLists does not add those subdirectories.

# On the master branch, you may also do add_subdirectory(HDVolLoader)

