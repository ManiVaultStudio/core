
add_executable(PointDataGTest
    PointDataGTest.cpp
)

target_link_libraries(PointDataGTest
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
