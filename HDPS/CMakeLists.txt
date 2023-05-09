cmake_minimum_required(VERSION 3.17)

# -----------------------------------------------------------------------------
# Project: HDPS-core
# -----------------------------------------------------------------------------
set(HDPS_CORE "hdps-core")
project(${HDPS_CORE})
set(version 0.2)

option(USE_AVX "Use AVX if available - by default OFF" OFF)

# These three targets defined in this CMakeLists.txt
set(HDPS_EXE HDPS)                  # The HDPS executable
set(HDPS_PUBLIC_LIB HDPS_Public)    # A public shared library used by plugin builders and the HDPS executable
set(HDPS_PRIVATE_LIB HDPS_Private)  # A private shared library contain code for the HDPS executable

set(LINK_TYPE SHARED)
if(MSVC)
    set(LINK_TYPE STATIC)
endif(MSVC)

# -----------------------------------------------------------------------------
# CMake Settings
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC_PATH_PREFIX ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

# include CMakePackageConfigHelpers macro
include(CMakePackageConfigHelpers)

# Test hardware avx capabilities
include(CMakeCheckSetAVX)

# -----------------------------------------------------------------------------
# Set install directory
# -----------------------------------------------------------------------------
if(DEFINED ENV{HDPS_INSTALL_DIR})
    message(STATUS 
    "***************************************************
    Using environment variable HDPS_INSTALL_DIR 
    $ENV{HDPS_INSTALL_DIR}
    to indicate where the HDPS output will be installed,
    make sure the directory exists.
    ***************************************************" )
endif()

# Get the env path and allow the user to change it
set(INSTALL_DIR $ENV{HDPS_INSTALL_DIR} CACHE PATH "The HDPS Install Directoy")
# Normalize the incoming install path
file(TO_CMAKE_PATH ${INSTALL_DIR} INSTALL_DIR)

if(NOT DEFINED INSTALL_DIR)
    message(FATAL_ERROR "No install path provided! Set the cmake variable INSTALL_DIR or environment variable HDPS_INSTALL_DIR")
else()
    message(STATUS "Install HDPS to ${INSTALL_DIR}")
endif()


# -----------------------------------------------------------------------------
# Apple settings
# -----------------------------------------------------------------------------
if(APPLE)
    # These are the properties for the Info.plist in the mac bindle
    set(MACOSX_BUNDLE_BUNDLE_NAME "HDPS")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2021. All rights reserved.")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.biovault.hdps")
    set(MACOSX_BUNDLE_ICON_FILE AppIcon.icns)

    # use, i.e. don't skip the full RPATH for the build tree
    set(CMAKE_SKIP_BUILD_RPATH  FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

    set(CMAKE_INSTALL_RPATH "@loader_path")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()


# -----------------------------------------------------------------------------
# Other Unix settings
# -----------------------------------------------------------------------------
if(UNIX AND NOT APPLE)
    # use, i.e. don't skip the full RPATH for the build tree
    set(CMAKE_SKIP_BUILD_RPATH  FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

    set(CMAKE_INSTALL_RPATH "${INSTALL_DIR}/$<CONFIGURATION>/lib")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()


# -----------------------------------------------------------------------------
# Windows compiler and linker flags
# -----------------------------------------------------------------------------
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

find_package(Qt6 6.3 COMPONENTS Core Widgets OpenGL OpenGLWidgets WebEngineWidgets REQUIRED)

add_definitions(-DQT_MESSAGELOGCONTEXT)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

# Qt Advanced docking system, set version manually
set(ADS_VERSION 4.0.1)
set(BUILD_EXAMPLES OFF CACHE BOOL "Qt-ads examples")
add_subdirectory(external/advanced_docking)

# File compression LIBRARY
if (WIN32)
    if(NOT DEFINED ZLIB_ROOT)
        message(FATAL_ERROR "You need to define ZLIB_ROOT, pointing to a zlib installation")
    else()
        message(STATUS "Using zlib at ${ZLIB_ROOT}")
    endif()

elseif (APPLE)
    message(STATUS "********Using brew zlib for QUAZIP *********")

    find_program(BREW_BIN brew)
    execute_process(COMMAND ${BREW_BIN} install pkg-config) 

else()
    find_package(ZLIB REQUIRED)
    message(STATUS "Using zlib at ${ZLIB_INCLUDE_DIRS}")
endif()

set(QUAZIP_QT_MAJOR_VERSION 6)
set(QUAZIP_BZIP2 OFF CACHE BOOL "Enables BZIP2 compression")  # we use ZLIB, not BZIP
add_subdirectory(external/quazip)


# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------
include(CMakeHdpsSourcesMain)           # defines MAIN_SOURCES
include(CMakeHdpsSourcesPublic)         # defines PUBLIC_SOURCES and PUBLIC_HEADERS
include(CMakeHdpsSourcesPrivate)        # defines PRIVATE_SOURCES
include(CMakeHdpsSourcesResources)      # defines RESOURCE_FILES and UI_HEADERS


# -----------------------------------------------------------------------------
# Target HDPS_PUBLIC_LIB
# -----------------------------------------------------------------------------
add_library(${HDPS_PUBLIC_LIB} ${LINK_TYPE}
    src/.editorconfig
    ${PUBLIC_SOURCES}
)

target_include_directories(${HDPS_PUBLIC_LIB} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(${HDPS_PUBLIC_LIB} PRIVATE cxx_std_17)

target_link_libraries(${HDPS_PUBLIC_LIB} PRIVATE Qt6::Widgets)
target_link_libraries(${HDPS_PUBLIC_LIB} PRIVATE Qt6::WebEngineWidgets)

# Use avx if enabled and available
check_and_set_AVX(${HDPS_PUBLIC_LIB} ${USE_AVX})

# -----------------------------------------------------------------------------
# Target HDPS_PRIVATE_LIB
# -----------------------------------------------------------------------------
add_library(${HDPS_PRIVATE_LIB} ${LINK_TYPE}
    src/.editorconfig
    ${PRIVATE_SOURCES}
    ${RESOURCE_FILES}
)

target_include_directories(${HDPS_PRIVATE_LIB} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(${HDPS_PRIVATE_LIB} PRIVATE cxx_std_17)

target_link_libraries(${HDPS_PRIVATE_LIB} PRIVATE Qt6::Widgets)
target_link_libraries(${HDPS_PRIVATE_LIB} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${HDPS_PRIVATE_LIB} PRIVATE Qt6::OpenGL)
target_link_libraries(${HDPS_PRIVATE_LIB} PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(${HDPS_PRIVATE_LIB} PRIVATE qtadvanceddocking)
target_link_libraries(${HDPS_PRIVATE_LIB} PRIVATE QuaZip)
target_link_libraries(${HDPS_PRIVATE_LIB} PRIVATE "$<TARGET_FILE:${HDPS_PUBLIC_LIB}>")

# Use avx if enabled and available
check_and_set_AVX(${HDPS_PRIVATE_LIB} ${USE_AVX})

add_dependencies(${HDPS_PRIVATE_LIB} ${HDPS_PUBLIC_LIB} QuaZip)

# -----------------------------------------------------------------------------
# Target HDPS_EXE
# -----------------------------------------------------------------------------
add_executable(${HDPS_EXE} MACOSX_BUNDLE WIN32
    src/.editorconfig
    ${MAIN_SOURCES}
    ${UI_HEADERS}
    ${RESOURCE_FILES}
)

target_compile_features(${HDPS_EXE} PRIVATE cxx_std_17)

# find libraries relative to the executable in the macOS .app tree
if(APPLE)
    set_target_properties( ${HDPS_EXE} PROPERTIES BUILD_WITH_INSTALL_RPATH True)
endif()

target_include_directories(${HDPS_EXE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${HDPS_EXE} PRIVATE Qt6::Widgets)
target_link_libraries(${HDPS_EXE} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${HDPS_EXE} PRIVATE Qt6::OpenGL)
target_link_libraries(${HDPS_EXE} PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(${HDPS_EXE} PRIVATE qtadvanceddocking)
target_link_libraries(${HDPS_EXE} PRIVATE QuaZip)

target_link_libraries(${HDPS_EXE} PRIVATE "$<TARGET_FILE:${HDPS_PUBLIC_LIB}>")
target_link_libraries(${HDPS_EXE} PRIVATE "$<TARGET_FILE:${HDPS_PRIVATE_LIB}>")

# Use avx if enabled and available
check_and_set_AVX(${HDPS_EXE} ${USE_AVX})

add_dependencies(${HDPS_EXE} ${HDPS_PUBLIC_LIB} ${HDPS_PRIVATE_LIB} QuaZip)

if(MSVC)
    set_target_properties(${HDPS_EXE} PROPERTIES
        WIN32_EXECUTABLE YES
        LINK_FLAGS "/ENTRY:mainCRTStartup"
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif(MSVC)


# -----------------------------------------------------------------------------
# Installation
# -----------------------------------------------------------------------------

# Install the HDPS executable 
if(APPLE)
    set(BUNDLE_DIR "${INSTALL_DIR}/$<CONFIGURATION>/${MACOSX_BUNDLE_BUNDLE_NAME}.app")
    set(WEBENGINEINSTALL_CMD "${CMAKE_CURRENT_SOURCE_DIR}/macos_webengineinstall.sh")
    install(TARGETS ${HDPS_EXE}
        RUNTIME DESTINATION bin COMPONENT MACOS_BUNDLE
        BUNDLE DESTINATION . COMPONENT MACOS_BUNDLE
    )
    # -always-overwrite
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
    install(CODE "execute_process(
            COMMAND ${MACDEPLOYQT_EXECUTABLE} \"${BUNDLE_DIR}\" -always-overwrite
        #	RESULT_VARIABLE retval
            COMMAND ${CMAKE_COMMAND} -E make_directory \"${BUNDLE_DIR}/Contents/Frameworks\"
            COMMAND ${CMAKE_COMMAND} -E copy 
                    \"${INSTALL_DIR}/$<CONFIGURATION>/lib/libqtadvanceddocking.${ADS_VERSION}.dylib\" 
                    \"${BUNDLE_DIR}/Contents/Frameworks/\"
            COMMAND ${CMAKE_COMMAND} -E copy 
                    \"$<TARGET_FILE:${HDPS_PUBLIC_LIB}>\" 
                    \"${BUNDLE_DIR}/Contents/Frameworks/\"
                    
            COMMAND ${CMAKE_COMMAND} -E copy 
                    \"$<TARGET_FILE:${HDPS_PRIVATE_LIB}>\" 
                    \"${BUNDLE_DIR}/Contents/Frameworks/\"
        )
        
        #if(NOT retval EQUAL 0)
        #	message(FATAL_ERROR \"macdeployqt failed\")
        #endif()
        "
        COMPONENT MACOS_BUNDLE
    )
    install(CODE "execute_process(
            COMMAND bash ${WEBENGINEINSTALL_CMD} ${Qt6_DIR} ${BUNDLE_DIR}
        )"
        COMPONENT MACOS_BUNDLE
    )
elseif(UNIX)
    install(TARGETS ${HDPS_EXE}
        RUNTIME DESTINATION . COMPONENT HDPS_EXECUTABLE
    )
    
    install(TARGETS ${HDPS_PRIVATE_LIB}
        LIBRARY DESTINATION lib COMPONENT HDPSPRIVATE_LINKLIB # Linux .so for linking
    )
else()
    install(TARGETS ${HDPS_EXE}
        RUNTIME DESTINATION . COMPONENT HDPS_EXECUTABLE
    )
    # On MSVC, HDPS_PRIVATE_LIB is statically linked to HDPS_EXE and does not need to be installed
endif()

# Install public headers 
install(CODE "foreach(PUBLIC_HEADER ${PUBLIC_HEADERS})
        string(REGEX REPLACE \"^src/\" \"\" STRIPPED_PUBLIC_HEADER \${PUBLIC_HEADER})
        execute_process(
            COMMAND \"${CMAKE_COMMAND}\" -E copy_if_different 
                \"${CMAKE_CURRENT_SOURCE_DIR}/\${PUBLIC_HEADER}\" 
                \"${INSTALL_DIR}/$<CONFIGURATION>/include/\${STRIPPED_PUBLIC_HEADER}\"
        )
    MESSAGE(STATUS \"Installing: \${PUBLIC_HEADER}\")
    endforeach(PUBLIC_HEADER)"
    COMPONENT HDPSPUBLIC_HEADERS
)

# Install public headers 
install(TARGETS ${HDPS_PUBLIC_LIB}
    EXPORT HdpsPublicExport
    ARCHIVE DESTINATION lib COMPONENT HDPSPUBLIC_LINKLIB  # Linux/Mac .so
    LIBRARY DESTINATION lib COMPONENT HDPSPUBLIC_LINKLIB  # Windows .lib
    INCLUDES DESTINATION include
)

# Generate a .cmake file that can be imported from the install
# to enable plugin building
# Plugin building requires HDPS_Public.lib and the include headers
# By convention this is placed in the lib directory
install(EXPORT HdpsPublicExport
        FILE HdpsCoreTargets.cmake
        NAMESPACE HdpsCore::
        DESTINATION lib/cmake/HdpsCore
        COMPONENT HDPSPUBLIC_TARGET
)

# ***Create a HdpsCore package to support the find_package command**

# Generate the version file for use with find_package
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/HdpsCoreConfigVersion.cmake"
  VERSION "${version}"
  # When the hdps core is stable - version 1.0 or greater -
  # compatibility can change to
  # AnyNewerVersion or SameMajorVersion
  # until then employ ExactVersion
  COMPATIBILITY ExactVersion
)

# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/HdpsCoreConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/HdpsCoreConfig.cmake"
  INSTALL_DESTINATION lib/cmake/HdpsCore
)

# install the package files
install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/HdpsCoreConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/HdpsCoreConfigVersion.cmake"
        DESTINATION lib/cmake/HdpsCore
        COMPONENT HDPSPUBLIC_PACKAGE
)

# could be used for the plugins subdirectories 
# see doc https://cmake.org/cmake/help/v3.18/command/export.html
#
# export(EXPORT HdpsCoreTargets
#        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/HdpsCoreTargets.cmake"
#        NAMESPACE HdpsCore::
# )

# Call the install functions of the all public targets
# needed for plugins directly after the build
add_custom_command(TARGET ${HDPS_PUBLIC_LIB} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E echo "Installing: ManiVault link library"
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component HDPSPUBLIC_HEADERS
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component HDPSPUBLIC_LINKLIB
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component HDPSPUBLIC_PACKAGE
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    COMMAND "${CMAKE_COMMAND}"
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component HDPSPUBLIC_TARGET
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    # If no plugin directory exsists, ManiVault might try to import 
    # any library in the executable directory as a plugin
    COMMAND "${CMAKE_COMMAND}"
        -E make_directory ${INSTALL_DIR}/$<CONFIGURATION>/Plugins
)

# Install the HDPS executable
if(APPLE)
    add_custom_command(TARGET ${HDPS_EXE} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E echo "Installing: ManiVault executable (bundle)"
        COMMAND "${CMAKE_COMMAND}"
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION>
            --component MACOS_BUNDLE
            --prefix ${INSTALL_DIR}/$<CONFIGURATION>/
    )
elseif(UNIX)
    add_custom_command(TARGET ${HDPS_PRIVATE_LIB} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E echo "Installing: ManiVault link library (private)"
        COMMAND "${CMAKE_COMMAND}"
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION>
            --component HDPSPRIVATE_LINKLIB
            --prefix ${INSTALL_DIR}/$<CONFIGURATION>
            --verbose
    )
endif()

if(NOT APPLE)
    add_custom_command(TARGET ${HDPS_EXE} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E echo "Installing: ManiVault executable"
        COMMAND "${CMAKE_COMMAND}"
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION>
            --component HDPS_EXECUTABLE
            --prefix ${INSTALL_DIR}/$<CONFIGURATION>
            --verbose
    )
endif()

# Install examples
add_custom_command(TARGET ${HDPS_EXE}  POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E echo "Installing: ManiVault examples"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/res/examples
            ${INSTALL_DIR}/$<CONFIGURATION>/examples
)

# Install dependency runtime libraries
add_custom_command(TARGET ${HDPS_EXE} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E echo "Installing: dependency runtime libraries"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:qtadvanceddocking>" "${INSTALL_DIR}$<IF:$<CXX_COMPILER_ID:MSVC>,,/bin>/$<CONFIGURATION>"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:QuaZip>" "${INSTALL_DIR}$<IF:$<CXX_COMPILER_ID:MSVC>,,/bin>/$<CONFIGURATION>"
)

# Install dependency licences
add_custom_command(TARGET ${HDPS_EXE} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E echo "Installing: dependency licences"
    COMMAND "${CMAKE_COMMAND}" 
        --install ${CMAKE_CURRENT_BINARY_DIR}/external/advanced_docking
        --config $<CONFIGURATION> 
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>/
        --component license
)

# -----------------------------------------------------------------------------
# Plugin projects
# -----------------------------------------------------------------------------

# System plugins
add_subdirectory(src/plugins/DataHierarchyPlugin)
add_subdirectory(src/plugins/DataPropertiesPlugin)
add_subdirectory(src/plugins/ActionsPlugin)
add_subdirectory(src/plugins/LoggingPlugin)

# Data plugins
add_subdirectory(src/plugins/PointData)
add_subdirectory(src/plugins/ColorData)
add_subdirectory(src/plugins/TextData)
add_subdirectory(src/plugins/ClusterData)
add_subdirectory(src/plugins/ImageData)


# -----------------------------------------------------------------------------
# Miscellaneous
# -----------------------------------------------------------------------------

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${HDPS_EXE} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY  $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug,${INSTALL_DIR}/release>)
    set_property(TARGET ${HDPS_EXE} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug/${HDPS_EXE}.exe,${INSTALL_DIR}/release/${HDPS_EXE}.exe>)
endif()
