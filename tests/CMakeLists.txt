# Test targets are grouped by architectural scope and registered with CTest
# when MV_BUILD_TESTING is enabled by the top-level project.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(public)
add_subdirectory(application)
add_subdirectory(integration)
add_subdirectory(behavioral)
