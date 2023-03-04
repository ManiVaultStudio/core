cmake_minimum_required(VERSION 3.17)

set(PROJECT "ColorData")

PROJECT(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

file(TO_CMAKE_PATH $ENV{HDPS_INSTALL_DIR} INSTALL_DIR)

find_package(Qt6 COMPONENTS Widgets WebEngineWidgets REQUIRED)

set(SOURCES
    src/ColorData.h
    src/ColorData.cpp
    src/ColorData.json
)

set(COLORDATA_HEADERS
    src/ColorData.h
)

source_group( Plugin FILES ${SOURCES})

include_directories("${INSTALL_DIR}/$<CONFIGURATION>/include/")

add_library(${PROJECT} SHARED ${SOURCES})

set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 17)

add_dependencies(${PROJECT} HDPS_Public)

if(MSVC)
    set(LIB_LINK_SUFFIX ".lib") 
else()
    set(LIB_LINK_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif(MSVC)
set(HDPS_LINK_LIBRARY "${INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${LIB_LINK_SUFFIX}")

target_link_libraries(${PROJECT} Qt6::Widgets)
target_link_libraries(${PROJECT} Qt6::WebEngineWidgets)
target_link_libraries(${PROJECT} "${HDPS_LINK_LIBRARY}")

set_target_properties(${PROJECT} PROPERTIES PUBLIC_HEADER "${COLORDATA_HEADERS}")

install(TARGETS ${PROJECT}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS
    ARCHIVE DESTINATION lib COMPONENT LINKLIB
    PUBLIC_HEADER DESTINATION include COMPONENT HEADERS
)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
)
