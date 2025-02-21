cmake_minimum_required(VERSION 3.22)

set(TEXTDATA "TextData")
PROJECT(${TEXTDATA})

set(TEXTDATA_SOURCES
    src/TextData.h
    src/TextData.cpp
    src/InfoAction.h
    src/InfoAction.cpp
    src/OrderedMap.h
    src/OrderedMap.cpp
    src/TextData.json
)

set(TEXTDATA_HEADERS
    src/TextData.h
    src/InfoAction.h
    src/OrderedMap.h
)

set(PLUGIN_MOC_HEADERS
    src/TextData.h
)

source_group( Plugin FILES ${TEXTDATA_SOURCES})

add_library(${TEXTDATA} SHARED ${TEXTDATA_SOURCES})

add_library(ManiVault::${TEXTDATA} ALIAS ${TEXTDATA})

qt_wrap_cpp(TEXTDATA_MOC ${PLUGIN_MOC_HEADERS} TARGET ${TEXTDATA})
target_sources(${TEXTDATA} PRIVATE ${TEXTDATA_MOC})

target_include_directories(${TEXTDATA} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${TEXTDATA} PRIVATE cxx_std_20)

# Generate a header file that contains the EXPORT macro for this library.
include(GenerateExportHeader)
generate_export_header(${TEXTDATA})

# Retrieve the file name of the generated export header.
file(GLOB EXPORT_HEADER_FILE_NAME
    RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)
list(APPEND TEXTDATA_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME})

target_link_libraries(${TEXTDATA} PRIVATE Qt6::Widgets)
target_link_libraries(${TEXTDATA} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${TEXTDATA} PRIVATE ${MV_PUBLIC_LIB})

set_target_properties(${TEXTDATA} PROPERTIES PUBLIC_HEADER "${TEXTDATA_HEADERS}")

install(TARGETS ${TEXTDATA}
    EXPORT "ManiVaultPublicExport"
    RUNTIME DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Linux/Mac .so
    ARCHIVE DESTINATION "$<CONFIGURATION>/lib/"     COMPONENT LINKLIB # Windows .lib
    PUBLIC_HEADER DESTINATION "$<CONFIGURATION>/include/${TEXTDATA}/" COMPONENT HEADERS
)

add_custom_command(TARGET ${TEXTDATA} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${MV_INSTALL_DIR}
)
