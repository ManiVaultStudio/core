# Downloads and sets up third party dependencies for the core

message(STATUS "Start downloading vendored core dependency libraries...")

if(MV_BUILD_TESTING)
    CPMAddPackage(
        NAME              googletest
        GITHUB_REPOSITORY google/googletest
        GIT_TAG           v1.15.2
        OPTIONS
            "INSTALL_GTEST OFF"
            "gtest_force_shared_crt ON"
    )
endif()

# File compression: zlib
CPMAddPackage(
  NAME              ZLIB
  GITHUB_REPOSITORY madler/zlib
  GIT_TAG           v1.3.2
  OPTIONS 
    "ZLIB_BUILD_EXAMPLES OFF" 
    "ZLIB_BUILD_SHARED OFF" 
    "ZLIB_BUILD_STATIC ON"
    "ZLIB_BUILD_TESTING OFF"
    "ZLIB_INSTALL OFF"
)

# Maintain build compatibility between find_package and CMakeLists.txt variants (for QuaZip)
add_library(ZLIB::ZLIB ALIAS zlibstatic)
set(ZLIB_LIBRARIES zlibstatic)
set_target_properties(zlibstatic PROPERTIES 
    POSITION_INDEPENDENT_CODE ON
    FOLDER CoreDependencies
    C_STANDARD ${CMAKE_C_STANDARD}
    C_STANDARD_REQUIRED ${CMAKE_C_STANDARD_REQUIRED}
    C_EXTENSIONS ${CMAKE_C_EXTENSIONS}
)

# File compression: QuaZip
CPMAddPackage(
  NAME              quazip
  GITHUB_REPOSITORY stachenov/quazip
  GIT_TAG           v1.7.2
  OPTIONS 
    "QUAZIP_QT_MAJOR_VERSION 6" 
    "QUAZIP_BZIP2 OFF" 
    "QUAZIP_INSTALL OFF" 
    "BUILD_SHARED_LIBS OFF"
    "QUAZIP_ENABLE_QTEXTCODEC OFF"
)

set_target_properties(QuaZip PROPERTIES 
    FOLDER CoreDependencies
    CXX_STANDARD ${CMAKE_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
    CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS}
)

# Serialization: JSON
CPMAddPackage(
  NAME              nlohmann_json
  GITHUB_REPOSITORY	nlohmann/json
  GIT_TAG           v3.12.0
  DOWNLOAD_ONLY     YES
  OPTIONS
    "JSON_BuildTests=OFF"            # skip upstream tests
    "JSON_MultipleHeaders=ON"        # shave compile time (optional)
)

# Serialization: Add valijson as a header-only library
CPMAddPackage(
  NAME              valijson
  GITHUB_REPOSITORY tristanpenman/valijson
  GIT_TAG           v1.1.0
  #GIT_SUBMODULES   ""                  # Does not yet work with CPM, see workaround from https://github.com/cpm-cmake/CPM.cmake/pull/461#issuecomment-1665044141
  GIT_SUBMODULES    "doc"               # list an existing subdir from the repo to not download any submodules
  DOWNLOAD_ONLY     YES
  OPTIONS 
    "VALIJSON_BUILD_TESTS OFF" 
    "VALIJSON_USE_EXCEPTIONS ON"
)

# Layout: Qt Advanced docking system
CPMAddPackage(
  NAME              advanced_docking
  GITHUB_REPOSITORY githubuser0xFFFF/Qt-Advanced-Docking-System
  GIT_TAG           5.0.0
  OPTIONS 
    "BUILD_EXAMPLES OFF"
)

set_target_properties(qtadvanceddocking-qt6 PROPERTIES 
    PROJECT_LABEL "qtadvanceddocking" 
    OUTPUT_NAME "qtadvanceddocking"
    FOLDER CoreDependencies
    UNITY_BUILD ${MV_UNITY_BUILD}
    CXX_STANDARD ${CMAKE_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
    CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS}
)

# bfloat16 implementation for pre c++23
CPMAddPackage(
  NAME              biovault_bfloat16
  GITHUB_REPOSITORY biovault/biovault_bfloat16
  GIT_TAG           v1.0.2
  DOWNLOAD_ONLY     YES
  EXCLUDE_FROM_ALL  YES
  SOURCE_DIR        ${MV_ROOT_DIR}/external/biovault/biovault_bfloat16/
)

CPMAddPackage(
    NAME zstd
    GITHUB_REPOSITORY facebook/zstd
    GIT_TAG v1.5.7
    SOURCE_SUBDIR build/cmake
    EXCLUDE_FROM_ALL                # exlcude the clean-all & uninstall targets, libzstd_static will still be built as it is linked to
    OPTIONS
        "ZSTD_BUILD_PROGRAMS OFF"
        "ZSTD_BUILD_TESTS OFF"
        "ZSTD_BUILD_CONTRIB OFF"
        "ZSTD_BUILD_SHARED OFF"
        "ZSTD_BUILD_STATIC ON"
)

