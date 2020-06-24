cmake_minimum_required(VERSION 3.17)

set(PROJECT "ImageData")

PROJECT(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

set(INSTALL_DIR $ENV{HDPS_INSTALL_DIR})

find_package(Qt5 COMPONENTS Widgets WebEngineWidgets REQUIRED)

file(GLOB H src/*.h)
file(GLOB CPP src/*.cpp)
file(GLOB JSON src/*.json)

set(SOURCES ${H} ${CPP})
set(AUX ${JSON})

source_group(Plugin FILES ${SOURCES})
source_group(Aux FILES ${AUX})

include_directories("$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/include/")

add_library(${PROJECT} SHARED ${SOURCES} ${AUX})

set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 14)

include(GenerateExportHeader)
generate_export_header(${PROJECT})

file(GLOB EXPORT_HEADER_FILE_NAME RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)

target_compile_features(${PROJECT} PRIVATE cxx_std_17)

if(MSVC)
    set(LIB_LINK_SUFFIX ".lib") 
else()
    set(LIB_LINK_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif(MSVC)

set(HDPS_LINK_LIBRARY "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${LIB_LINK_SUFFIX}")
set(POINTDATA_LINK_LIBRARY "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}PointData${LIB_LINK_SUFFIX}") 

target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} Qt5::WebEngineWidgets)
target_link_libraries(${PROJECT} "${HDPS_LINK_LIBRARY}")
target_link_libraries(${PROJECT} "${POINTDATA_LINK_LIBRARY}")

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
	"$<TARGET_FILE:${PROJECT}>"
	"$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/Plugins/$<TARGET_FILE_NAME:${PROJECT}>"
)

# Copy image data plugin header
add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
	"${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME}"
	"$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/include/${PROJECT}/${EXPORT_HEADER_FILE_NAME}"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Image.h"
    "${INSTALL_DIR}/$<CONFIGURATION>/include/${PROJECT}/Image.h"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ImageData.h"
    "${INSTALL_DIR}/$<CONFIGURATION>/include/${PROJECT}/ImageData.h"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Images.h"
    "${INSTALL_DIR}/$<CONFIGURATION>/include/${PROJECT}/Images.h"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Common.h"
    "${INSTALL_DIR}/$<CONFIGURATION>/include/${PROJECT}/Common.h"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
	"$<TARGET_LINKER_FILE:${PROJECT}>"
	"$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/$<TARGET_LINKER_FILE_NAME:${PROJECT}>"
)