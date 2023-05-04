cmake_minimum_required(VERSION 3.21)

set(COLORDATA "ColorData")
PROJECT(${COLORDATA})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# Get the env path and allow the user to change it
set(INSTALL_DIR $ENV{HDPS_INSTALL_DIR} CACHE PATH "The HDPS Install Directoy")
# Normalize the incoming install path
file(TO_CMAKE_PATH ${INSTALL_DIR} INSTALL_DIR)

find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

set(COLOR_SOURCES
    src/ColorData.h
    src/ColorData.cpp
    src/ColorData.json
)

set(COLORDATA_HEADERS
    src/ColorData.h
)

source_group( Plugin FILES ${COLOR_SOURCES})

add_library(${COLORDATA} SHARED ${COLOR_SOURCES})

add_dependencies(${COLORDATA} HDPS_Public)

target_include_directories(${COLORDATA} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${COLORDATA} PRIVATE cxx_std_17)

set(HDPS_LINK_PATH "${INSTALL_DIR}/$<CONFIGURATION>/lib")
set(HDPS_LINK_SUFFIX $<IF:$<CXX_COMPILER_ID:MSVC>,${CMAKE_LINK_LIBRARY_SUFFIX},${CMAKE_SHARED_LIBRARY_SUFFIX}>)

set(HDPS_LINK_LIBRARY "${HDPS_LINK_PATH}/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${HDPS_LINK_SUFFIX}")

target_link_libraries(${COLORDATA} PRIVATE Qt6::Widgets)
target_link_libraries(${COLORDATA} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${COLORDATA} PRIVATE "${HDPS_LINK_LIBRARY}")

set_target_properties(${COLORDATA} PROPERTIES PUBLIC_HEADER "${COLORDATA_HEADERS}")

install(TARGETS ${COLORDATA}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
    ARCHIVE DESTINATION lib     COMPONENT LINKLIB # Windows .lib
    PUBLIC_HEADER DESTINATION include/${COLORDATA} COMPONENT HEADERS
)

add_custom_command(TARGET ${COLORDATA} POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
)
