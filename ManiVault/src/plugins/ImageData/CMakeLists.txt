cmake_minimum_required(VERSION 3.22)

set(IMAGEDATA "ImageData")
PROJECT(${IMAGEDATA})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

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

add_library(ManiVault::${IMAGEDATA} ALIAS ${IMAGEDATA})

target_include_directories(${IMAGEDATA} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${IMAGEDATA} PRIVATE cxx_std_20)

# Generate a header file that contains the EXPORT macro for this library.
include(GenerateExportHeader)
generate_export_header(${IMAGEDATA})

# Retrieve the file name of the generated export header.
file(GLOB EXPORT_HEADER_FILE_NAME 
    RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)
list(APPEND IMAGE_DATA_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME})

target_link_libraries(${IMAGEDATA} PRIVATE Qt6::Widgets)
target_link_libraries(${IMAGEDATA} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${IMAGEDATA} PRIVATE ${MV_PUBLIC_LIB})
target_link_libraries(${IMAGEDATA} PRIVATE PointData)
target_link_libraries(${IMAGEDATA} PRIVATE ClusterData)

# Use AVX if enabled and available
mv_check_and_set_AVX(${IMAGEDATA} ${MV_USE_AVX})

set_target_properties(${IMAGEDATA} PROPERTIES PUBLIC_HEADER "${IMAGE_DATA_HEADERS}")
set_target_properties(${IMAGEDATA} PROPERTIES AUTOMOC ON)

if(MV_UNITY_BUILD)
    set_target_properties(${IMAGEDATA} PROPERTIES UNITY_BUILD ON)
endif()

install(TARGETS ${IMAGEDATA}
    EXPORT "ManiVaultPublicExport"
    RUNTIME DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Linux/Mac .so
    ARCHIVE DESTINATION "$<CONFIGURATION>/lib/"     COMPONENT LINKLIB # Windows .lib
    PUBLIC_HEADER DESTINATION "$<CONFIGURATION>/include/${IMAGEDATA}/" COMPONENT HEADERS
)

add_custom_command(TARGET ${IMAGEDATA} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${MV_INSTALL_DIR}
)
