add_executable(MV_Application_GifEncoderTest
    GifEncoderTest.cpp
    ../../src/private/GifEncoder.cpp
)

target_include_directories(MV_Application_GifEncoderTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/private
)

target_link_libraries(MV_Application_GifEncoderTest PRIVATE
    Qt6::Core
    Qt6::Gui
)

target_compile_features(MV_Application_GifEncoderTest PRIVATE cxx_std_${MV_CXX_STANDARD})
set_target_properties(MV_Application_GifEncoderTest PROPERTIES FOLDER "Tests/Application")

add_test(NAME MV_Application_GifEncoderTest COMMAND MV_Application_GifEncoderTest)
set_tests_properties(MV_Application_GifEncoderTest PROPERTIES
    LABELS "unit;application"
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${QT_DIR}/../../../bin"
)

# Keep Visual Studio test launches consistent with the application launch.
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET MV_Application_GifEncoderTest PROPERTY
        VS_DEBUGGER_ENVIRONMENT "PATH=${QT_DIR}/../../../bin"
    )
endif()
