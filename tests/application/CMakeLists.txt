add_executable(MV_Application_GifEncoderTest
    GifEncoderTest.cpp
    ${MV_ROOT_DIR}/src/app/GifEncoder.cpp
)

source_group("Tests/Application" FILES GifEncoderTest.cpp)
source_group("Tests/Application/Implementation" FILES ${MV_ROOT_DIR}/src/app/GifEncoder.cpp)

target_include_directories(MV_Application_GifEncoderTest PRIVATE
    ${MV_ROOT_DIR}/src/app
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

add_executable(MV_Application_ArchiverTest
    ArchiverTest.cpp
    ${MV_ROOT_DIR}/src/app/Archiver.cpp
)

source_group("Tests/Application" FILES ArchiverTest.cpp)
source_group("Tests/Application/Implementation" FILES ${MV_ROOT_DIR}/src/app/Archiver.cpp)

target_include_directories(MV_Application_ArchiverTest PRIVATE
    ${MV_ROOT_DIR}/src/app
    ${MV_ROOT_DIR}/src/lib
)

target_link_libraries(MV_Application_ArchiverTest PRIVATE
    ManiVault::Core
    Qt6::Core
    QuaZip
    ZLIB::ZLIB
)

target_compile_features(MV_Application_ArchiverTest PRIVATE cxx_std_${MV_CXX_STANDARD})
set_target_properties(MV_Application_ArchiverTest PROPERTIES
    AUTOMOC ON
    FOLDER "Tests/Application"
)

add_test(NAME MV_Application_ArchiverTest COMMAND MV_Application_ArchiverTest)
set_tests_properties(MV_Application_ArchiverTest PROPERTIES
    LABELS "unit;application;security"
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:${QT_DIR}/../../../bin"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET MV_Application_ArchiverTest PROPERTY
        VS_DEBUGGER_ENVIRONMENT "PATH=${QT_DIR}/../../../bin"
    )
endif()

add_executable(MV_Application_TemporaryDirectoriesTest
    ApplicationTemporaryDirectoriesTest.cpp
)

source_group("Tests/Application" FILES ApplicationTemporaryDirectoriesTest.cpp)

target_include_directories(MV_Application_TemporaryDirectoriesTest PRIVATE
    ${MV_ROOT_DIR}/src/lib
)

target_link_libraries(MV_Application_TemporaryDirectoriesTest PRIVATE
    ManiVault::Core
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::WebEngineWidgets
)

target_compile_features(MV_Application_TemporaryDirectoriesTest PRIVATE cxx_std_${MV_CXX_STANDARD})
set_target_properties(MV_Application_TemporaryDirectoriesTest PROPERTIES
    AUTOMOC ON
    FOLDER "Tests/Application"
)

add_test(NAME MV_Application_TemporaryDirectoriesTest COMMAND MV_Application_TemporaryDirectoriesTest)
set_tests_properties(MV_Application_TemporaryDirectoriesTest PROPERTIES
    LABELS "unit;application"
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:ManiVault::Core>;PATH=path_list_prepend:${QT_DIR}/../../../bin"
)

add_test(NAME MV_Application_TemporaryDirectoriesFallbackTest COMMAND MV_Application_TemporaryDirectoriesTest --invalid-root)
set_tests_properties(MV_Application_TemporaryDirectoriesFallbackTest PROPERTIES
    LABELS "unit;application"
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:ManiVault::Core>;PATH=path_list_prepend:${QT_DIR}/../../../bin"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET MV_Application_TemporaryDirectoriesTest PROPERTY
        VS_DEBUGGER_ENVIRONMENT "QT_QPA_PLATFORM=offscreen;PATH=$<TARGET_FILE_DIR:ManiVault::Core>"
    )
endif()
