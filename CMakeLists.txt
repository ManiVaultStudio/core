# CMakeLists to generate a project that has all subprojects of the plugin system in one.

cmake_minimum_required(VERSION 3.17)

set(PROJECT "HdpsAllInOne")

PROJECT(${PROJECT})

# Retrieve the submodule(s), as suggested by "Git Submodule Method", from
# https://cliutils.gitlab.io/modern-cmake/chapters/projects/submodule.html
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/biovault_bfloat16/biovault_bfloat16.h")
    message(WARNING "[HDPS] The submodule biovault_bfloat16 was not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_definitions(-DQT_MESSAGELOGCONTEXT)

add_subdirectory(CsvLoader)
add_subdirectory(FcsLoader)
add_subdirectory(BinLoader)
add_subdirectory(HDPS)
add_subdirectory(Scatterplot)

add_dependencies(PointData HDPS)
add_dependencies(ScatterplotPlugin HDPS)
add_dependencies(ScatterplotPlugin PointData)
add_dependencies(ScatterplotPlugin ClusterData)
add_dependencies(CsvLoader HDPS)
add_dependencies(CsvLoader PointData)
add_dependencies(FcsLoader HDPS)
add_dependencies(FcsLoader PointData)
add_dependencies(BinLoader HDPS)
add_dependencies(BinLoader PointData)

add_dependencies(ImageData HDPS)
add_dependencies(ImageData PointData)
# Note: The plugins BasicClustering and ImageView are not in used (as Julian Thijssen commented,
# June 24, 2019), so this CMakeLists does not add those subdirectories.

# On the master branch, you may also do add_subdirectory(HDVolLoader)

