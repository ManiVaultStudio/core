cmake_minimum_required(VERSION 3.17)

set(IMAGEDATA "ImageData")
PROJECT(${IMAGEDATA})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# Get the env path and allow the user to change it
set(INSTALL_DIR $ENV{HDPS_INSTALL_DIR} CACHE PATH "The HDPS Install Directoy")
# Normalize the incoming install path
file(TO_CMAKE_PATH ${INSTALL_DIR} INSTALL_DIR)

find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

file(GLOB JSON src/*.json)
set(AUX ${JSON})

set(IMAGE_DATA_HEADERS 
    src/Image.h
    src/ImageData.h
    src/Images.h
    src/Common.h
    ${CMAKE_CURRENT_BINARY_DIR}/imagedata_export.h
)

set(IMAGE_DATA_SOURCES 
    src/Image.cpp
    src/ImageData.cpp
    src/Images.cpp
)

set(ACTIONS_SOURCES 
    src/InfoAction.h
    src/InfoAction.cpp
)

source_group(Actions FILES ${ACTIONS_SOURCES})
source_group(Plugin FILES ${IMAGE_DATA_HEADERS} ${IMAGE_DATA_SOURCES})
source_group(Aux FILES ${AUX})

add_library(${IMAGEDATA} SHARED ${IMAGE_DATA_HEADERS} ${IMAGE_DATA_SOURCES} ${ACTIONS_SOURCES} ${AUX})

add_dependencies(${IMAGEDATA} HDPS_Public PointData ClusterData)

target_include_directories(${IMAGEDATA} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${IMAGEDATA} PRIVATE cxx_std_17)

include(GenerateExportHeader)
generate_export_header(${IMAGEDATA})

# Retrieve the file name of the generated export header.
file(GLOB EXPORT_HEADER_FILE_NAME 
    RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)
list(APPEND IMAGE_DATA_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME})

set(HDPS_LINK_PATH "${INSTALL_DIR}/$<CONFIGURATION>/lib")
set(PLUGIN_LINK_PATH "${INSTALL_DIR}/$<CONFIGURATION>/$<IF:$<CXX_COMPILER_ID:MSVC>,lib,Plugins>")
set(HDPS_LINK_SUFFIX $<IF:$<CXX_COMPILER_ID:MSVC>,${CMAKE_LINK_LIBRARY_SUFFIX},${CMAKE_SHARED_LIBRARY_SUFFIX}>)

set(HDPS_LINK_LIBRARY "${HDPS_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${HDPS_LINK_SUFFIX}")
set(POINTDATA_LINK_LIBRARY "${PLUGIN_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}PointData${HDPS_LINK_SUFFIX}") 
set(CLUSTER_DATA_LINK_LIBRARY "${PLUGIN_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}ClusterData${HDPS_LINK_SUFFIX}") 

target_link_libraries(${IMAGEDATA} PRIVATE Qt6::Widgets)
target_link_libraries(${IMAGEDATA} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${IMAGEDATA} PRIVATE "${HDPS_LINK_LIBRARY}")
target_link_libraries(${IMAGEDATA} PRIVATE "${POINTDATA_LINK_LIBRARY}")
target_link_libraries(${IMAGEDATA} PRIVATE "${CLUSTER_DATA_LINK_LIBRARY}")

set_target_properties(${IMAGEDATA} PROPERTIES PUBLIC_HEADER "${IMAGE_DATA_HEADERS}")

install(TARGETS ${IMAGEDATA}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
    ARCHIVE DESTINATION lib     COMPONENT LINKLIB # Windows .lib
    PUBLIC_HEADER DESTINATION include/${IMAGEDATA} COMPONENT HEADERS
)

add_custom_command(TARGET ${IMAGEDATA} POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
)
