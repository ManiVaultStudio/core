# -----------------------------------------------------------------------------
# Set global build options for ManiVault projects
# -----------------------------------------------------------------------------
# Several (compiler-dependent) CMake and C++ flags should be 
# consistent across core and plugins and only need to be set once.
#
# Usage:
#  mv_project_defaults()

function(mv_project_defaults)

    # -----------------------------------------------------------------------------
    # Enable PDB files in Release builds (Visual Studio)
    # -----------------------------------------------------------------------------

    option(MV_RELWITHDEBUGINFO "Generate program database (PDB) files with debug information in Release builds" OFF)
    if(MV_RELWITHDEBUGINFO AND CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        if(NOT CMAKE_CXX_FLAGS_RELEASE MATCHES "/Zi") # avoid keep appending flags every configure run 
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi /Zf" CACHE STRING "" FORCE)
            set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG" CACHE STRING "" FORCE)

            if(TARGET MV_Application)
                set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG" CACHE STRING "" FORCE)
            endif()
        endif()
    endif()


    # Guard: only apply global options once, even if included multiple times
    if(MV_PROJECT_DEFAULTS_APPLIED)
        return()
    endif()

    # -----------------------------------------------------------------------------
    # MSVC settings
    # -----------------------------------------------------------------------------
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)

        # /DWIN32 and /EHsc are already the defaults, see https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Platform/Windows-MSVC.cmake
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /MP /Zc:__cplusplus /Zc:preprocessor" CACHE STRING "" FORCE)
    endif()

    # -----------------------------------------------------------------------------
    # Apple settings
    # -----------------------------------------------------------------------------
    if(APPLE)
        set(CMAKE_XCODE_GENERATE_SCHEME ON CACHE BOOL "" FORCE)

        # macOS-specific RPATH
        set(MV_INSTALL_RPATH "@loader_path" INTERNAL)
    endif()

    # -----------------------------------------------------------------------------
    # Other Unix settings
    # -----------------------------------------------------------------------------
    if(UNIX AND NOT APPLE)
        set(MV_INSTALL_RPATH "${MV_INSTALL_DIR}/$<CONFIGURATION>/lib" INTERNAL)
    endif()

    # -----------------------------------------------------------------------------
    # Common Unix settings
    # -----------------------------------------------------------------------------
    if(UNIX)
        # use, i.e. don't skip, the full RPATH for the build tree
        set(CMAKE_SKIP_BUILD_RPATH FALSE CACHE BOOL "" FORCE)
        # when building, don't use the install RPATH already
        # (but later on when installing)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
        set(CMAKE_INSTALL_RPATH "${MV_INSTALL_RPATH}" CACHE BOOL "" FORCE)
        # add the automatically determined parts of the RPATH
        # which point to directories outside the build tree to the install RPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE CACHE BOOL "" FORCE)
    endif()

    # -----------------------------------------------------------------------------
    # Set guard variable
    # -----------------------------------------------------------------------------
    set(MV_PROJECT_DEFAULTS_APPLIED TRUE CACHE INTERNAL "Whether ManiVault project defaults are applied")

endfunction()
