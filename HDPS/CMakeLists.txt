cmake_minimum_required(VERSION 3.17)

# -----------------------------------------------------------------------------
# Project: ManiVault-core
# -----------------------------------------------------------------------------
set(MV_CORE "mv-core")
project(${MV_CORE})
set(version 0.2)

# These three targets defined in this CMakeLists.txt
set(MV_APPLICATION_NAME "ManiVault Studio")  # Name of the executable
set(MV_EXE MV_Application)                   # The executable target
set(MV_PUBLIC_LIB Core)                      # A public shared library used by plugin builders and the ManiVault executable
set(MV_PRIVATE_LIB MV_Private)               # A private shared library contain code for the ManiVault executable

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
set(CMAKE_INSTALL_MESSAGE LAZY)
set(CMAKE_AUTOMOC_PATH_PREFIX ON)
set(CMAKE_AUTOMOC ON)

# include CMakePackageConfigHelpers macro
include(CMakePackageConfigHelpers)

# Test hardware avx capabilities
include(CMakeCheckSetAVX)

# -----------------------------------------------------------------------------
# Set install directory
# -----------------------------------------------------------------------------
# Normalize the incoming install path
file(TO_CMAKE_PATH ${INSTALL_DIR} INSTALL_DIR)

if(DEFINED ENV{HDPS_INSTALL_DIR})
    set(ENV_HDPS_INSTALL_DIR "$ENV{HDPS_INSTALL_DIR}")
    file(TO_CMAKE_PATH ${ENV_HDPS_INSTALL_DIR} ENV_HDPS_INSTALL_DIR)

    if ("${INSTALL_DIR}" STREQUAL "${ENV_HDPS_INSTALL_DIR}")
        set(messageText 
        "***************************************************
        Using environment variable HDPS_INSTALL_DIR 
        $ENV{HDPS_INSTALL_DIR}
        to indicate where the ManiVault output will be installed,
        make sure the directory exists.
        ***************************************************")
        string(REPLACE "\n        " "\n" messageText "${messageText}")
        message(STATUS "${messageText}")
    endif()

endif()

if(NOT DEFINED INSTALL_DIR)
    message(FATAL_ERROR "No install path provided! Set the cmake variable INSTALL_DIR or environment variable HDPS_INSTALL_DIR")
else()
    message(STATUS "Install ManiVault to ${INSTALL_DIR}")
endif()


# -----------------------------------------------------------------------------
# Apple settings
# -----------------------------------------------------------------------------
if(APPLE)
    # These are the properties for the Info.plist in the mac bindle
    set(MACOSX_BUNDLE_BUNDLE_NAME "ManiVault Studio")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1.0")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright (c) 2023. All rights reserved.")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.biovault.mv")
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
include(CMakeMvSourcesMain)           # defines MAIN_SOURCES
include(CMakeMvSourcesPublic)         # defines PUBLIC_SOURCES and PUBLIC_HEADERS
include(CMakeMvSourcesPrivate)        # defines PRIVATE_SOURCES and PRIVATE_HEADERS
include(CMakeMvSourcesResources)      # defines RESOURCE_FILES and UI_HEADERS


# -----------------------------------------------------------------------------
# Target MV_PUBLIC_LIB
# -----------------------------------------------------------------------------
add_library(${MV_PUBLIC_LIB} ${LINK_TYPE}
    src/.editorconfig
    ${PUBLIC_SOURCES}
)

# Set the project name in VS different to the target name
set_target_properties(${MV_PUBLIC_LIB} PROPERTIES PROJECT_LABEL "MV_Public")
# Set the library output name different to the target name
set_target_properties(${MV_PUBLIC_LIB} PROPERTIES OUTPUT_NAME "MV_Public")

target_include_directories(${MV_PUBLIC_LIB} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(${MV_PUBLIC_LIB} PRIVATE cxx_std_17)

target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::Widgets)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::WebEngineWidgets)

