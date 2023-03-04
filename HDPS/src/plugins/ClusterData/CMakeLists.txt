cmake_minimum_required(VERSION 3.17)

set(PROJECT "ClusterData")

PROJECT(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

file(TO_CMAKE_PATH $ENV{HDPS_INSTALL_DIR} INSTALL_DIR)

find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

set(SOURCES
    src/ClusterData.h
    src/ClusterData.cpp
    src/Cluster.h
    src/Cluster.cpp
    src/ClusterData.json
)

set(CLUSTER_HEADERS
    src/Cluster.h
    src/ClusterData.h
    src/ClustersAction.h
    src/ClustersActionWidget.h
    src/ClustersModel.h
    src/ClustersFilterModel.h
    src/RemoveClustersAction.h
    src/MergeClustersAction.h
    src/FilterClustersAction.h
    src/SelectClustersAction.h
    src/SubsetAction.h
    src/RefreshClustersAction.h
    src/ColorizeClustersAction.h
    src/PrefixClustersAction.h
    src/OverwriteClustersConfirmationDialog.h
)

set(ACTIONS_SOURCES
    src/InfoAction.h
    src/InfoAction.cpp
    src/ClustersAction.h
    src/ClustersAction.cpp
    src/ClustersActionWidget.h
    src/ClustersActionWidget.cpp
    src/SubsetAction.h
    src/SubsetAction.cpp
    src/RemoveClustersAction.h
    src/RemoveClustersAction.cpp
    src/MergeClustersAction.h
    src/MergeClustersAction.cpp
    src/SelectClustersAction.h
    src/SelectClustersAction.cpp
    src/FilterClustersAction.h
    src/FilterClustersAction.cpp
    src/RefreshClustersAction.h
    src/RefreshClustersAction.cpp
    src/ColorizeClustersAction.h
    src/ColorizeClustersAction.cpp
    src/PrefixClustersAction.h
    src/PrefixClustersAction.cpp
)

set(MODEL_SOURCES
    src/ClustersModel.h
    src/ClustersModel.cpp
    src/ClustersFilterModel.h
    src/ClustersFilterModel.cpp
)

set(DIALOG_SOURCES
    src/OverwriteClustersConfirmationDialog.h
    src/OverwriteClustersConfirmationDialog.cpp
)

source_group(Plugin FILES ${SOURCES})
source_group(Actions FILES ${ACTIONS_SOURCES})
source_group(Model FILES ${MODEL_SOURCES})
source_group(Dialog FILES ${DIALOG_SOURCES})

include_directories("${INSTALL_DIR}/$<CONFIGURATION>/include/")

add_library(${PROJECT} SHARED ${SOURCES} ${ACTIONS_SOURCES} ${MODEL_SOURCES} ${DIALOG_SOURCES})

set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 17)

add_dependencies(${PROJECT} HDPS_Public)
add_dependencies(${PROJECT} PointData)

# Generate a header file that contains the EXPORT macro for this library.
include(GenerateExportHeader)
generate_export_header(${PROJECT})

# Retrieve the file name of the generated export header.
file(GLOB EXPORT_HEADER_FILE_NAME
    RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)
list(APPEND CLUSTER_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME})

if(MSVC)
    set(LIB_LINK_SUFFIX ".lib") 
else()
    set(LIB_LINK_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif(MSVC)

set(HDPS_LINK_LIBRARY "${INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${LIB_LINK_SUFFIX}")
set(POINTDATA_LINK_LIBRARY "${INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}PointData${LIB_LINK_SUFFIX}") 

target_link_libraries(${PROJECT} Qt6::Widgets)
target_link_libraries(${PROJECT} Qt6::WebEngineWidgets)
target_link_libraries(${PROJECT} "${HDPS_LINK_LIBRARY}")
target_link_libraries(${PROJECT} "${POINTDATA_LINK_LIBRARY}")

set_target_properties(${PROJECT} PROPERTIES PUBLIC_HEADER "${CLUSTER_HEADERS}")

install(TARGETS ${PROJECT}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
    ARCHIVE DESTINATION lib COMPONENT LINKLIB
    PUBLIC_HEADER DESTINATION include COMPONENT HEADERS
)

if (NOT WIN32)
    install(TARGETS ${PROJECT}
        LIBRARY DESTINATION lib COMPONENT LINKLIB # Linux/Mac .so for linking
        PUBLIC_HEADER DESTINATION include COMPONENT HEADERS
    )
endif()

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
)
