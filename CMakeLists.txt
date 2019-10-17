# CMakeLists to generate a project that has all subprojects of the plugin system in one.

cmake_minimum_required(VERSION 3.1.0)

set(PROJECT "HdpsAllInOne")

PROJECT(${PROJECT})

add_definitions(-DQT_MESSAGELOGCONTEXT)

add_subdirectory(CsvLoader)
add_subdirectory(FcsLoader)
add_subdirectory(BinLoader)
add_subdirectory(HDPS)
add_subdirectory(Scatterplot)

add_dependencies(PointsPlugin HDPS)
add_dependencies(ScatterplotPlugin HDPS)
add_dependencies(ScatterplotPlugin PointsPlugin)
add_dependencies(CsvLoader HDPS)
add_dependencies(CsvLoader PointsPlugin)
add_dependencies(FcsLoader HDPS)
add_dependencies(FcsLoader PointsPlugin)
add_dependencies(BinLoader HDPS)
add_dependencies(BinLoader PointsPlugin)

# Note: The plugins BasicClustering and ImageView are not in used (as Julian Thijssen commented,
# June 24, 2019), so this CMakeLists does not add those subdirectories.

# On the master branch, you may also do add_subdirectory(HDVolLoader)

