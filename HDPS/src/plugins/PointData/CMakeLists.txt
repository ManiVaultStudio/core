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

find_package(Qt5Widgets REQUIRED)

set(POINTS_SOURCES
    src/PointData.h
    src/PointData.cpp
    src/PointData.json
    src/PointDataIterator.h
    src/PointDataRange.h
    src/PointView.h
    src/RandomAccessRange.h
)

source_group( Plugin FILES ${POINTS_SOURCES})

include_directories("$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/include/")

add_library(${PROJECT} SHARED ${POINTS_SOURCES})

set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 14)

target_include_directories(${PROJECT} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/biovault_bfloat16)

add_dependencies(${PROJECT} HDPS_Public)

# Generate a header file that contains the EXPORT macro for this library.
include(GenerateExportHeader)
generate_export_header(${PROJECT})

# Retrieve the file name of the generated export header.
file(GLOB EXPORT_HEADER_FILE_NAME
    RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)

if(MSVC)
    set(LIB_LINK_SUFFIX ".lib") 
else()
    set(LIB_LINK_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif(MSVC)
set(HDPS_LINK_LIBRARY "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${LIB_LINK_SUFFIX}")

target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} Qt5::WebEngineWidgets)
target_link_libraries(${PROJECT} "${HDPS_LINK_LIBRARY}")

# Copy the DLL, the LIB file and the export header of this library to the appropriate subdirectory
# of the installation directory.
add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "$<TARGET_FILE:${PROJECT}>"
    "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/Plugins/$<TARGET_FILE_NAME:${PROJECT}>"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "$<TARGET_LINKER_FILE:${PROJECT}>"
    "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/$<TARGET_LINKER_FILE_NAME:${PROJECT}>"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME}"
    "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/include/${EXPORT_HEADER_FILE_NAME}"
)

# Copy point data header and its depending bfloat16 header
add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PointData.h"
    "${INSTALL_DIR}/$<CONFIGURATION>/include/PointData.h"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PointDataIterator.h"
    "${INSTALL_DIR}/$<CONFIGURATION>/include/PointDataIterator.h"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PointDataRange.h"
    "${INSTALL_DIR}/$<CONFIGURATION>/include/PointDataRange.h"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PointView.h"
    "${INSTALL_DIR}/$<CONFIGURATION>/include/PointView.h"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/RandomAccessRange.h"
    "${INSTALL_DIR}/$<CONFIGURATION>/include/RandomAccessRange.h"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/biovault_bfloat16/biovault_bfloat16.h"
    "${INSTALL_DIR}/$<CONFIGURATION>/include/biovault_bfloat16.h"
)

if (HDPS_USE_GTEST)
    add_subdirectory(gtest)
endif()
