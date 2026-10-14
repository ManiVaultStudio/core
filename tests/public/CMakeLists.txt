add_executable(MV_Public_ParseByteSizeTest
    ParseByteSizeTest.cpp
)

target_include_directories(MV_Public_ParseByteSizeTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

target_link_libraries(MV_Public_ParseByteSizeTest PRIVATE
    ManiVault::Core
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_features(MV_Public_ParseByteSizeTest PRIVATE cxx_std_${MV_CXX_STANDARD})
set_target_properties(MV_Public_ParseByteSizeTest PROPERTIES FOLDER "Tests/Public")

add_test(NAME MV_Public_ParseByteSizeTest COMMAND MV_Public_ParseByteSizeTest)
set_tests_properties(MV_Public_ParseByteSizeTest PROPERTIES
    LABELS "unit;public"
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Core>;PATH=path_list_prepend:${QT_DIR}/../../../bin"
)

# Keep Visual Studio test launches consistent with the application launch.
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET MV_Public_ParseByteSizeTest PROPERTY
        VS_DEBUGGER_ENVIRONMENT "PATH=${QT_DIR}/../../../bin"
    )
endif()
