cmake_minimum_required(VERSION 3.17)

set(MV_PUBLIC_TEST "MV_Public_test")
PROJECT(${MV_PUBLIC_TEST})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DWIN32 /EHsc /MP /permissive- /Zc:__cplusplus")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
endif(MSVC)

# Check if the directory to the ManiVault installation has been provided
if(NOT DEFINED MV_INSTALL_DIR)
    set(MV_INSTALL_DIR "" CACHE PATH "Directory where ManiVault is installed")
    message(FATAL_ERROR "Please set MV_INSTALL_DIR to the directory where ManiVault is installed")
endif()
file(TO_CMAKE_PATH ${MV_INSTALL_DIR} MV_INSTALL_DIR)

enable_testing()

CPMAddPackage(
  NAME GTest
  GITHUB_REPOSITORY google/googletest
  VERSION 1.13.0
)

set(MV_PUBLIC_SRC_DIR ../../src)

file(GLOB_RECURSE MV_PUBLIC_TESTS "${MV_PUBLIC_SRC_DIR}/*_test.cpp")

foreach(_var ${MV_PUBLIC_SRC_DIR})
    message(STATUS "${_var}: ${${_var}}")
endforeach()

add_executable(${MV_PUBLIC_TEST} ${MV_PUBLIC_TESTS})

target_link_libraries(${MV_PUBLIC_TEST} PRIVATE ${MV_PUBLIC_LIB} GTest::gtest GTest::gtest_main)
target_link_libraries(${MV_PUBLIC_TEST} PRIVATE GTest::gtest)
target_link_libraries(${MV_PUBLIC_TEST} PRIVATE GTest::gtest_main)

#add_test(NAME ${MV_PUBLIC_TEST} COMMAND ${MV_PUBLIC_TEST})

#target_include_directories(${MV_PUBLIC_TEST} PRIVATE "${MV_INSTALL_DIR}/$<CONFIGURATION>/include/")

target_compile_features(${MV_PUBLIC_TEST} PRIVATE cxx_std_17)

install(TARGETS ${MV_PUBLIC_TEST}
	RUNTIME DESTINATION ${MV_INSTALL_DIR} COMPONENT MV_PUBLIC_TEST
)

add_custom_command(TARGET ${MV_PUBLIC_TEST} POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    --install ${CMAKE_CURRENT_BINARY_DIR}
    --config $<CONFIGURATION>
    --prefix ${MV_INSTALL_DIR}/$<CONFIGURATION>
)

