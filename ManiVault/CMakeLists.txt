cmake_minimum_required(VERSION 3.17)

# -----------------------------------------------------------------------------
# Project: ManiVault-core
# -----------------------------------------------------------------------------
set(MV_CORE "mv-core")
project(${MV_CORE} 
    VERSION ${MV_VERSION} 
    DESCRIPTION "Core application and library of ManiVault Studio, a flexible and extensible visual analytics framework for high-dimensional data"
    LANGUAGES CXX C)

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
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND MSVC_VERSION LESS 1930))
    set(CMAKE_C_STANDARD 17)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS OFF)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INSTALL_MESSAGE LAZY)

# include CMakePackageConfigHelpers macro
include(CMakePackageConfigHelpers)

# Test hardware avx capabilities
include(mv_check_and_set_AVX)

# Helper function to install runtime dependencies
include(mv_install_dependencies_utils)

# Generate a header file that contains the EXPORT macro for public libraries
include(GenerateExportHeader)

# Cmake package manager, downloads dependencies during configuration
include(CPM)

# create a file that defines MV_VERSION_MAJOR, etc
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/ManiVaultVersion.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/src/ManiVaultVersion.h" @ONLY)

# -----------------------------------------------------------------------------
# Apple settings
# -----------------------------------------------------------------------------
if(APPLE)
    # These are the properties for the Info.plist in the mac bindle
    set(MACOSX_BUNDLE_BUNDLE_NAME "ManiVault Studio")
    set(MACOSX_BUNDLE_PATHNAME "ManiVault\ Studio")
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
	
	# No linking optimization for MSVC builds with error logging (linking optimization screws up the program database which prohibits symbolication)
	if(${MV_USE_ERROR_LOGGING})
		# set(CMAKE_C_FLAGS_RELEASE "/Od /Zi")        	# Disable optimization and enable debug info
		# set(CMAKE_CXX_FLAGS_RELEASE "/Od /Zi")     		# Same for C++
		# set(CMAKE_EXE_LINKER_FLAGS_RELEASE "/DEBUG")  	# Keep debug symbols in the executable
		
		# Remove /OPT:ICF flag from CMake's default flags
		string(REPLACE "/OPT:ICF" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
	endif()
endif(MSVC)

# Add preprocessor directive to notify sources of error logging
if(${MV_USE_ERROR_LOGGING})
	add_compile_definitions(ERROR_LOGGING)
endif()
	
# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

# Qt
find_package(Qt6 6.3 COMPONENTS Core Widgets OpenGL OpenGLWidgets WebEngineWidgets REQUIRED)

add_definitions(-DQT_MESSAGELOGCONTEXT)

message(STATUS "Using Qt version ${Qt6Widgets_VERSION}")

# File compression: zlib
CPMAddPackage(
  NAME              ZLIB
  GITHUB_REPOSITORY madler/zlib
  GIT_TAG           v1.3.1
  OPTIONS "ZLIB_BUILD_EXAMPLES OFF" "SKIP_INSTALL_ALL ON"
)

set_target_properties(zlib PROPERTIES EXCLUDE_FROM_ALL TRUE)

# Maintain build compatibility between find_package and CMakeLists.txt variants (for QuaZip)
add_library(ZLIB::ZLIB ALIAS zlibstatic)
set(ZLIB_LIBRARIES zlibstatic)

# File compression: QuaZip
CPMAddPackage(
  NAME              quazip
  GITHUB_REPOSITORY stachenov/quazip
  GIT_TAG           v1.4
  OPTIONS "QUAZIP_QT_MAJOR_VERSION 6" "QUAZIP_BZIP2 OFF" "QUAZIP_INSTALL OFF" "BUILD_SHARED_LIBS OFF"
)

# Qt Advanced docking system
CPMAddPackage(
  NAME              advanced_docking
  GITHUB_REPOSITORY githubuser0xFFFF/Qt-Advanced-Docking-System
  GIT_TAG           4.3.1
  OPTIONS "BUILD_EXAMPLES OFF"
)

if(MV_UNITY_BUILD)
    set_target_properties(qt6advanceddocking PROPERTIES UNITY_BUILD ON)
endif()

set_target_properties(qt6advanceddocking PROPERTIES PROJECT_LABEL "qtadvanceddocking" OUTPUT_NAME "qtadvanceddocking")

# bfloat16 implementation for pre c++23
CPMAddPackage(
  NAME              biovault_bfloat16
  GITHUB_REPOSITORY biovault/biovault_bfloat16
  GIT_TAG           v1.0.1
  DOWNLOAD_ONLY     YES
  EXCLUDE_FROM_ALL  YES
  SOURCE_DIR        ${CMAKE_CURRENT_SOURCE_DIR}/external/biovault/biovault_bfloat16/
)

if(${MV_USE_ERROR_LOGGING})
	
	# Add Sentry with CPM
	CPMAddPackage(
		NAME sentry-native
		GIT_REPOSITORY https://github.com/getsentry/sentry-native.git
		GIT_TAG 0.7.19 														# Use the latest stable version
		OPTIONS
			"SENTRY_BACKEND crashpad"										# Use Crashpad for crash reporting
			"SENTRY_TRANSPORT winhttp" 										# Use the default HTTP transport
	)

	include_directories(${sentry-native_SOURCE_DIR}/include)
	include_directories(${sentry-native_SOURCE_DIR}/external/crashpad/client)

	# Put targets in the proper source folder
	set_target_properties(sentry mini_chromium
		PROPERTIES
		FOLDER CoreDependencies
	)
	
	# Put targets in the proper source folder
	set_target_properties(crashpad_client crashpad_compat crashpad_getopt crashpad_handler crashpad_handler_lib crashpad_minidump crashpad_snapshot crashpad_tools crashpad_util crashpad_wer crashpad_zlib
		PROPERTIES
		FOLDER CoreDependencies/Crashpad
	)
	
endif()

# -----------------------------------------------------------------------------
# Source files
# -----------------------------------------------------------------------------
include(CMakeMvSourcesApplication)    # defines MAIN_SOURCES, PRIVATE_SOURCES and PRIVATE_HEADERS
include(CMakeMvSourcesPublic)         # defines PUBLIC_SOURCES, PUBLIC_HEADERS and PRECOMPILE_HEADERS
include(CMakeMvSourcesResources)      # defines RESOURCE_FILES, MAIN_APP_ICON_RESOURCE and MAIN_STYLES

# -----------------------------------------------------------------------------
# Target MV_PUBLIC_LIB
# -----------------------------------------------------------------------------
add_library(${MV_PUBLIC_LIB} SHARED)

add_library(ManiVault::Core ALIAS Core)

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
    ${SHADERS}
)

