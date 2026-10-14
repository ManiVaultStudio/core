add_executable(PointDataGTest
    PointDataGTestMain.cpp
    PointDataGTest.cpp
    ../src/tiled/PointDataTileCache.cpp
    ../src/tiled/PointDataTiledStorage.cpp
)

target_include_directories(PointDataGTest PRIVATE
    "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/"
    "${CMAKE_CURRENT_BINARY_DIR}/.."
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../private"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/biovault"
)

target_compile_features(PointDataGTest PRIVATE cxx_std_${MV_CXX_STANDARD})

target_link_libraries(PointDataGTest PRIVATE
    ${MV_PUBLIC_LIB}
    PointData
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Network
    GTest::gtest
)

set_target_properties(PointDataGTest PROPERTIES
    FOLDER "Tests/PointData"
)

if(MSVC)
    target_compile_options(PointDataGTest PRIVATE /W4)
else()
    target_compile_options(PointDataGTest PRIVATE -Wall -Wextra -pedantic)
endif()

add_test(NAME PointDataGTest COMMAND PointDataGTest)
set_tests_properties(PointDataGTest PROPERTIES
    LABELS "unit;pointdata"
    ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Core>;PATH=path_list_prepend:$<TARGET_FILE_DIR:PointData>;PATH=path_list_prepend:${QT_DIR}/../../../bin"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET PointDataGTest PROPERTY
        VS_DEBUGGER_ENVIRONMENT "PATH=$<TARGET_FILE_DIR:Core>;$<TARGET_FILE_DIR:PointData>;${QT_DIR}/../../../bin"
    )
endif()

