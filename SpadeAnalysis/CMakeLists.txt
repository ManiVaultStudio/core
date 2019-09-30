cmake_minimum_required(VERSION 3.1.0)

set(PROJECT "SpadeAnalysisPlugin")

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
find_package(OpenMP)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_program(BREW NAMES brew)
    message(STATUS, "In Darwin with ${BREW}")
    execute_process(COMMAND ${BREW} --prefix libomp OUTPUT_VARIABLE BREW_LIBOMP_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS, "BREW LIBOMP ${BREW_LIBOMP_PREFIX}")
    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY "${BREW_LIBOMP_PREFIX}/lib/libomp.dylib")
    include_directories("${BREW_LIBOMP_PREFIX}/include")
endif()


set(SOURCES
    src/SpadeAnalysisPlugin.h
    src/SpadeAnalysisPlugin.cpp
    src/SpadeAnalysisPlugin.json
    src/SpadeSettingsWidget.h
    src/SpadeSettingsWidget.cpp
)

set( EXTERNAL_SOURCES
    external/hcluster/hcluster.h
    external/hcluster/hcluster.cpp
    external/boruvka/boruvka.h
    external/boruvka/boruvka.cpp
)

source_group( Plugin FILES ${SOURCES})
source_group( External FILES ${EXTERNAL_SOURCES})

include_directories("$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/include/")

add_library(${PROJECT} SHARED ${SOURCES} ${EXTERNAL_SOURCES})

if(MSVC)
    set(LIB_LINK_SUFFIX ".lib") 
else()
    set(LIB_LINK_SUFFIX "${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif(MSVC)
set(HDPS_LINK_LIBRARY "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}HDPS_Public${LIB_LINK_SUFFIX}")
set(POINTSPLUGIN_LINK_LIBRARY "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/lib/${CMAKE_SHARED_LIBRARY_PREFIX}PointsPlugin${LIB_LINK_SUFFIX}")    

target_link_libraries(${PROJECT} Qt5::Widgets)
target_link_libraries(${PROJECT} Qt5::WebEngineWidgets)
target_link_libraries(${PROJECT} "${HDPS_LINK_LIBRARY}")
target_link_libraries(${PROJECT} "${POINTSPLUGIN_LINK_LIBRARY}")
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(${PROJECT} "$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/Plugins/libClustersPlugin.dylib")
endif()
if(OpenMP_CXX_FOUND)
    message(STATUS, "OpenMP found")
    target_link_libraries(${PROJECT} OpenMP::OpenMP_CXX)
elseif(OpenMP_omp_LIBRARY)
    message(STATUS, "Brew OpenMP found")
    target_link_libraries(${PROJECT} ${OpenMP_omp_LIBRARY})
else()
    message(WARNING, "OpenMP not found!")    
endif()

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
	"$<TARGET_FILE:${PROJECT}>"
	"$ENV{HDPS_INSTALL_DIR}/$<CONFIGURATION>/Plugins/$<TARGET_FILE_NAME:${PROJECT}>"
)