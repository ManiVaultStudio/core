add_library(MV_HeadlessCoreTestPlugin SHARED
    HeadlessCoreTestPlugin.h
    HeadlessCoreTestPlugin.cpp
    HeadlessCoreTestPlugin.json
)

target_include_directories(MV_HeadlessCoreTestPlugin PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

target_link_libraries(MV_HeadlessCoreTestPlugin PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Widgets
    Qt6::WebEngineWidgets
    ManiVault::Core
)

target_compile_features(MV_HeadlessCoreTestPlugin PRIVATE cxx_std_${MV_CXX_STANDARD})
set_target_properties(MV_HeadlessCoreTestPlugin PROPERTIES
    AUTOMOC ON
    FOLDER "Tests/Integration"
)

install(TARGETS MV_HeadlessCoreTestPlugin
    RUNTIME DESTINATION Plugins
    LIBRARY DESTINATION Plugins
)

add_custom_command(TARGET MV_HeadlessCoreTestPlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${MV_INSTALL_DIR}/$<CONFIGURATION>/Plugins"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:MV_HeadlessCoreTestPlugin>"
        "${MV_INSTALL_DIR}/$<CONFIGURATION>/Plugins/"
)

if(APPLE)
    set(MV_TEST_APPLICATION "${MV_INSTALL_DIR}/$<CONFIG>/ManiVault Studio.app/Contents/MacOS/ManiVault Studio")
else()
    set(MV_TEST_APPLICATION "${MV_INSTALL_DIR}/$<CONFIG>/$<TARGET_FILE_NAME:MV_Application>")
endif()

add_test(
    NAME MV_Integration_CoreScenario_core_initialized
    COMMAND "${MV_TEST_APPLICATION}" --run-core-scenario core.initialized
)
set_tests_properties(MV_Integration_CoreScenario_core_initialized PROPERTIES
    LABELS "integration;core-scenario"
    TIMEOUT 60
    WORKING_DIRECTORY "${MV_INSTALL_DIR}/$<CONFIG>"
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${QT_DIR}/../../../bin"
)
