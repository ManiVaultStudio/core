# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

find_package(Qt6 6.8 COMPONENTS Core Widgets OpenGL OpenGLWidgets WebEngineWidgets Concurrent REQUIRED)
add_definitions(-DQT_MESSAGELOGCONTEXT)
message(STATUS "Using Qt version ${Qt6Widgets_VERSION}")

# Download and set up third party dependencies
include(DownloadDependencies)

# Downloads external icons library
include(DownloadFontAwesome)

# -----------------------------------------------------------------------------
# Main library and application
# -----------------------------------------------------------------------------
add_subdirectory(lib)
add_subdirectory(app)

# -----------------------------------------------------------------------------
# Plugins
# -----------------------------------------------------------------------------

# System plugins
add_subdirectory(plugins/DataHierarchyPlugin)
add_subdirectory(plugins/DataPropertiesPlugin)
add_subdirectory(plugins/SharedParametersPlugin)
add_subdirectory(plugins/LoggingPlugin)
add_subdirectory(plugins/TasksPlugin)
add_subdirectory(plugins/SampleScopePlugin)
add_subdirectory(plugins/TutorialPlugin)

# Data plugins
add_subdirectory(plugins/PointData)
add_subdirectory(plugins/ColorData)
add_subdirectory(plugins/TextData)
add_subdirectory(plugins/ClusterData)
add_subdirectory(plugins/ImageData)

# -----------------------------------------------------------------------------
# Miscellaneous
# -----------------------------------------------------------------------------

# setup variables that find_packge config would define, for building core and plugins in the same project (DevBundle)
set(ManiVault_INCLUDE_DIR "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/" CACHE INTERNAL "")
set(ManiVault_LIB_DIR "${MV_INSTALL_DIR}/$<CONFIGURATION>/lib/" CACHE INTERNAL "")
set(ManiVault_INSTALL_DIR "${MV_INSTALL_DIR}" CACHE INTERNAL "")
set(ManiVault_CMAKE_DIR "${MV_ROOT_DIR}/cmake/" CACHE INTERNAL "") # this differs from config
set(ManiVault_VERSION "${MV_VERSION}" CACHE INTERNAL "")
set(ManiVault_CXX_STANDARD "${MV_CXX_STANDARD}" CACHE INTERNAL "")
