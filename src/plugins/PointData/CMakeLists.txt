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
    src/storage/widgets/StorageBackendConfigurationDialog.cpp
    src/storage/eager/EagerPointsStorageBackend.cpp
    src/storage/tiled/TiledPointsStorageBackend.cpp
    src/storage/actions/TiledStorageBackendConversionAction.cpp
    src/storage/tiled/TiledStorageAsymmetryAction.cpp
    src/storage/tiled/PointDataStorageRecommendationDialog.cpp
    src/storage/tiled/PointDataTileCache.cpp
    src/storage/tiled/PackedTileFile.cpp
    src/storage/tiled/PointDataTiledStorage.cpp
    src/storage/tiled/PointDataTiledSerialization.cpp
    src/developer/PointDataPhantomGenerator.cpp
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
    src/actions/InfoAction.h
    src/actions/SelectedIndicesAction.h
    src/actions/ProxyDatasetsAction.h
    src/actions/EditProxyDatasetsAction.h
    src/actions/NumberOfSelectedPointsAction.h
    src/actions/DimensionPickerAction.h
    src/actions/DimensionsPickerAction.h
    src/actions/DimensionsPickerFilterAction.h
    src/actions/DimensionsPickerSelectAction.h
    src/actions/DimensionsPickerMiscellaneousAction.h
    src/actions/DimensionsPickerHolder.h
    src/actions/DimensionsPickerItemModel.h
    src/actions/DimensionsPickerProxyModel.h
    src/actions/ModelResetter.h
    src/actions/CreateSetFromSelectionAction.h
    src/SparseMatrix.h
    src/DimensionNamesSerializer.h
    src/actions/PointsAction.h
    src/actions/PointsViewAction.h
    src/actions/StorageBackendAction.h
    src/storage/widgets/StorageBackendConfigurationDialog.h
    src/view/PointsTableModel.h
    src/view/PointsViewWidget.h
    src/developer/PointDataPhantomGenerator.h
    src/developer/PointDataBenchmark.h
)

set(POINTS_STORAGE_COMMON_HEADERS
    src/storage/interface/PointDataAccess.h
    src/storage/interface/PointsStorageBackend.h
    src/storage/interface/PointsStorageBackendTypes.h
)

set(POINTS_STORAGE_ACTION_HEADERS
    src/storage/actions/StorageBackendConversionAction.h
    src/storage/actions/TiledStorageBackendConversionAction.h
)

set(POINTS_STORAGE_EAGER_HEADERS
    src/storage/eager/EagerPointsStorageBackend.h
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
)

set(POINTS_STORAGE_HEADERS ${POINTS_STORAGE_COMMON_HEADERS} ${POINTS_STORAGE_ACTION_HEADERS} ${POINTS_STORAGE_EAGER_HEADERS} ${POINTS_STORAGE_TILED_HEADERS})

# Note: This nesting of folders is intentional to allow uniform header inclusion in all projects
set(BFLOAT_HEADER
    ${MV_ROOT_DIR}/external/biovault/biovault_bfloat16/biovault_bfloat16.h
)

set(ACTIONS_SOURCES
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
    src/actions/DimensionPickerAction.h
    src/actions/DimensionPickerAction.cpp
    src/actions/DimensionsPickerAction.h
    src/actions/DimensionsPickerAction.cpp
    src/actions/DimensionsPickerHolder.h
    src/actions/DimensionsPickerHolder.cpp
    src/actions/DimensionsPickerItemModel.h
    src/actions/DimensionsPickerItemModel.cpp
    src/actions/DimensionsPickerProxyModel.h
    src/actions/DimensionsPickerProxyModel.cpp
    src/actions/DimensionsPickerFilterAction.h
    src/actions/DimensionsPickerFilterAction.cpp
    src/actions/DimensionsPickerSelectAction.h
    src/actions/DimensionsPickerSelectAction.cpp
    src/actions/DimensionsPickerMiscellaneousAction.h
    src/actions/DimensionsPickerMiscellaneousAction.cpp
    src/actions/ModelResetter.h
    src/actions/ModelResetter.cpp
    src/actions/PointsAction.h
    src/actions/PointsAction.cpp
    src/actions/PointsViewAction.h
    src/actions/PointsViewAction.cpp
    src/actions/StorageBackendAction.h
    src/actions/StorageBackendAction.cpp
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
source_group(Storage\\widgets FILES
    src/storage/widgets/StorageBackendConfigurationDialog.h
    src/storage/widgets/StorageBackendConfigurationDialog.cpp
)
source_group(Storage\\Actions FILES
    ${POINTS_STORAGE_ACTION_HEADERS}
    src/storage/actions/TiledStorageBackendConversionAction.cpp
)
source_group(Storage\\Interface FILES
    ${POINTS_STORAGE_COMMON_HEADERS}
)
source_group(Storage\\eager FILES
    src/storage/eager/EagerPointsStorageBackend.h
    src/storage/eager/EagerPointsStorageBackend.cpp
)
source_group(Storage\\tiled FILES
    ${POINTS_STORAGE_TILED_HEADERS}
    src/storage/tiled/TiledPointsStorageBackend.cpp
    src/storage/tiled/TiledStorageAsymmetryAction.cpp
    src/storage/tiled/PointDataStorageRecommendationDialog.cpp
    src/storage/tiled/PointDataTileCache.cpp
    src/storage/tiled/PackedTileFile.cpp
    src/storage/tiled/PointDataTiledStorage.cpp
    src/storage/tiled/PointDataTiledSerialization.cpp
)
source_group(BFloat FILES ${BFLOAT_HEADER})
source_group(Aux FILES ${POINT_DATA_AUX})

add_library(${POINTDATA} SHARED ${POINTS_SOURCES} ${ACTIONS_SOURCES} ${POINTS_VIEW_WIDGETS} ${POINTS_STORAGE_HEADERS} ${POINT_DATA_AUX})

add_library(ManiVault::${POINTDATA} ALIAS ${POINTDATA})

target_include_directories(${POINTDATA} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")
target_include_directories(${POINTDATA} PRIVATE ${MV_ROOT_DIR}/external/biovault/)

target_compile_features(${POINTDATA} PUBLIC cxx_std_${MV_CXX_STANDARD})

target_compile_options(${POINTDATA}
        PRIVATE
            $<$<AND:$<BOOL:${MV_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:MSVC>>:/WX>
    )

# Generate a header file that contains the EXPORT macro for this library.
include(GenerateExportHeader)
generate_export_header(${POINTDATA})

# Retrieve the file name of the generated export header.
file(GLOB EXPORT_HEADER_FILE_NAME
    RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)
list(APPEND POINTS_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME})

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

install(FILES ${POINTS_STORAGE_COMMON_HEADERS}
    DESTINATION "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/${POINTDATA}/storage/interface/"
    COMPONENT HEADERS
)

install(FILES ${POINTS_STORAGE_ACTION_HEADERS}
    DESTINATION "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/${POINTDATA}/storage/actions/"
    COMPONENT HEADERS
)

install(FILES ${POINTS_STORAGE_EAGER_HEADERS}
    DESTINATION "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/${POINTDATA}/storage/eager/"
    COMPONENT HEADERS
)

install(FILES ${POINTS_STORAGE_TILED_HEADERS}
    DESTINATION "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/${POINTDATA}/storage/tiled/"
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
