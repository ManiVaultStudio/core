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

add_executable(MV_Public_ColorSchemeTest
    ColorSchemeTest.cpp
)

target_include_directories(MV_Public_ColorSchemeTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

target_link_libraries(MV_Public_ColorSchemeTest PRIVATE
    ManiVault::Core
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_features(MV_Public_ColorSchemeTest PRIVATE cxx_std_${MV_CXX_STANDARD})
set_target_properties(MV_Public_ColorSchemeTest PROPERTIES FOLDER "Tests/Public")

add_test(NAME MV_Public_ColorSchemeTest COMMAND MV_Public_ColorSchemeTest)
set_tests_properties(MV_Public_ColorSchemeTest PROPERTIES
    LABELS "unit;public"
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Core>;PATH=path_list_prepend:${QT_DIR}/../../../bin"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET MV_Public_ColorSchemeTest PROPERTY
        VS_DEBUGGER_ENVIRONMENT "PATH=${QT_DIR}/../../../bin"
    )
endif()

add_executable(MV_Public_BlobCodecTest
    BlobCodecTest.cpp
)

target_include_directories(MV_Public_BlobCodecTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

target_link_libraries(MV_Public_BlobCodecTest PRIVATE
    ManiVault::Core
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_features(MV_Public_BlobCodecTest PRIVATE cxx_std_${MV_CXX_STANDARD})
set_target_properties(MV_Public_BlobCodecTest PROPERTIES FOLDER "Tests/Public")

add_test(NAME MV_Public_BlobCodecTest COMMAND MV_Public_BlobCodecTest)
set_tests_properties(MV_Public_BlobCodecTest PROPERTIES
    LABELS "unit;public"
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Core>;PATH=path_list_prepend:${QT_DIR}/../../../bin"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET MV_Public_BlobCodecTest PROPERTY
        VS_DEBUGGER_ENVIRONMENT "PATH=${QT_DIR}/../../../bin"
    )
endif()

add_executable(MV_Public_ColorMapTest
    ColorMapTest.cpp
)

target_include_directories(MV_Public_ColorMapTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

target_link_libraries(MV_Public_ColorMapTest PRIVATE
    ManiVault::Core
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_features(MV_Public_ColorMapTest PRIVATE cxx_std_${MV_CXX_STANDARD})
set_target_properties(MV_Public_ColorMapTest PROPERTIES FOLDER "Tests/Public")

add_test(NAME MV_Public_ColorMapTest COMMAND MV_Public_ColorMapTest)
set_tests_properties(MV_Public_ColorMapTest PROPERTIES
    LABELS "unit;public"
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Core>;PATH=path_list_prepend:${QT_DIR}/../../../bin"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET MV_Public_ColorMapTest PROPERTY
        VS_DEBUGGER_ENVIRONMENT "PATH=${QT_DIR}/../../../bin"
    )
endif()

add_executable(MV_Public_DataTypeTest
    DataTypeTest.cpp
)

target_include_directories(MV_Public_DataTypeTest PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
)

target_link_libraries(MV_Public_DataTypeTest PRIVATE
    ManiVault::Core
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

target_compile_features(MV_Public_DataTypeTest PRIVATE cxx_std_${MV_CXX_STANDARD})
set_target_properties(MV_Public_DataTypeTest PROPERTIES FOLDER "Tests/Public")

add_test(NAME MV_Public_DataTypeTest COMMAND MV_Public_DataTypeTest)
set_tests_properties(MV_Public_DataTypeTest PROPERTIES
    LABELS "unit;public"
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Core>;PATH=path_list_prepend:${QT_DIR}/../../../bin"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET MV_Public_DataTypeTest PROPERTY
        VS_DEBUGGER_ENVIRONMENT "PATH=${QT_DIR}/../../../bin"
    )
endif()
