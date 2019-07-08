cmake_minimum_required(VERSION 3.1.0)

set(PROJECT "TsneAnalysisPlugin")

PROJECT(${PROJECT})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

find_package(Qt5 COMPONENTS Widgets WebEngineWidgets REQUIRED)

set(SOURCES
    src/TsneAnalysisPlugin.h
    src/TsneAnalysisPlugin.cpp
    src/TsneAnalysisPlugin.json
    src/TsneAnalysis.h
    src/TsneAnalysis.cpp
    src/TsneSettingsWidget.h
    src/TsneSettingsWidget.cpp
    src/TsneData.h
)

source_group( Plugin FILES ${SOURCES})

include_directories("$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/include/")
include_directories ("lib/HDI/include")

if(MSVC)
    include_directories ("lib/Flann/Win/include")
endif(MSVC)

if(APPLE)
    include_directories ("lib/Flann/OSX/include")
endif(APPLE)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    include_directories ("lib/Flann/Linux/include")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_library(${PROJECT} SHARED ${SOURCES})

target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} Qt5::WebEngineWidgets)
target_link_libraries(${PROJECT} "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/HDPS_Public.lib")

if(MSVC)
    MESSAGE( STATUS "Linking Windows libraries...")
    target_link_libraries(${PROJECT} debug "${CMAKE_CURRENT_SOURCE_DIR}/lib/HDI/Win/Debug/hdidimensionalityreduction.lib")
    target_link_libraries(${PROJECT} debug "${CMAKE_CURRENT_SOURCE_DIR}/lib/HDI/Win/Debug/hdidata.lib")
    target_link_libraries(${PROJECT} debug "${CMAKE_CURRENT_SOURCE_DIR}/lib/HDI/Win/Debug/hdiutils.lib")
    target_link_libraries(${PROJECT} optimized "${CMAKE_CURRENT_SOURCE_DIR}/lib/HDI/Win/Release/hdidimensionalityreduction.lib")
    target_link_libraries(${PROJECT} optimized "${CMAKE_CURRENT_SOURCE_DIR}/lib/HDI/Win/Release/hdidata.lib")
    target_link_libraries(${PROJECT} optimized "${CMAKE_CURRENT_SOURCE_DIR}/lib/HDI/Win/Release/hdiutils.lib")

    target_link_libraries(${PROJECT} debug "${CMAKE_CURRENT_SOURCE_DIR}/lib/Flann/Win/Debug/flann_cpp_s.lib")
    target_link_libraries(${PROJECT} optimized "${CMAKE_CURRENT_SOURCE_DIR}/lib/Flann/Win/Release/flann_cpp_s.lib")
endif(MSVC)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    MESSAGE( STATUS "Linking Linux libraries...")
    target_link_libraries(${PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/lib/AtSNE/Linux/libbh_t_sne_library.a")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(APPLE)
    MESSAGE( STATUS "Linking Mac OS X libraries...")

    target_link_libraries(${PROJECT} debug "${CMAKE_CURRENT_SOURCE_DIR}/lib/AtSNE/OSX/Debug/libbh_t_sne_library.a")
    target_link_libraries(${PROJECT} optimized "${CMAKE_CURRENT_SOURCE_DIR}/lib/AtSNE/OSX/Release/libbh_t_sne_library.a")

    target_link_libraries(${PROJECT} "${CMAKE_CURRENT_SOURCE_DIR}/lib/Flann/OSX/libflann_s.a")
endif(APPLE)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "$<TARGET_FILE:${PROJECT}>"
    "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/Plugins/$<TARGET_FILE_NAME:${PROJECT}>"
)