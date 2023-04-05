set(PROJECT "PointData")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

file(TO_CMAKE_PATH $ENV{HDPS_INSTALL_DIR} INSTALL_DIR)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(POINTS_SOURCES
    src/PointData.h
    src/PointData.cpp
    src/PointData.json
    src/PointDataIterator.h
    src/PointDataRange.h
    src/PointView.h
    src/RandomAccessRange.h
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
)

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

source_group(Plugin FILES ${POINTS_SOURCES})
source_group(Actions FILES ${ACTIONS_SOURCES})

add_library(${PROJECT} SHARED ${POINTS_SOURCES} ${ACTIONS_SOURCES})

add_dependencies(${PROJECT} HDPS_Public)

target_include_directories(${PROJECT} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/include/")
target_include_directories(${PROJECT} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../external/biovault/)

target_compile_features(${PROJECT} PRIVATE cxx_std_17)

# Generate a header file that contains the EXPORT macro for this library.
include(GenerateExportHeader)
generate_export_header(${PROJECT})

# Retrieve the file name of the generated export header.
file(GLOB EXPORT_HEADER_FILE_NAME
    RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)
list(APPEND POINTS_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME})

set(HDPS_LINK_PATH "${INSTALL_DIR}/$<CONFIGURATION>/lib")
set(HDPS_LINK_SUFFIX $<IF:$<CXX_COMPILER_ID:MSVC>,${CMAKE_LINK_LIBRARY_SUFFIX},${CMAKE_SHARED_LIBRARY_SUFFIX}>)

set(HDPS_LINK_LIBRARY "${HDPS_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${HDPS_LINK_SUFFIX}")

target_link_libraries(${PROJECT} PRIVATE Qt6::Widgets)
target_link_libraries(${PROJECT} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${PROJECT} PRIVATE "${HDPS_LINK_LIBRARY}")

if(UNIX AND NOT APPLE)
   find_package(TBB REQUIRED)
   target_link_libraries(${PROJECT} PRIVATE TBB::tbb)
endif()

set_target_properties(${PROJECT} PROPERTIES PUBLIC_HEADER "${POINTS_HEADERS}")

install(TARGETS ${PROJECT}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
    ARCHIVE DESTINATION lib     COMPONENT LINKLIB # Windows .lib
    PUBLIC_HEADER DESTINATION include/${PROJECT} COMPONENT HEADERS
)

# Copy the external biovault_bfloat16 header into its own subfolder - cmake's install PUBLIC_HEADER flattens all subdirs
install(CODE "execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  
                ${BFLOAT_HEADER} 
                ${INSTALL_DIR}/$<CONFIGURATION>/include/biovault_bfloat16/biovault_bfloat16.h)"
)

# PointData is needed to build other plugins - install it now.
add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
)

if (HDPS_USE_GTEST)
    add_subdirectory(gtest)
endif()
