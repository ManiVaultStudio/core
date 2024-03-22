cmake_minimum_required(VERSION 3.17)

# -----------------------------------------------------------------------------
# Project: ManiVault-core
# -----------------------------------------------------------------------------
set(MV_CORE "mv-core")

set(MV_VERSION_MAJOR 1)
set(MV_VERSION_MINOR 0)
set(MV_VERSION_PATCH 0)
set(MV_VERSION_SUFFIX "rc") # e.g. for release candidates rc1

include(cmake/Utils.cmake)
mv_set_version()

project(${MV_CORE} VERSION ${MV_VERSION})

# These three targets defined in this CMakeLists.txt
set(MV_APPLICATION_NAME "ManiVault Studio")  # Name of the executable
set(MV_EXE MV_Application)                   # The ManiVault executable target
set(MV_PUBLIC_LIB Core)                      # A public shared library used by plugin builders and the ManiVault executable

message(STATUS "ManiVault version ${MV_VERSION_FULL}")
message(STATUS "ManiVault Install dir: ${MV_INSTALL_DIR}")

# -----------------------------------------------------------------------------
# CMake Settings
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INSTALL_MESSAGE LAZY)

# include CMakePackageConfigHelpers macro
include(CMakePackageConfigHelpers)

# Test hardware avx capabilities
include(CMakeCheckSetAVX)

# Generate a header file that contains the EXPORT macro for public libraries
include(GenerateExportHeader)

# create a file that defines MV_VERSION_MAJOR, etc
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ManiVaultVersion.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/ManiVaultVersion.h" @ONLY)

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

    set(CMAKE_INSTALL_RPATH "${MV_INSTALL_DIR}/$<CONFIGURATION>/lib")

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

# Qt
find_package(Qt6 6.3 COMPONENTS Core Widgets OpenGL OpenGLWidgets WebEngineWidgets REQUIRED)

add_definitions(-DQT_MESSAGELOGCONTEXT)
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x050F00)

message(STATUS "Using Qt version ${Qt6Widgets_VERSION}")

# Qt Advanced docking system
set(ADS_VERSION 4.2.1)
message(STATUS "Using Qt Advanced Docking System version ${ADS_VERSION}")
set(BUILD_EXAMPLES OFF CACHE BOOL "Qt-ads examples")
add_subdirectory(external/advanced_docking)

if(MV_UNITY_BUILD)
    set_target_properties(qt6advanceddocking PROPERTIES UNITY_BUILD ON)
endif()

# File compression: ZLib, a dependency for QuaZip
if(APPLE)
    message(STATUS "********Using brew zlib for QUAZIP *********")

    find_program(BREW_BIN brew)
    execute_process(COMMAND ${BREW_BIN} install pkg-config) 
endif()

find_package(ZLIB)

if(ZLIB_FOUND)
    message(STATUS "Using zlib version ${ZLIB_VERSION_STRING}")
else()
    set(ZLIB_ERROR_MSG "You need to define ZLIB_ROOT, pointing to a zlib installation")
    if (LINUX)
        set(ZLIB_ERROR_MSG "You need to install zlib, e.g. (on Ubuntu) apt install zlib1g")
    endif()

    message(FATAL_ERROR ${ZLIB_ERROR_MSG})
endif()

# File compression: QuaZip
set(QUAZIP_QT_MAJOR_VERSION 6)
set(QUAZIP_BZIP2 OFF CACHE BOOL "Enables BZIP2 compression")  # we use ZLIB, not BZIP
set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")                        # prevent some status messages from quazip
add_subdirectory(external/quazip)
set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")
get_target_property(QuaZip_VERSION QuaZip VERSION)
message(STATUS "Using QuaZip version ${QuaZip_VERSION}")

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------
include(CMakeMvSourcesApplication)    # defines MAIN_SOURCES, PRIVATE_SOURCES and PRIVATE_HEADERS
include(CMakeMvSourcesPublic)         # defines PUBLIC_SOURCES, PUBLIC_HEADERS and PRECOMPILE_HEADERS
include(CMakeMvSourcesResources)      # defines RESOURCE_FILES, UI_HEADERS and MAIN_APP_ICON_RESOURCE

# -----------------------------------------------------------------------------
# Target MV_PUBLIC_LIB
# -----------------------------------------------------------------------------
add_library(${MV_PUBLIC_LIB} SHARED)

set_target_properties(${MV_PUBLIC_LIB} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    AUTOMOC ON
    PROJECT_LABEL "MV_Public"       # Set the project name in VS different to the target name
    OUTPUT_NAME "MV_Public"         # Set the library output name different to the target name
)

target_sources(${MV_PUBLIC_LIB}
    PRIVATE
    src/.editorconfig
    ${PUBLIC_SOURCES}
)

