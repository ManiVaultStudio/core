cmake_minimum_required(VERSION 3.22)

set(POINTDATA "PointData")
PROJECT(${POINTDATA} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(POINTS_SOURCES
    src/PointData.h
    src/PointData.cpp
    src/PointDataLegacySerialization.h
    src/PointDataLegacySerialization.cpp
    src/PointDataIterator.h
    src/PointDataRange.h
    src/view/PointView.h
    src/RandomAccessRange.h
    src/SparseMatrix.h
    src/DimensionNamesSerializer.h
    src/DimensionNamesSerializer.cpp
    src/DimensionPickerAction.cpp
    src/DimensionsPickerAction.cpp
    src/DimensionsPickerFilterAction.cpp
    src/DimensionsPickerHolder.cpp
    src/DimensionsPickerItemModel.cpp
    src/DimensionsPickerMiscellaneousAction.cpp
    src/DimensionsPickerProxyModel.cpp
    src/DimensionsPickerSelectAction.cpp
    src/ModelResetter.cpp
    src/developer/PointDataPhantomGenerator.cpp
    src/developer/PointDataTestSuite.cpp
    src/developer/PointDataBenchmark.cpp
)

set(POINTS_VIEW_WIDGETS
    src/view/PointsTableModel.h
    src/view/PointsTableModel.cpp
    src/view/PointsViewWidget.h
    src/view/PointsViewWidget.cpp
)

set(POINTS_HEADERS
    src/PointData.h
    src/PointDataLegacySerialization.h
    src/PointDataIterator.h
    src/PointDataRange.h
    src/view/PointView.h
    src/RandomAccessRange.h
    src/DimensionPickerAction.h
    src/DimensionsPickerAction.h
    src/DimensionsPickerFilterAction.h
    src/DimensionsPickerSelectAction.h
    src/DimensionsPickerMiscellaneousAction.h
    src/DimensionsPickerHolder.h
    src/DimensionsPickerItemModel.h
    src/DimensionsPickerProxyModel.h
    src/ModelResetter.h
    src/actions/InfoAction.h
    src/actions/SelectedIndicesAction.h
    src/actions/ProxyDatasetsAction.h
    src/actions/EditProxyDatasetsAction.h
    src/actions/NumberOfSelectedPointsAction.h
    src/actions/CreateSetFromSelectionAction.h
    src/SparseMatrix.h
    src/DimensionNamesSerializer.h
    src/actions/PointsAction.h
    src/actions/PointsViewAction.h
    src/actions/StorageBackendAction.h
    src/actions/StorageBackendConfiguratorAction.h
    src/view/PointsTableModel.h
    src/view/PointsViewWidget.h
    src/developer/PointDataPhantomGenerator.h
    src/developer/PointDataBenchmark.h
)

set(POINTS_STORAGE_INTERFACE_HEADERS
    src/storage/interface/PointDataAccess.h
    src/storage/interface/PointsStorageBackend.h
    src/storage/interface/PointsStorageBackendTypes.h
    src/storage/interface/StorageBackendConversionAction.h
    src/storage/interface/StorageBackendConfigurationDialog.h
    src/storage/interface/StorageBackendMemory.h
)

set(POINTS_STORAGE_INTERFACE_SOURCES
    src/storage/interface/StorageBackendConfigurationDialog.cpp
    src/storage/interface/StorageBackendConversionAction.cpp
    src/storage/interface/StorageBackendMemory.cpp
)

set(POINTS_STORAGE_DENSE_HEADERS
    src/storage/dense/DensePointsStorageBackend.h
    src/storage/dense/DenseStorageBackendConversionAction.h
)

set(POINTS_STORAGE_DENSE_SOURCES
    src/storage/dense/DensePointsStorageBackend.cpp
    src/storage/dense/DenseStorageBackendConversionAction.cpp
)

set(POINTS_STORAGE_SPARSE_HEADERS
    src/storage/sparse/SparsePointsStorageBackend.h
    src/storage/sparse/SparseStorageBackendConversionAction.h
)

set(POINTS_STORAGE_SPARSE_SOURCES
    src/storage/sparse/SparsePointsStorageBackend.cpp
    src/storage/sparse/SparseStorageBackendConversionAction.cpp
)

set(POINTS_STORAGE_TILED_HEADERS
    src/storage/tiled/PointDataTile.h
    src/storage/tiled/PointDataTileCache.h
    src/storage/tiled/PointDataTiledStorage.h
    src/storage/tiled/PointDataTiledSerialization.h
    src/storage/tiled/PackedTileFile.h
    src/storage/tiled/TiledPointsStorageBackend.h
    src/storage/tiled/TiledStorageAsymmetryAction.h
    src/storage/tiled/TiledStorageGeometry.h
    src/storage/tiled/PointDataStorageRecommendation.h
    src/storage/tiled/PointDataStorageRecommendationDialog.h
    src/storage/tiled/TiledStorageBackendConfigurationAction.h
    src/storage/tiled/TiledStorageBackendConversionAction.h
)

set(POINTS_STORAGE_TILED_SOURCES
    src/storage/tiled/TiledPointsStorageBackend.cpp
    src/storage/tiled/TiledStorageAsymmetryAction.cpp
    src/storage/tiled/PointDataStorageRecommendationDialog.cpp
    src/storage/tiled/TiledStorageBackendConfigurationAction.cpp
    src/storage/tiled/TiledStorageBackendConversionAction.cpp
    src/storage/tiled/PointDataTileCache.cpp
    src/storage/tiled/PackedTileFile.cpp
    src/storage/tiled/PointDataTiledStorage.cpp
    src/storage/tiled/PointDataTiledSerialization.cpp
)

set(POINTS_STORAGE_HEADERS ${POINTS_STORAGE_INTERFACE_HEADERS} ${POINTS_STORAGE_DENSE_HEADERS} ${POINTS_STORAGE_SPARSE_HEADERS} ${POINTS_STORAGE_TILED_HEADERS})

# Note: This nesting of folders is intentional to allow uniform header inclusion in all projects
set(BFLOAT_HEADER
    ${MV_ROOT_DIR}/external/biovault/biovault_bfloat16/biovault_bfloat16.h
)

set(ACTIONS_SOURCES
    src/DimensionPickerAction.h
    src/DimensionPickerAction.cpp
    src/DimensionsPickerAction.h
    src/DimensionsPickerAction.cpp
    src/DimensionsPickerHolder.h
    src/DimensionsPickerHolder.cpp
    src/DimensionsPickerItemModel.h
    src/DimensionsPickerItemModel.cpp
    src/DimensionsPickerProxyModel.h
    src/DimensionsPickerProxyModel.cpp
    src/DimensionsPickerFilterAction.h
    src/DimensionsPickerFilterAction.cpp
    src/DimensionsPickerSelectAction.h
    src/DimensionsPickerSelectAction.cpp
    src/DimensionsPickerMiscellaneousAction.h
    src/DimensionsPickerMiscellaneousAction.cpp
    src/actions/InfoAction.h
    src/actions/InfoAction.cpp
    src/actions/ProxyDatasetsAction.h
    src/actions/ProxyDatasetsAction.cpp
    src/actions/EditProxyDatasetsAction.h
    src/actions/EditProxyDatasetsAction.cpp
    src/actions/NumberOfSelectedPointsAction.h
    src/actions/NumberOfSelectedPointsAction.cpp
    src/actions/SelectedIndicesAction.h
    src/actions/SelectedIndicesAction.cpp
    src/actions/CreateSetFromSelectionAction.h
    src/actions/CreateSetFromSelectionAction.cpp
    src/actions/PointsAction.h
    src/actions/PointsAction.cpp
    src/actions/PointsViewAction.h
    src/actions/PointsViewAction.cpp
    src/actions/StorageBackendAction.h
    src/actions/StorageBackendAction.cpp
    src/actions/StorageBackendConfiguratorAction.h
    src/actions/StorageBackendConfiguratorAction.cpp
)

set(POINT_DATA_AUX
    PluginInfo.json
)

source_group(Plugin FILES ${POINTS_SOURCES})
source_group(Actions FILES ${ACTIONS_SOURCES})
source_group(Widgets FILES ${POINTS_VIEW_WIDGETS})
source_group(View FILES
    src/view/PointView.h
)
source_group(Storage\\Interface FILES
    ${POINTS_STORAGE_INTERFACE_HEADERS}
    ${POINTS_STORAGE_INTERFACE_SOURCES}
)
source_group(Storage\\Dense FILES
    ${POINTS_STORAGE_DENSE_HEADERS}
    ${POINTS_STORAGE_DENSE_SOURCES}
)
source_group(Storage\\Sparse FILES
    ${POINTS_STORAGE_SPARSE_HEADERS}
    ${POINTS_STORAGE_SPARSE_SOURCES}
)
source_group(Storage\\Tiled FILES
    ${POINTS_STORAGE_TILED_HEADERS}
    ${POINTS_STORAGE_TILED_SOURCES}
)
source_group(BFloat FILES ${BFLOAT_HEADER})
source_group(Aux FILES ${POINT_DATA_AUX})

add_library(${POINTDATA} SHARED ${POINTS_SOURCES} ${ACTIONS_SOURCES} ${POINTS_VIEW_WIDGETS} ${POINTS_STORAGE_HEADERS} ${POINTS_STORAGE_INTERFACE_SOURCES} ${POINTS_STORAGE_DENSE_SOURCES} ${POINTS_STORAGE_SPARSE_SOURCES} ${POINTS_STORAGE_TILED_SOURCES} ${POINT_DATA_AUX})

# Prevent anonymous-namespace symbol collisions in unity builds.
set_source_files_properties(
    src/view/PointsTableModel.cpp
    src/view/PointsViewWidget.cpp
    src/developer/PointDataPhantomGenerator.cpp
    src/developer/PointDataTestSuite.cpp
    PROPERTIES SKIP_UNITY_BUILD_INCLUSION ON
)

add_library(ManiVault::${POINTDATA} ALIAS ${POINTDATA})

target_include_directories(${POINTDATA} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")
target_include_directories(${POINTDATA} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/src/view" "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/PointData")
target_include_directories(${POINTDATA} PRIVATE ${MV_ROOT_DIR}/external/biovault/)

target_compile_features(${POINTDATA} PUBLIC cxx_std_${MV_CXX_STANDARD})

target_compile_options(${POINTDATA}
        PRIVATE
            $<$<AND:$<BOOL:${MV_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:MSVC>>:/WX>
    )

# Generate a header file that contains the EXPORT macro for this library.
include(GenerateExportHeader)
generate_export_header(${POINTDATA}
    EXPORT_FILE_NAME "${CMAKE_CURRENT_BINARY_DIR}/PointData/pointdata_export.h"
)

# Add the generated export header to the public headers installed below.
list(APPEND POINTS_HEADERS "${CMAKE_CURRENT_BINARY_DIR}/PointData/pointdata_export.h")

target_link_libraries(${POINTDATA} PRIVATE Qt6::Concurrent Qt6::Widgets)
target_link_libraries(${POINTDATA} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${POINTDATA} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
mv_check_and_set_AVX(${POINTDATA} ${MV_USE_AVX})

if(UNIX AND NOT APPLE)
   find_package(TBB REQUIRED)
   target_link_libraries(${POINTDATA} PRIVATE TBB::tbb)
endif()

set_target_properties(${POINTDATA} PROPERTIES 
    AUTOMOC ON
    PUBLIC_HEADER "${POINTS_HEADERS}"
    UNITY_BUILD ${MV_UNITY_BUILD}
)

install(TARGETS ${POINTDATA}
    EXPORT "ManiVaultPublicExport"
    RUNTIME DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Linux/Mac .so
    ARCHIVE DESTINATION "$<CONFIGURATION>/lib/"     COMPONENT LINKLIB # Windows .lib
    PUBLIC_HEADER DESTINATION "$<CONFIGURATION>/include/${POINTDATA}/" COMPONENT HEADERS
)

install(FILES ${POINTS_STORAGE_INTERFACE_HEADERS}
    DESTINATION "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/${POINTDATA}/storage/interface/"
    COMPONENT HEADERS
)

install(FILES ${POINTS_STORAGE_DENSE_HEADERS}
    DESTINATION "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/${POINTDATA}/storage/dense/"
    COMPONENT HEADERS
)

install(FILES ${POINTS_STORAGE_TILED_HEADERS}
    DESTINATION "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/${POINTDATA}/storage/tiled/"
    COMPONENT HEADERS
)

install(FILES ${POINTS_STORAGE_SPARSE_HEADERS}
    DESTINATION "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/${POINTDATA}/storage/sparse/"
    COMPONENT HEADERS
)

# Copy the external biovault_bfloat16 header into its own subfolder - cmake's install PUBLIC_HEADER flattens all subdirs
install(FILES "${BFLOAT_HEADER}" DESTINATION "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/biovault_bfloat16/")

# PointData is needed to build other plugins - install it now.
add_custom_command(TARGET ${POINTDATA} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${MV_INSTALL_DIR}
)

mv_handle_plugin_config(${POINTDATA})

if(MV_BUILD_TESTING)
    add_subdirectory(gtest)
endif()
