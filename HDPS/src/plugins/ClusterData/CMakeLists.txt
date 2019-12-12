cmake_minimum_required(VERSION 3.1.0)

set(PROJECT "ClusterData")

PROJECT(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

set(INSTALL_DIR $ENV{HDPS_INSTALL_DIR})

find_package(Qt5 COMPONENTS Widgets WebEngineWidgets REQUIRED)

set(SOURCES
    src/ClusterData.h
    src/ClusterData.cpp
    src/ClusterData.json
)

source_group( Plugin FILES ${SOURCES})

include_directories("$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/include/")

add_library(${PROJECT} SHARED ${SOURCES})
add_dependencies(${PROJECT} HDPS_Public)

if(MSVC)
    set(LIB_LINK_SUFFIX ".lib") 
else()
    set(LIB_LINK_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif(MSVC)

if(APPLE)
    set(HDPS_LINK_LIBRARY "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/HDPS.app/Contents/Frameworks/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${LIB_LINK_SUFFIX}")
else()
    set(HDPS_LINK_LIBRARY "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${LIB_LINK_SUFFIX}")
endif(APPLE)

target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} Qt5::WebEngineWidgets)
target_link_libraries(${PROJECT} "${HDPS_LINK_LIBRARY}")


add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "$<TARGET_FILE:${PROJECT}>"
    "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/Plugins/$<TARGET_FILE_NAME:${PROJECT}>"
)
if(NOT APPLE)
    add_custom_command(TARGET ${PROJECT} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<TARGET_LINKER_FILE:${PROJECT}>"
        "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/$<TARGET_LINKER_FILE_NAME:${PROJECT}>"
    )
endif(NOT APPLE)

# Copy clusters plugin header
add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ClusterData.h"
    "${INSTALL_DIR}/$<CONFIGURATION>/include/ClusterData.h"
)