target_include_directories(${MV_PUBLIC_LIB} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(${MV_PUBLIC_LIB} PRIVATE cxx_std_17)

target_compile_definitions(${MV_PUBLIC_LIB} PRIVATE MV_SHARED_EXPORT)

target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::Core)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::Gui)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::Widgets)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::OpenGL)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::WebEngineWidgets)

# Use AVX if enabled and available
check_and_set_AVX(${MV_PUBLIC_LIB} ${MV_USE_AVX})

if(MV_PRECOMPILE_HEADERS)
    target_precompile_headers(${MV_PUBLIC_LIB} PRIVATE 
        ${PRECOMPILE_HEADERS}
    )
endif()

if(MV_UNITY_BUILD)
    set_target_properties(${MV_PUBLIC_LIB} PROPERTIES UNITY_BUILD ON)
endif()

# -----------------------------------------------------------------------------
# Target MV_EXE
# -----------------------------------------------------------------------------
add_executable(${MV_EXE} MACOSX_BUNDLE WIN32)

set_target_properties(${MV_EXE} PROPERTIES
    AUTOMOC ON
    OUTPUT_NAME ${MV_APPLICATION_NAME}
)

target_sources(${MV_EXE}
    PRIVATE
    src/.editorconfig
    ${MAIN_SOURCES}
    ${PRIVATE_SOURCES}
    ${RESOURCE_FILES}
    ${MAIN_APP_ICON_RESOURCE}
)

target_include_directories(${MV_EXE} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}         # for resources in /res
)

target_compile_features(${MV_EXE} PRIVATE cxx_std_17)

