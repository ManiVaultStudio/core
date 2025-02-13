cmake_minimum_required(VERSION 3.22)

set(COLORDATA "ColorData")
PROJECT(${COLORDATA})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# Check if the directory to the ManiVault installation has been provided
if(NOT DEFINED MV_INSTALL_DIR)
    set(MV_INSTALL_DIR "" CACHE PATH "Directory where ManiVault is installed")
    message(FATAL_ERROR "Please set MV_INSTALL_DIR to the directory where ManiVault is installed")
endif()
file(TO_CMAKE_PATH ${MV_INSTALL_DIR} MV_INSTALL_DIR)

find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

set(COLORDATA_SOURCES
    src/ColorData.h
    src/ColorData.cpp
    src/ColorData.json
)

set(COLORDATA_HEADERS
    src/ColorData.h
)

set(PLUGIN_MOC_HEADERS
    src/ColorData.h
)

source_group( Plugin FILES ${COLORDATA_SOURCES})

add_library(${COLORDATA} SHARED ${COLORDATA_SOURCES})

add_library(ManiVault::${COLORDATA} ALIAS ${COLORDATA})

qt_wrap_cpp(COLORDATA_MOC ${PLUGIN_MOC_HEADERS} TARGET ${COLORDATA})
target_sources(${COLORDATA} PRIVATE ${COLORDATA_MOC})

target_include_directories(${COLORDATA} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${COLORDATA} PRIVATE cxx_std_20)

# Generate a header file that contains the EXPORT macro for this library.
include(GenerateExportHeader)
generate_export_header(${COLORDATA})

# Retrieve the file name of the generated export header.
file(GLOB EXPORT_HEADER_FILE_NAME
    RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)
list(APPEND COLORDATA_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME})

target_link_libraries(${COLORDATA} PRIVATE Qt6::Widgets)
target_link_libraries(${COLORDATA} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${COLORDATA} PRIVATE ${MV_PUBLIC_LIB})

set_target_properties(${COLORDATA} PROPERTIES PUBLIC_HEADER "${COLORDATA_HEADERS}")

install(TARGETS ${COLORDATA}
    EXPORT "ManiVaultPublicExport"
    RUNTIME DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION "$<CONFIGURATION>/Plugins/" COMPONENT PLUGINS # Linux/Mac .so
    ARCHIVE DESTINATION "$<CONFIGURATION>/lib/"     COMPONENT LINKLIB # Windows .lib
    PUBLIC_HEADER DESTINATION "$<CONFIGURATION>/include/${COLORDATA}/" COMPONENT HEADERS
)

add_custom_command(TARGET ${COLORDATA} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${MV_INSTALL_DIR}
)
