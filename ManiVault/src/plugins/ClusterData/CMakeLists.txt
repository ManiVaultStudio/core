cmake_minimum_required(VERSION 3.22)

set(CLUSTERDATA "ClusterData")
PROJECT(${CLUSTERDATA})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CLUSTERD_SOURCES
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

source_group(Plugin FILES ${CLUSTERD_SOURCES})
source_group(Actions FILES ${ACTIONS_SOURCES})
source_group(Model FILES ${MODEL_SOURCES})
source_group(Dialog FILES ${DIALOG_SOURCES})

add_library(${CLUSTERDATA} SHARED ${CLUSTERD_SOURCES} ${ACTIONS_SOURCES} ${MODEL_SOURCES} ${DIALOG_SOURCES})

add_library(ManiVault::${CLUSTERDATA} ALIAS ${CLUSTERDATA})

target_include_directories(${CLUSTERDATA} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${CLUSTERDATA} PRIVATE cxx_std_20)

# Generate a header file that contains the EXPORT macro for this library.
include(GenerateExportHeader)
generate_export_header(${CLUSTERDATA})

# Retrieve the file name of the generated export header.
file(GLOB EXPORT_HEADER_FILE_NAME
    RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)
list(APPEND CLUSTER_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME})

target_link_libraries(${CLUSTERDATA} PRIVATE Qt6::Widgets)
target_link_libraries(${CLUSTERDATA} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${CLUSTERDATA} PRIVATE ${MV_PUBLIC_LIB})
target_link_libraries(${CLUSTERDATA} PRIVATE PointData)

# Use AVX if enabled and available
mv_check_and_set_AVX(${CLUSTERDATA} ${MV_USE_AVX})

set_target_properties(${CLUSTERDATA} PROPERTIES PUBLIC_HEADER "${CLUSTER_HEADERS}")
set_target_properties(${CLUSTERDATA} PROPERTIES AUTOMOC ON)

if(MV_UNITY_BUILD)
    set_target_properties(${CLUSTERDATA} PROPERTIES UNITY_BUILD ON)
endif()

install(TARGETS ${CLUSTERDATA}
    EXPORT "ManiVaultPublicExport"
    RUNTIME DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Linux/Mac .so
    ARCHIVE DESTINATION "$<CONFIGURATION>/lib/"     COMPONENT LINKLIB # Windows .lib
    PUBLIC_HEADER DESTINATION "$<CONFIGURATION>/include/${CLUSTERDATA}/" COMPONENT HEADERS
)

add_custom_command(TARGET ${CLUSTERDATA} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${MV_INSTALL_DIR}
)