target_include_directories(${MV_PUBLIC_LIB} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_compile_features(${MV_PUBLIC_LIB} PRIVATE cxx_std_17)

target_compile_definitions(${MV_PUBLIC_LIB} PRIVATE MV_SHARED_EXPORT)

target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::Core)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::Gui)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::Widgets)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::OpenGL)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::OpenGLWidgets)
target_link_libraries(${MV_PUBLIC_LIB} PRIVATE Qt6::WebEngineWidgets)

# Use AVX if enabled and available
mv_check_and_set_AVX(${MV_PUBLIC_LIB} ${MV_USE_AVX})

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
    ${MAIN_STYLES}
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

if(${MV_USE_ERROR_LOGGING})
	target_link_libraries(${MV_EXE} PRIVATE sentry)
	target_link_libraries(${MV_EXE} PRIVATE crashpad_client)
endif()

target_link_libraries(${MV_EXE} PRIVATE ${MV_PUBLIC_LIB})

# Use AVX if enabled and available
mv_check_and_set_AVX(${MV_EXE} ${MV_USE_AVX})

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

if(APPLE)
    
    # Find libraries relative to the executable in the macOS .app tree
    set_target_properties(${MV_EXE} PROPERTIES BUILD_WITH_INSTALL_RPATH True)

endif()

