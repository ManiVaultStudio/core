
add_executable(PointDataGTest
    PointDataGTest.cpp
    PointsGTest.cpp
)

target_include_directories(PointDataGTest BEFORE PRIVATE 
    ${CMAKE_SOURCE_DIR}/HDPS/src # For <MainWindow.h>
    ${CMAKE_SOURCE_DIR}/HDPS/src/plugins/PointData/src # For the header files to be tested
    ${CMAKE_BINARY_DIR}/HDPS # For <ui_MainWindow.h>
)

target_link_libraries(PointDataGTest
    HDPS_Private # For MainWindow
    PointData
    Qt5::Widgets
    gtest_main
)

if(MSVC)
    target_compile_options(PointDataGTest PRIVATE /W4)
else()
    target_compile_options(PointDataGTest PRIVATE -Wall -Wextra -pedantic)
endif()

add_test(NAME PointDataGTest COMMAND PointDataGTest)


add_custom_command(TARGET PointDataGTest POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
    "$<TARGET_FILE:PointDataGTest>"
    "${INSTALL_DIR}/$<CONFIGURATION>/$<TARGET_FILE_NAME:PointDataGTest>"
)

