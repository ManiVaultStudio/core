cmake_minimum_required(VERSION 3.1.0)

set(PROJECT "TsneAnalysisPlugin")

PROJECT(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

find_package(Qt5Widgets REQUIRED)

set(SOURCES
    src/TsneAnalysisPlugin.h
    src/TsneAnalysisPlugin.cpp
    src/TsneAnalysisPlugin.json
    src/TsneAnalysis.h
    src/TsneAnalysis.cpp
)

source_group( Plugin FILES ${SOURCES})

include_directories("$ENV{CYTOSPLORE_INSTALL_DIR}/$<CONFIGURATION>/include/")

add_library(${PROJECT} SHARED ${SOURCES})

target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} "$ENV{CYTOSPLORE_INSTALL_DIR}/$<CONFIGURATION>/lib/Cytosplore_Public.lib")


add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy
	"$<TARGET_FILE:${PROJECT}>"
	"$ENV{CYTOSPLORE_INSTALL_DIR}/$<CONFIGURATION>/Plugins/$<TARGET_FILE_NAME:${PROJECT}>"
)