set_target_properties(libzstd_static PROPERTIES 
    POSITION_INDEPENDENT_CODE ON
    FOLDER CoreDependencies
    C_STANDARD ${CMAKE_C_STANDARD}
    C_STANDARD_REQUIRED ${CMAKE_C_STANDARD_REQUIRED}
    C_EXTENSIONS ${CMAKE_C_EXTENSIONS}
)

set_target_properties(clean-all PROPERTIES 
    FOLDER CoreDependencies
)
set_target_properties(uninstall PROPERTIES 
    FOLDER CoreDependencies
)

CPMAddPackage(
    NAME Taskflow
    GITHUB_REPOSITORY taskflow/taskflow
    GIT_TAG v4.1.0
    OPTIONS
        "TF_BUILD_BENCHMARKS OFF"
        "TF_BUILD_CUDA OFF"
        "TF_BUILD_EXAMPLES OFF"
        "TF_BUILD_PROFILER OFF"
        "TF_BUILD_TESTS OFF"
        "TF_BUILD_MODULES OFF"
)

# Use of Sentry on MacOS breaks the build so turn it off for now...
if(MV_USE_ERROR_LOGGING)

    # Add Sentry with CPM
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(SENTRY_TRANSPORT "winhttp")     # Use the default HTTP transport
    else()
        set(SENTRY_TRANSPORT "curl")        # Use the curl HTTP transport
    endif()

    CPMAddPackage(
        NAME                sentry-native
        GITHUB_REPOSITORY   getsentry/sentry-native
        GIT_TAG             0.15.2
        EXCLUDE_FROM_ALL    YES
        OPTIONS
            "SENTRY_BACKEND crashpad"										# Use Crashpad for crash reporting
            "SENTRY_TRANSPORT ${SENTRY_TRANSPORT}"
            "SENTRY_BUILD_SHARED_LIBS OFF"
            "CRASHPAD_ENABLE_INSTALL OFF"
            "CRASHPAD_ENABLE_INSTALL_DEV OFF"
    )

    set_target_properties(sentry mini_chromium
        PROPERTIES
        FOLDER CoreDependencies
        C_STANDARD ${CMAKE_C_STANDARD}
        C_STANDARD_REQUIRED ${CMAKE_C_STANDARD_REQUIRED}
        C_EXTENSIONS ${CMAKE_C_EXTENSIONS}
        CXX_STANDARD ${CMAKE_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
        CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS}
    )
    
    set(CRASHPAD_LIBS crashpad_client crashpad_compat crashpad_getopt crashpad_handler crashpad_handler_lib crashpad_minidump crashpad_mpack crashpad_snapshot crashpad_tools crashpad_util crashpad_zlib)
    
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        list(APPEND CRASHPAD_LIBS crashpad_wer)
    endif()
    
    set_target_properties(${CRASHPAD_LIBS}
        PROPERTIES
        FOLDER CoreDependencies/Crashpad
        CXX_STANDARD ${CMAKE_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
        CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS}
    )

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_definitions(sentry PRIVATE
            _GNU_SOURCE
        )
        # override settings for sentry
        set_target_properties(sentry
            PROPERTIES
            C_EXTENSIONS ON
            CXX_EXTENSIONS ON
        )
    endif()

    # No linking optimization for MSVC builds with error logging (linking optimization screws up the program database which prohibits symbolication)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # Remove /OPT:ICF from the linker flags
        string(REPLACE "/OPT:ICF" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
        string(REPLACE "/OPT:ICF" "" CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}")
        string(REPLACE "/OPT:ICF" "" CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS}")
    endif()

endif()

if(MV_ENABLE_CPPTRACE)

    set(CPPTRACE_OPTIONS
        "CPPTRACE_BUILD_BENCHMARKING OFF"
        "CPPTRACE_BUILD_TOOLS OFF"
        "CPPTRACE_PROVIDE_EXPORT_SET OFF"
    )

    # do not use the default backend libdwarf
    if(UNIX AND NOT APPLE)
        list(APPEND CPPTRACE_OPTIONS "CPPTRACE_GET_SYMBOLS_WITH_LIBBACKTRACE ON")
    elseif(APPLE)
        list(APPEND CPPTRACE_OPTIONS "CPPTRACE_GET_SYMBOLS_WITH_ADDR2LINE ON")
    endif()

    CPMAddPackage(
        NAME cpptrace
        GITHUB_REPOSITORY jeremy-rifkin/cpptrace
        GIT_TAG v1.0.4
        OPTIONS ${CPPTRACE_OPTIONS}
    )

    set_target_properties(cpptrace-lib 
        PROPERTIES
        FOLDER CoreDependencies
    )

endif()

message(STATUS "Finished downloading vendored core dependency libraries.")
