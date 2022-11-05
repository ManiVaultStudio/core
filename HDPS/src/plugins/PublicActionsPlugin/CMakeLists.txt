cmake_minimum_required(VERSION 3.1.0)

set(PROJECT "PublicActionsPlugin")

PROJECT(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

file(TO_CMAKE_PATH $ENV{HDPS_INSTALL_DIR} INSTALL_DIR)

find_package(Qt6 6.3.1 COMPONENTS Widgets WebEngineWidgets REQUIRED)

set(PLUGIN
    src/PublicActionsPlugin.h
    src/PublicActionsPlugin.cpp
)

set(WIDGETS
    src/PublicActionsWidget.h
    src/PublicActionsWidget.cpp
)

set(AUX
    src/PublicActionsPlugin.json
)

set(SOURCES
	${PLUGIN}
	${WIDGETS}
	${AUX}
)

source_group(Plugin FILES ${PLUGIN})
source_group(Widgets FILES ${WIDGETS})
source_group(Aux FILES ${AUX})

include_directories("$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/include/")

add_library(${PROJECT} SHARED ${SOURCES})

add_dependencies(${PROJECT} HDPS_Public)

target_link_libraries(${PROJECT} Qt6::Widgets)
target_link_libraries(${PROJECT} Qt6::WebEngineWidgets)

if(MSVC)
    set(LIB_LINK_SUFFIX ".lib") 
else()
    set(LIB_LINK_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif(MSVC)

target_link_libraries(${PROJECT} ${INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${LIB_LINK_SUFFIX})

install(TARGETS ${PROJECT}
   RUNTIME DESTINATION Plugins COMPONENT SHAREDLIB
)

add_custom_command(TARGET ${PROJECT} POST_BUILD
	COMMAND "${CMAKE_COMMAND}"
	--install ${CMAKE_CURRENT_BINARY_DIR}
	--config $<CONFIGURATION>
	--prefix ${INSTALL_DIR}/$<CONFIGURATION>
)

set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug,${INSTALL_DIR}/release>)
set_property(TARGET ${PROJECT} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug/HDPS.exe,${INSTALL_DIR}/release/HDPS.exe>)
