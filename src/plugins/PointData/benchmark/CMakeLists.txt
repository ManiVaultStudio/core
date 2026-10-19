add_executable(PointDataBenchmark
    PointDataBenchmark.cpp
    ../src/tiled/PackedTileFile.cpp
    ../src/tiled/PointDataTileCache.cpp
    ../src/tiled/PointDataTiledStorage.cpp
)

configure_file(
    run_PointDataBenchmark.cmd.in
    "${CMAKE_CURRENT_BINARY_DIR}/run_PointDataBenchmark.cmd"
    @ONLY
)

configure_file(
    run_PointDataBenchmarkSweep.cmd.in
    "${CMAKE_CURRENT_BINARY_DIR}/run_PointDataBenchmarkSweep.cmd"
    @ONLY
)

target_include_directories(PointDataBenchmark PRIVATE
    "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/"
    "${CMAKE_CURRENT_BINARY_DIR}/.."
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../private"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../../external/biovault"
)

target_compile_features(PointDataBenchmark PRIVATE cxx_std_${MV_CXX_STANDARD})

target_compile_definitions(PointDataBenchmark PRIVATE POINTDATA_STATIC_DEFINE)

target_link_libraries(PointDataBenchmark PRIVATE
    ${MV_PUBLIC_LIB}
    Qt6::Core
    Qt6::Widgets
)

set_target_properties(PointDataBenchmark PROPERTIES
    FOLDER "Benchmarks/PointData"
)

add_custom_command(TARGET PointDataBenchmark POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_BINARY_DIR}/run_PointDataBenchmark.cmd"
        "$<TARGET_FILE_DIR:PointDataBenchmark>/run_PointDataBenchmark.cmd"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_BINARY_DIR}/run_PointDataBenchmarkSweep.cmd"
        "$<TARGET_FILE_DIR:PointDataBenchmark>/run_PointDataBenchmarkSweep.cmd"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/PointDataBenchmarkSweep.ps1"
        "$<TARGET_FILE_DIR:PointDataBenchmark>/PointDataBenchmarkSweep.ps1"
)

install(TARGETS PointDataBenchmark
    RUNTIME DESTINATION "$<CONFIGURATION>/benchmarks/PointData"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/run_PointDataBenchmark.cmd"
    "${CMAKE_CURRENT_BINARY_DIR}/run_PointDataBenchmarkSweep.cmd"
    "${CMAKE_CURRENT_SOURCE_DIR}/PointDataBenchmarkSweep.ps1"
    DESTINATION "$<CONFIGURATION>/benchmarks/PointData"
)

if(MSVC)
    target_compile_options(PointDataBenchmark PRIVATE /W4)
else()
    target_compile_options(PointDataBenchmark PRIVATE -Wall -Wextra -pedantic)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_property(TARGET PointDataBenchmark PROPERTY
        VS_DEBUGGER_ENVIRONMENT "PATH=$<TARGET_FILE_DIR:Core>;${QT_DIR}/../../../bin"
    )
endif()
