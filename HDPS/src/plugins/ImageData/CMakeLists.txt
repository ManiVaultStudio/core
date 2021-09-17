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

file(TO_CMAKE_PATH $ENV{HDPS_INSTALL_DIR} INSTALL_DIR)

find_package(Qt5 COMPONENTS Widgets WebEngineWidgets REQUIRED)

file(GLOB JSON src/*.json)
set(AUX ${JSON})

set(IMAGE_DATA_HEADERS 
    src/Image.h
    src/ImageData.h
    src/Images.h
    src/Common.h
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

include_directories("${INSTALL_DIR}/$<CONFIGURATION>/include/")

add_library(${PROJECT} SHARED ${IMAGE_DATA_HEADERS} ${IMAGE_DATA_SOURCES} ${ACTIONS_SOURCES} ${AUX})

set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 14)

include(GenerateExportHeader)
generate_export_header(${PROJECT})

file(GLOB EXPORT_HEADER_FILE_NAME 
    RELATIVE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/*_export.h)
list(APPEND IMAGEDATA_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_HEADER_FILE_NAME})

target_compile_features(${PROJECT} PRIVATE cxx_std_17)

if(MSVC)
    set(LIB_LINK_SUFFIX ".lib") 
else()
    set(LIB_LINK_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif(MSVC)

set(HDPS_LINK_LIBRARY "${INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${LIB_LINK_SUFFIX}")
set(POINTDATA_LINK_LIBRARY "${INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}PointData${LIB_LINK_SUFFIX}") 

target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} Qt5::WebEngineWidgets)
target_link_libraries(${PROJECT} "${HDPS_LINK_LIBRARY}")
target_link_libraries(${PROJECT} "${POINTDATA_LINK_LIBRARY}")

set_target_properties(${PROJECT} PROPERTIES PUBLIC_HEADER "${IMAGE_DATA_HEADERS}")

install(TARGETS ${PROJECT}
    RUNTIME DESTINATION Plugins COMPONENT PLUGINS
    LIBRARY DESTINATION Plugins COMPONENT PLUGINS
    ARCHIVE DESTINATION lib COMPONENT LINKLIB
    PUBLIC_HEADER DESTINATION include/${PROJECT} COMPONENT HEADERS
)

if (NOT WIN32)
    install(TARGETS ${PROJECT}
        LIBRARY DESTINATION lib COMPONENT LINKLIB # Linux/Mac .so for linking
        PUBLIC_HEADER DESTINATION include COMPONENT HEADERS
    )
endif()

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_BINARY_DIR}/HDPS/src/plugins/ImageData
        --config $<CONFIGURATION>
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
)
