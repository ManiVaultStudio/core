cmake_minimum_required(VERSION 3.22)

set(POINTDATA "PointData")
PROJECT(${POINTDATA})

set(POINTS_SOURCES
    src/PointData.h
    src/PointData.cpp
    src/PointData.json
    src/PointDataIterator.h
    src/PointDataRange.h
    src/PointView.h
    src/RandomAccessRange.h
    src/SparseMatrix.h
)

set(POINTS_HEADERS
    src/PointData.h
    src/PointDataIterator.h
    src/PointDataRange.h
    src/PointView.h
    src/RandomAccessRange.h
    src/InfoAction.h
    src/SelectedIndicesAction.h
    src/ProxyDatasetsAction.h
    src/EditProxyDatasetsAction.h
    src/NumberOfSelectedPointsAction.h
    src/DimensionPickerAction.h
    src/DimensionsPickerAction.h
    src/DimensionsPickerFilterAction.h
    src/DimensionsPickerSelectAction.h
    src/DimensionsPickerMiscellaneousAction.h
    src/DimensionsPickerHolder.h
    src/DimensionsPickerItemModel.h
    src/DimensionsPickerProxyModel.h
    src/ModelResetter.h
    src/CreateSetFromSelectionAction.h
    src/SparseMatrix.h
)

# Note: This nesting of folders is intentional to allow uniform header inclusion in all projects
set(BFLOAT_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../external/biovault/biovault_bfloat16/biovault_bfloat16.h
)

set(ACTIONS_SOURCES
    src/InfoAction.h
    src/InfoAction.cpp
    src/ProxyDatasetsAction.h
    src/ProxyDatasetsAction.cpp
    src/EditProxyDatasetsAction.h
    src/EditProxyDatasetsAction.cpp
    src/NumberOfSelectedPointsAction.h
    src/NumberOfSelectedPointsAction.cpp
    src/SelectedIndicesAction.h
    src/SelectedIndicesAction.cpp
    src/CreateSetFromSelectionAction.h
    src/CreateSetFromSelectionAction.cpp
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
    src/ModelResetter.h
    src/ModelResetter.cpp
)

set(PLUGIN_MOC_HEADERS
    src/PointData.h
    src/ProxyDatasetsAction.h
    src/EditProxyDatasetsAction.h
    src/CreateSetFromSelectionAction.h
    src/DimensionPickerAction.h
    src/DimensionsPickerAction.h
)

source_group(Plugin FILES ${POINTS_SOURCES})
source_group(Actions FILES ${ACTIONS_SOURCES})

add_library(${POINTDATA} SHARED ${POINTS_SOURCES} ${ACTIONS_SOURCES})

add_library(ManiVault::${POINTDATA} ALIAS ${POINTDATA})

qt_wrap_cpp(POINTDATA_MOC ${PLUGIN_MOC_HEADERS} TARGET ${POINTDATA})
target_sources(${POINTDATA} PRIVATE ${POINTDATA_MOC})

target_include_directories(${POINTDATA} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")
target_include_directories(${POINTDATA} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../external/biovault/)

target_compile_features(${POINTDATA} PRIVATE cxx_std_20)

# Generate a header file that contains the EXPORT macro for this library.
include(GenerateExportHeader)
generate_export_header(${POINTDATA})

# Retrieve the file name of the generated export header.
file(GLOB EXPORT_HEADER_FILE_NAME
    RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)
list(APPEND POINTS_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME})

target_link_libraries(${POINTDATA} PRIVATE Qt6::Widgets)
target_link_libraries(${POINTDATA} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${POINTDATA} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
mv_check_and_set_AVX(${POINTDATA} ${MV_USE_AVX})

if(UNIX AND NOT APPLE)
   find_package(TBB REQUIRED)
   target_link_libraries(${POINTDATA} PRIVATE TBB::tbb)
endif()

if(MV_UNITY_BUILD)
    set_target_properties(${POINTDATA} PROPERTIES UNITY_BUILD ON)
    set_source_files_properties(${POINTDATA_MOC} PROPERTIES SKIP_UNITY_BUILD_INCLUSION TRUE)
endif()

set_target_properties(${POINTDATA} PROPERTIES PUBLIC_HEADER "${POINTS_HEADERS}")
set_target_properties(${POINTDATA} PROPERTIES AUTOMOC ON)

install(TARGETS ${POINTDATA}
    EXPORT "ManiVaultPublicExport"
    RUNTIME DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Linux/Mac .so
    ARCHIVE DESTINATION "$<CONFIGURATION>/lib/"     COMPONENT LINKLIB # Windows .lib
    PUBLIC_HEADER DESTINATION "$<CONFIGURATION>/include/${POINTDATA}/" COMPONENT HEADERS
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

# Currentlt, no tests are available
#if (MV_USE_GTEST)
#    add_subdirectory(gtest)
#endif()