# Use avx if enabled and available
check_and_set_AVX(${MV_PUBLIC_LIB} ${MV_USE_AVX})

# -----------------------------------------------------------------------------
# Target MV_PRIVATE_LIB
# -----------------------------------------------------------------------------
add_library(${MV_PRIVATE_LIB} ${LINK_TYPE}
    src/.editorconfig
    ${PRIVATE_SOURCES}
    ${RESOURCE_FILES}
)

target_include_directories(${MV_PRIVATE_LIB} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(${MV_PRIVATE_LIB} PRIVATE cxx_std_17)

target_link_libraries(${MV_PRIVATE_LIB} PRIVATE Qt6::Widgets)
target_link_libraries(${MV_PRIVATE_LIB} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${MV_PRIVATE_LIB} PRIVATE Qt6::OpenGL)
target_link_libraries(${MV_PRIVATE_LIB} PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(${MV_PRIVATE_LIB} PRIVATE qtadvanceddocking)
target_link_libraries(${MV_PRIVATE_LIB} PRIVATE QuaZip)
target_link_libraries(${MV_PRIVATE_LIB} PRIVATE ${MV_PUBLIC_LIB})

# Use avx if enabled and available
check_and_set_AVX(${MV_PRIVATE_LIB} ${MV_USE_AVX})

add_dependencies(${MV_PRIVATE_LIB} ${MV_PUBLIC_LIB} QuaZip qtadvanceddocking)

# -----------------------------------------------------------------------------
# Target MV_EXE
# -----------------------------------------------------------------------------
add_executable(${MV_EXE} MACOSX_BUNDLE WIN32
    src/.editorconfig
    ${MAIN_SOURCES}
    ${UI_HEADERS}
    ${RESOURCE_FILES}
)

set_target_properties(${MV_EXE} PROPERTIES OUTPUT_NAME ${MV_APPLICATION_NAME})
target_compile_features(${MV_EXE} PRIVATE cxx_std_17)

# find libraries relative to the executable in the macOS .app tree
if(APPLE)
    set_target_properties( ${MV_EXE} PROPERTIES BUILD_WITH_INSTALL_RPATH True)
endif()

target_include_directories(${MV_EXE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(${MV_EXE} PRIVATE Qt6::Widgets)
target_link_libraries(${MV_EXE} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${MV_EXE} PRIVATE Qt6::OpenGL)
target_link_libraries(${MV_EXE} PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(${MV_EXE} PRIVATE qtadvanceddocking)
target_link_libraries(${MV_EXE} PRIVATE QuaZip)

target_link_libraries(${MV_EXE} PRIVATE ${MV_PUBLIC_LIB})
target_link_libraries(${MV_EXE} PRIVATE ${MV_PRIVATE_LIB})

# Use avx if enabled and available
check_and_set_AVX(${MV_EXE} ${MV_USE_AVX})

add_dependencies(${MV_EXE} ${MV_PUBLIC_LIB} ${MV_PRIVATE_LIB} QuaZip qtadvanceddocking)

if(MSVC)
    set_target_properties(${MV_EXE} PROPERTIES
        WIN32_EXECUTABLE YES
        LINK_FLAGS "/ENTRY:mainCRTStartup"
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif(MSVC)


# -----------------------------------------------------------------------------
# Installation
# -----------------------------------------------------------------------------

# Install the ManiVault executable 
if(APPLE)
    set(BUNDLE_DIR "${INSTALL_DIR}/$<CONFIGURATION>/${MACOSX_BUNDLE_BUNDLE_NAME}.app")
    set(WEBENGINEINSTALL_CMD "${CMAKE_CURRENT_SOURCE_DIR}/macos_webengineinstall.sh")
    install(TARGETS ${MV_EXE}
        RUNTIME DESTINATION bin COMPONENT MACOS_BUNDLE
        BUNDLE DESTINATION . COMPONENT MACOS_BUNDLE
    )
    # -always-overwrite
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
    install(CODE "execute_process(
            COMMAND ${MACDEPLOYQT_EXECUTABLE} \"${BUNDLE_DIR}\" -always-overwrite
        #	RESULT_VARIABLE retval
            COMMAND ${CMAKE_COMMAND} -E make_directory \"${BUNDLE_DIR}/Contents/Frameworks\"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                    \"${INSTALL_DIR}/$<CONFIGURATION>/lib/libqtadvanceddocking.${ADS_VERSION}.dylib\" 
                    \"${BUNDLE_DIR}/Contents/Frameworks/\"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                    \"$<TARGET_FILE:${MV_PUBLIC_LIB}>\" 
                    \"${BUNDLE_DIR}/Contents/Frameworks/\"
                    
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                    \"$<TARGET_FILE:${MV_PRIVATE_LIB}>\" 
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
    install(TARGETS ${MV_EXE}
        RUNTIME DESTINATION . COMPONENT MV_EXECUTABLE
    )
    
    install(TARGETS ${MV_PRIVATE_LIB}
        LIBRARY DESTINATION lib COMPONENT MVPRIVATE_LINKLIB # Linux .so for linking
    )
else()
    install(TARGETS ${MV_EXE}
        RUNTIME DESTINATION . COMPONENT MV_EXECUTABLE
    )
    # On MSVC, MV_PRIVATE_LIB is statically linked to MV_EXE and does not need to be installed
endif()

# Install public headers
foreach(PUBLIC_HEADER ${PUBLIC_HEADERS})
    # Remove the "src" prefix of the header get the remaining subdirectory
    string(REGEX REPLACE "^src/(.*/).*" "\\1" STRIPPED_PUBLIC_HEADER ${PUBLIC_HEADER})
    if("${STRIPPED_PUBLIC_HEADER}" STREQUAL "${PUBLIC_HEADER}")
        set(STRIPPED_PUBLIC_HEADER "")
    endif()
    set(HEADER_DEST "${INSTALL_DIR}/$<CONFIGURATION>/include/${STRIPPED_PUBLIC_HEADER}")

    # Install the header file while preserving the folder structure
    install(FILES ${PUBLIC_HEADER} DESTINATION ${HEADER_DEST} COMPONENT MVPUBLIC_HEADERS)
endforeach()

# Install public library
install(TARGETS ${MV_PUBLIC_LIB}
    EXPORT ManiVaultPublicExport
    ARCHIVE DESTINATION lib COMPONENT MVPUBLIC_LINKLIB  # Linux/Mac .so
    LIBRARY DESTINATION lib COMPONENT MVPUBLIC_LINKLIB  # Windows .lib
    INCLUDES DESTINATION include
)

# Generate a .cmake file that can be imported from the install
# to enable plugin building
# Plugin building requires MV_Public.lib and the include headers
# By convention this is placed in the lib directory
install(EXPORT ManiVaultPublicExport
        FILE ManiVaultTargets.cmake
        NAMESPACE ManiVault::
        DESTINATION lib/cmake/mv
        COMPONENT MVPUBLIC_TARGET
)

# ***Create a HdpsCore package to support the find_package command**

# Generate the version file for use with find_package
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ManiVaultConfigVersion.cmake"
  VERSION "${version}"
  # When the hdps core is stable - version 1.0 or greater -
  # compatibility can change to
  # AnyNewerVersion or SameMajorVersion
  # until then employ ExactVersion
  COMPATIBILITY ExactVersion
)

# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/MvCoreConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/ManiVaultConfig.cmake"
  INSTALL_DESTINATION lib/cmake/mv
)

# install the package files
install(FILES
          "${CMAKE_CURRENT_BINARY_DIR}/ManiVaultConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/ManiVaultConfigVersion.cmake"
        DESTINATION lib/cmake/mv
        COMPONENT MVPUBLIC_PACKAGE
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
add_custom_command(TARGET ${MV_PUBLIC_LIB} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault public headers"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_HEADERS
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault public link library"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_LINKLIB
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault cmake config files"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_PACKAGE
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault cmake target files"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_TARGET
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    # If no plugin directory exsists, ManiVault might try to import 
    # any library in the executable directory as a plugin
    COMMAND ${CMAKE_COMMAND}
        -E make_directory ${INSTALL_DIR}/$<CONFIGURATION>/Plugins
)

# Install the ManiVault executable
if(APPLE)
    add_custom_command(TARGET ${MV_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault executable bundle"
        COMMAND ${CMAKE_COMMAND}
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION>
            --component MACOS_BUNDLE
            --prefix ${INSTALL_DIR}/$<CONFIGURATION>/
    )
elseif(UNIX)
    add_custom_command(TARGET ${MV_PRIVATE_LIB} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault private link library"
        COMMAND ${CMAKE_COMMAND}
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION>
            --component MVPRIVATE_LINKLIB
            --prefix ${INSTALL_DIR}/$<CONFIGURATION>
            --verbose
    )
endif()

if(NOT APPLE)
    add_custom_command(TARGET ${MV_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault executable"
        COMMAND ${CMAKE_COMMAND}
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION>
            --component MV_EXECUTABLE
            --prefix ${INSTALL_DIR}/$<CONFIGURATION>
            --verbose
    )
endif()

# Install ManiVault licences
add_custom_command(TARGET ${MV_EXE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault licences"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INSTALL_DIR}/$<CONFIGURATION>/license/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE ${INSTALL_DIR}/$<CONFIGURATION>/license/LICENSE
)

# Install examples
add_custom_command(TARGET ${MV_EXE}  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault examples"
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<VERSION_LESS:${CMAKE_VERSION},3.26.0>,copy_directory,copy_directory_if_different>  # copy_directory_if_different is only available starting 3.26
            ${CMAKE_CURRENT_SOURCE_DIR}/res/examples
            ${INSTALL_DIR}/$<CONFIGURATION>/examples
)

# Install dependency runtime libraries
# on windows, the DLLs should be placed in the same directory as the executable
add_custom_command(TARGET ${MV_EXE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: dependency runtime libraries"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:qtadvanceddocking> ${INSTALL_DIR}/$<CONFIGURATION>$<IF:$<CXX_COMPILER_ID:MSVC>,,/lib/>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:QuaZip> ${INSTALL_DIR}/$<CONFIGURATION>$<IF:$<CXX_COMPILER_ID:MSVC>,,/lib/>
)

if(UNIX AND NOT APPLE)
    # Creat symlink with slightly different name required on some linux distributions
    include(CMakeQuaZipSymlink)
    create_quazip_symlink()
endif()

# Install dependency licences
add_custom_command(TARGET ${MV_EXE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: dependency licences"
    COMMAND ${CMAKE_COMMAND} 
        --install ${CMAKE_CURRENT_BINARY_DIR}/external/advanced_docking
        --config $<CONFIGURATION> 
        --prefix ${INSTALL_DIR}/$<CONFIGURATION>/
        --component license
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INSTALL_DIR}/$<CONFIGURATION>/license/quazip
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/external/quazip/COPYING ${INSTALL_DIR}/$<CONFIGURATION>/license/quazip/LICENSE
)

# -----------------------------------------------------------------------------
# Plugin projects
# -----------------------------------------------------------------------------

# System plugins
add_subdirectory(src/plugins/DataHierarchyPlugin)
add_subdirectory(src/plugins/DataPropertiesPlugin)
add_subdirectory(src/plugins/SharedParametersPlugin)
add_subdirectory(src/plugins/LoggingPlugin)
add_subdirectory(src/plugins/TasksPlugin)

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
    set_property(TARGET ${MV_EXE} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY  $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug,${INSTALL_DIR}/release>)
    set_property(TARGET ${MV_EXE} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${INSTALL_DIR}/debug/${MV_APPLICATION_NAME}.exe,${INSTALL_DIR}/release/${MV_APPLICATION_NAME}.exe>)
endif()
