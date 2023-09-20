cmake_minimum_required(VERSION 3.17)

set(TEXTDATA "TextData")
PROJECT(${TEXTDATA})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# Get the env path and allow the user to change it
set(INSTALL_DIR $ENV{MV_INSTALL_DIR} CACHE PATH "The ManiVault Install Directoy")
# Normalize the incoming install path
file(TO_CMAKE_PATH ${INSTALL_DIR} INSTALL_DIR)

find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

set(TEXTDATA_SOURCES
    src/TextData.h
    src/TextData.cpp
    src/TextData.json
)

set(TEXTDATA_HEADERS
    src/TextData.h
)

set(PLUGIN_MOC_HEADERS
    src/TextData.h
)

source_group( Plugin FILES ${TEXTDATA_SOURCES})

add_library(${TEXTDATA} SHARED ${TEXTDATA_SOURCES})

qt_wrap_cpp(TEXTDATA_MOC ${PLUGIN_MOC_HEADERS} TARGET ${TEXTDATA})
target_sources(${TEXTDATA} PRIVATE ${TEXTDATA_MOC})

target_include_directories(${TEXTDATA} PRIVATE "${INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${TEXTDATA} PRIVATE cxx_std_17)

target_link_libraries(${TEXTDATA} PRIVATE Qt6::Widgets)
target_link_libraries(${TEXTDATA} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${TEXTDATA} PRIVATE HDPS_Public)

set_target_properties(${TEXTDATA} PROPERTIES PUBLIC_HEADER "${TEXTDATA_HEADERS}")

install(TARGETS ${TEXTDATA}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS # Windows .dll
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS # Linux/Mac .so
    ARCHIVE DESTINATION lib     COMPONENT LINKLIB # Windows .lib
    PUBLIC_HEADER DESTINATION include/${TEXTDATA} COMPONENT HEADERS
)

add_custom_command(TARGET ${TEXTDATA} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
)