target_link_libraries(${MV_EXE} PRIVATE Qt6::Core)
target_link_libraries(${MV_EXE} PRIVATE Qt6::Gui)
target_link_libraries(${MV_EXE} PRIVATE Qt6::Widgets)
target_link_libraries(${MV_EXE} PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(${MV_EXE} PRIVATE Qt6::OpenGL)
target_link_libraries(${MV_EXE} PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(${MV_EXE} PRIVATE qt6advanceddocking)
target_link_libraries(${MV_EXE} PRIVATE QuaZip)

target_link_libraries(${MV_EXE} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
check_and_set_AVX(${MV_EXE} ${MV_USE_AVX})

if(MV_PRECOMPILE_HEADERS)
    target_precompile_headers(${MV_EXE} PRIVATE 
        ${PRECOMPILE_HEADERS}
    )
endif()

if(MV_UNITY_BUILD)
    set_target_properties(${MV_EXE} PROPERTIES UNITY_BUILD ON)
endif()

if(MSVC)
    set_target_properties(${MV_EXE} PROPERTIES
        WIN32_EXECUTABLE YES
        LINK_FLAGS "/ENTRY:mainCRTStartup"
        LINK_FLAGS "/SUBSYSTEM:CONSOLE"
    )
endif(MSVC)

# find libraries relative to the executable in the macOS .app tree
if(APPLE)
    set_target_properties( ${MV_EXE} PROPERTIES BUILD_WITH_INSTALL_RPATH True)
endif()

# -----------------------------------------------------------------------------
# Installation
# -----------------------------------------------------------------------------

# Install the ManiVault executable 
if(APPLE)
    set(BUNDLE_DIR "${MV_INSTALL_DIR}/$<CONFIGURATION>/${MACOSX_BUNDLE_BUNDLE_NAME}.app")
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
                    \$<TARGET_FILE:qt6advanceddocking>\" 
                    \"${BUNDLE_DIR}/Contents/Frameworks/\"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                    \$<TARGET_FILE:QuaZip>\" 
                    \"${BUNDLE_DIR}/Contents/Frameworks/\"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                    \"$<TARGET_FILE:${MV_PUBLIC_LIB}>\" 
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
else()
    install(TARGETS ${MV_EXE}
        RUNTIME DESTINATION . COMPONENT MV_EXECUTABLE
    )
endif()

# Install public headers
foreach(PUBLIC_HEADER ${PUBLIC_HEADERS})
    # Remove the "src" prefix of the header get the remaining subdirectory
    string(REGEX REPLACE "^src/(.*/).*" "\\1" STRIPPED_PUBLIC_HEADER ${PUBLIC_HEADER})
    if("${STRIPPED_PUBLIC_HEADER}" STREQUAL "${PUBLIC_HEADER}")
        set(STRIPPED_PUBLIC_HEADER "")
    endif()
    set(HEADER_DEST "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/${STRIPPED_PUBLIC_HEADER}")

    # Install the header file while preserving the folder structure
    install(FILES ${PUBLIC_HEADER} DESTINATION ${HEADER_DEST} COMPONENT MVPUBLIC_HEADERS)
endforeach()

# Install public library
install(TARGETS ${MV_PUBLIC_LIB}
    EXPORT ManiVaultPublicExport
    RUNTIME DESTINATION . COMPONENT MVPUBLIC_LINKLIB    # Windows .dll
    ARCHIVE DESTINATION lib COMPONENT MVPUBLIC_LINKLIB  # Linux/Mac .so
    LIBRARY DESTINATION lib COMPONENT MVPUBLIC_LINKLIB  # Windows .lib
    INCLUDES DESTINATION include
)


if(MSVC)
    # Install program database (.pdb) files
    install(FILES $<TARGET_PDB_FILE:${MV_PUBLIC_LIB}> DESTINATION . COMPONENT MVPUBLIC_LINKLIB CONFIGURATIONS "Debug" OPTIONAL)
    install(FILES $<TARGET_PDB_FILE:${MV_EXE}>        DESTINATION . COMPONENT MV_EXECUTABLE    CONFIGURATIONS "Debug" OPTIONAL)
endif()

# TODO: Create a ManiVaultCore package to support the find_package command

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

# Generate the version file for use with find_package
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/ManiVaultConfigVersion.cmake"
  VERSION "${version}"
  # When the ManiVault core is stable - version 1.0 or greater -
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
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_HEADERS
        --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault public link library"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_LINKLIB
        --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
        --verbose
    # COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault cmake config files"
    # COMMAND ${CMAKE_COMMAND}
        # --install ${CMAKE_CURRENT_BINARY_DIR}
        # --config $<CONFIGURATION>
        # --component MVPUBLIC_PACKAGE
        # --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
        # --verbose
    # COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault cmake target files"
    # COMMAND ${CMAKE_COMMAND}
        # --install ${CMAKE_CURRENT_BINARY_DIR}
        # --config $<CONFIGURATION>
        # --component MVPUBLIC_TARGET
        # --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
        # --verbose
    # If no plugin directory exists, ManiVault might try to import 
    # any library in the executable directory as a plugin
    COMMAND ${CMAKE_COMMAND}
        -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/Plugins
)

# Install the ManiVault executable
if(APPLE)
    add_custom_command(TARGET ${MV_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault executable bundle"
        COMMAND ${CMAKE_COMMAND}
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION>
            --component MACOS_BUNDLE
            --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>/
    )
endif()

if(NOT APPLE)
    add_custom_command(TARGET ${MV_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault executable"
        COMMAND ${CMAKE_COMMAND}
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION>
            --component MV_EXECUTABLE
            --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
            --verbose
    )

    # Install dependency runtime libraries
    # on windows, the DLLs should be placed in the same directory as the executable
    add_custom_command(TARGET ${MV_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Installing: dependency runtime libraries"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:qt6advanceddocking> ${MV_INSTALL_DIR}/$<CONFIGURATION>$<IF:$<CXX_COMPILER_ID:MSVC>,,/lib/>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:QuaZip> ${MV_INSTALL_DIR}/$<CONFIGURATION>$<IF:$<CXX_COMPILER_ID:MSVC>,,/lib/>
    )

    # Create symlink with slightly different name required on some linux distributions
    if(UNIX)
        include(CMakeQuaZipSymlink)
        create_quazip_symlink()
    endif()
endif()

# Install examples
add_custom_command(TARGET ${MV_EXE}  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault examples"
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<VERSION_LESS:${CMAKE_VERSION},3.26.0>,copy_directory,copy_directory_if_different>  # copy_directory_if_different is only available starting 3.26
            ${CMAKE_CURRENT_SOURCE_DIR}/res/examples
            ${MV_INSTALL_DIR}/$<CONFIGURATION>/examples
)

# Install ManiVault licenses
add_custom_command(TARGET ${MV_EXE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault licenses"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/LICENSE
)

# Install dependency licenses
add_custom_command(TARGET ${MV_EXE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: dependency licenses"
    COMMAND ${CMAKE_COMMAND} 
        --install ${CMAKE_CURRENT_BINARY_DIR}/external/advanced_docking
        --config $<CONFIGURATION> 
        --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>/
        --component license
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/quazip
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/external/quazip/COPYING ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/quazip/LICENSE
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
    set_property(TARGET ${MV_EXE} PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=${QT_DIR}/../../../bin")
    set_property(TARGET ${MV_EXE} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY  $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/debug,${MV_INSTALL_DIR}/release>)
    set_property(TARGET ${MV_EXE} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/debug/${MV_APPLICATION_NAME}.exe,${MV_INSTALL_DIR}/release/${MV_APPLICATION_NAME}.exe>)
endif()