set_target_properties(qt6advanceddocking QuaZip zlib zlibstatic
    PROPERTIES
    FOLDER CoreDependencies
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
add_subdirectory(src/plugins/SampleScopePlugin)
add_subdirectory(src/plugins/TutorialPlugin)

set_target_properties(DataHierarchyPlugin DataPropertiesPlugin SharedParametersPlugin LoggingPlugin TasksPlugin SampleScopePlugin TutorialPlugin
    PROPERTIES
    FOLDER SystemPlugins
)

# Data plugins
add_subdirectory(src/plugins/PointData)
add_subdirectory(src/plugins/ColorData)
add_subdirectory(src/plugins/TextData)
add_subdirectory(src/plugins/ClusterData)
add_subdirectory(src/plugins/ImageData)

set_target_properties(PointData ColorData TextData ClusterData ImageData
    PROPERTIES
    FOLDER DataPlugins
)

# -----------------------------------------------------------------------------
# Installation
# -----------------------------------------------------------------------------

# Install the ManiVault executable 
if(APPLE)
    # set(BUNDLE_DIR "${MV_INSTALL_DIR}/$<CONFIGURATION>/${MACOSX_BUNDLE_BUNDLE_NAME}.app")
    set(BUNDLE_DIR "${MV_INSTALL_DIR}/$<CONFIGURATION>/ManiVault Studio.app")
    set(WEBENGINEINSTALL_CMD "${CMAKE_CURRENT_SOURCE_DIR}/macos_webengineinstall.sh")
    install(TARGETS ${MV_EXE}
        RUNTIME DESTINATION bin COMPONENT MACOS_BUNDLE
        BUNDLE DESTINATION . COMPONENT MACOS_BUNDLE
    )
    # -always-overwrite
    find_program(MACDEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
    install(CODE "
            message(\"**** MacOS bundle install ****\")
            file(MAKE_DIRECTORY \"${BUNDLE_DIR}\")
            message(\"**** Run macdeployqt for \"${BUNDLE_DIR}\" ****\")
            execute_process(COMMAND ${MACDEPLOYQT_EXECUTABLE} \"${BUNDLE_DIR}\" -always-overwrite RESULT_VARIABLE retval)
            if(NOT retval EQUAL 0)
        	    message(FATAL_ERROR \"macdeployqt failed\")
            endif()
            set(ADDITIONAL_LIBS
				\"$<TARGET_FILE:qt6advanceddocking>\"
				\"$<TARGET_FILE:${MV_PUBLIC_LIB}>\"
				\"$<TARGET_FILE:sentry>\"
				\"$<TARGET_FILE:crashpad_wer>\"
				\"$<TARGET_FILE:crashpad_handler}>\"
			)
            message(\"**** Copy additional libs: \${ADDITIONAL_LIBS}\")
            file(COPY \${ADDITIONAL_LIBS} DESTINATION \"${BUNDLE_DIR}/Contents/Frameworks/\")
            
        "
        COMPONENT MACOS_BUNDLE
    )
      
    install(CODE "execute_process(
            COMMAND bash ${WEBENGINEINSTALL_CMD} ${Qt6_DIR} \"${BUNDLE_DIR}\"
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
    EXPORT "ManiVaultPublicExport"
    RUNTIME DESTINATION $<CONFIGURATION>        COMPONENT MVPUBLIC_LINKLIB  # Windows .dll
    ARCHIVE DESTINATION "$<CONFIGURATION>/lib/" COMPONENT MVPUBLIC_LINKLIB  # Linux/Mac .so
    LIBRARY DESTINATION "$<CONFIGURATION>/lib/" COMPONENT MVPUBLIC_LINKLIB  # Windows .lib
)

# Generate a .cmake file that can be imported from the install to enable plugin building
install(EXPORT "ManiVaultPublicExport"
        FILE ManiVaultTargets.cmake
        NAMESPACE ManiVault::
        DESTINATION cmake/mv
        COMPONENT MVPUBLIC_EXPORT
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/mv_install_dependencies_utils.cmake"
        DESTINATION cmake/mv
        COMPONENT MVPUBLIC_CMAKE_HELPER
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/mv_install_dependencies.cmake.in"
        DESTINATION cmake/mv
        COMPONENT MVPUBLIC_CMAKE_HELPER
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/cmake/mv_check_and_set_AVX.cmake"
        DESTINATION cmake/mv
        COMPONENT MVPUBLIC_CMAKE_HELPER
)

if(MSVC)
    # Install program database (.pdb) files
    install(FILES $<TARGET_PDB_FILE:${MV_PUBLIC_LIB}> DESTINATION $<CONFIGURATION> COMPONENT MVPUBLIC_LINKLIB CONFIGURATIONS "Debug" OPTIONAL)
    install(FILES $<TARGET_PDB_FILE:${MV_EXE}>        DESTINATION .                COMPONENT MV_EXECUTABLE    CONFIGURATIONS "Debug" OPTIONAL)
endif()

# Generate the version file for use with find_package
write_basic_package_version_file(
  ManiVaultConfigVersion.cmake
  VERSION ${MV_VERSION}
  COMPATIBILITY SameMajorVersion
)

# create config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/MvCoreConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/ManiVaultConfig.cmake"
    INSTALL_DESTINATION cmake/mv
    PATH_VARS MV_INSTALL_DIR
)

# install the package files
install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ManiVaultConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ManiVaultConfigVersion.cmake"
        DESTINATION cmake/mv
        COMPONENT MVPUBLIC_PACKAGE
)

# setup variables that find_packge config would define, for building core and plugins in the same project
set(ManiVault_INCLUDE_DIR "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/" CACHE INTERNAL "")
set(ManiVault_LIB_DIR "${MV_INSTALL_DIR}/$<CONFIGURATION>/lib/" CACHE INTERNAL "")
set(ManiVault_INSTALL_DIR "${MV_INSTALL_DIR}" CACHE INTERNAL "")
set(ManiVault_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" CACHE INTERNAL "") # this differs from config

# Call the install functions of the all public targets
# needed for plugins directly after the build
add_custom_command(TARGET ${MV_PUBLIC_LIB} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault public headers"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_HEADERS
        --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault public link library"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_LINKLIB
        --prefix ${MV_INSTALL_DIR}
        --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault cmake config files"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_PACKAGE
        --prefix ${MV_INSTALL_DIR}
        --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault cmake target files"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_EXPORT
        --prefix ${MV_INSTALL_DIR}
        --verbose
    COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault cmake helper files"
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_CURRENT_BINARY_DIR}
        --config $<CONFIGURATION>
        --component MVPUBLIC_CMAKE_HELPER
        --prefix ${MV_INSTALL_DIR}
        --verbose
    # If no plugin directory exists, ManiVault might try to import 
    # any library in the executable directory as a plugin
    COMMAND ${CMAKE_COMMAND}
        -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/Plugins
)

# Install the ManiVault executable
if(APPLE)
    add_custom_command(TARGET ${MV_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault MacOS bundle to ${MV_INSTALL_DIR}/$<CONFIGURATION>"
        COMMAND ${CMAKE_COMMAND}
            --install ${CMAKE_BINARY_DIR}
            --config $<CONFIGURATION>
            --component MACOS_BUNDLE
            --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
            --verbose
    )
endif()

if(NOT APPLE)
    add_custom_command(TARGET ${MV_EXE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Installing: ManiVault executable to ${MV_INSTALL_DIR}/$<CONFIGURATION>"
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
    )
	
	if(${MV_USE_ERROR_LOGGING})
		
		# Copy sentry-native runtime to install directory after successful compilation
		add_custom_target(sentry_post_build ALL
			COMMAND ${CMAKE_COMMAND} -E echo "Installing: sentry runtime library"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:sentry> ${MV_INSTALL_DIR}/$<CONFIGURATION>/
			DEPENDS sentry  # Ensure sentry_native is built first
		)
		
		# Copy crashpad_wer runtime to install directory after successful compilation
		add_custom_target(crashpad_wer_post_build ALL
			COMMAND ${CMAKE_COMMAND} -E echo "Installing: crashpad_wer runtime library"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:crashpad_wer> ${MV_INSTALL_DIR}/$<CONFIGURATION>/
			DEPENDS crashpad_wer  # Ensure crashpad_wer is built first
		)
		
		# Copy crashpad_handler runtime to install directory after successful compilation
		add_custom_target(crashpad_handler_post_build ALL
			COMMAND ${CMAKE_COMMAND} -E echo "Installing: crashpad_handler runtime library"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:crashpad_handler> ${MV_INSTALL_DIR}/$<CONFIGURATION>/
			DEPENDS crashpad_handler  # Ensure sentry_native is built first
		)
		
		# Make ManiVault executable depend on the custom post-build targets
		add_dependencies(${MV_EXE} sentry_post_build)
		add_dependencies(${MV_EXE} crashpad_wer_post_build)
		add_dependencies(${MV_EXE} crashpad_handler_post_build)
		
		# Put custom post-build targets in the proper source folder
		set_target_properties(sentry_post_build crashpad_wer_post_build crashpad_handler_post_build
			PROPERTIES
			FOLDER CoreDependencies/Miscellaneous
		)
		
		if(MSVC)
			# Find all .pdb files in the build directory
			file(GLOB_RECURSE PDB_FILES "${CMAKE_BINARY_DIR}/**/*.pdb")
			
			# Program database files target directory
			set(PDB_DIR "${MV_INSTALL_DIR}/$<CONFIGURATION>/pdb/")
			
			# Copy all PDB files after the ManiVault executable has been built
			add_custom_command(
				TARGET ${MV_EXE} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory ${PDB_DIR}  					# Ensure the target PDB directory exists
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PDB_FILES} ${PDB_DIR}	# Copy all .pdb files
				COMMENT "Copying .pdb files to ${PDB_DIR}"
			)

		endif()
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
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/ads
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${advanced_docking_SOURCE_DIR}/LICENSE ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/ads/LICENSE
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/quazip
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${quazip_SOURCE_DIR}/COPYING ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/quazip/LICENSE
	COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/zlib
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${zlib_SOURCE_DIR}/LICENSE ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/zlib/LICENSE
    COMMAND ${CMAKE_COMMAND} -E make_directory ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/biovault_bfloat16
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${biovault_bfloat16_SOURCE_DIR}/LICENSE ${MV_INSTALL_DIR}/$<CONFIGURATION>/license/biovault_bfloat16/LICENSE
)

# -----------------------------------------------------------------------------
# Miscellaneous
# -----------------------------------------------------------------------------

# Automatically set the debug environment (command + working directory) for MSVC
if(MSVC)
    set_property(TARGET ${MV_EXE} PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=${QT_DIR}/../../../bin")
    set_property(TARGET ${MV_EXE} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY  $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/debug,${MV_INSTALL_DIR}/release>)
    set_property(TARGET ${MV_EXE} PROPERTY VS_DEBUGGER_COMMAND $<IF:$<CONFIG:DEBUG>,${MV_INSTALL_DIR}/debug/${MV_APPLICATION_NAME}.exe,${MV_INSTALL_DIR}/release/${MV_APPLICATION_NAME}.exe>)
endif()
