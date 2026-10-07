# Test targets are grouped by architectural scope and registered with CTest
# when MV_BUILD_TESTING is enabled by the top-level project.
add_subdirectory(public)
add_subdirectory(application)
add_subdirectory(integration)
add_subdirectory(behavioral)
