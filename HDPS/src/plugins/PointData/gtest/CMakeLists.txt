
add_executable(PointDataGTest
    PointDataGTest.cpp
    PointDataIteratorGTest.cpp
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


install(TARGETS ${PROJECT}
    RUNTIME DESTINATION . COMPONENT EXECUTABLE
)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        --install ${CMAKE_BINARY_DIR}
        --config $<CONFIGURATION>
        --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
)

