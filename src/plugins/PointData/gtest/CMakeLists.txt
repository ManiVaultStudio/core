
add_executable(PointDataGTest
    PointDataGTest.cpp
    PointDataIteratorGTest.cpp
    PointsGTest.cpp
)

target_include_directories(${CLUSTERDATA} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_link_libraries(PointDataGTest
    ${MV_PUBLIC_LIB}
    PointData
    Qt6::Widgets
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

