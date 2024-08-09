function(mv_set_version)
    
    # Read version number and suffix from VERSION file in top level dir
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" VERSION_CONTENTS)
    
    string(REGEX MATCH "MAJOR[ \t]+([0-9]+)" _ ${VERSION_CONTENTS})
    set(MV_VERSION_MAJOR "${CMAKE_MATCH_1}")
    string(REGEX MATCH "MINOR[ \t]+([0-9]+)" _ ${VERSION_CONTENTS})
    set(MV_VERSION_MINOR "${CMAKE_MATCH_1}")
    string(REGEX MATCH "PATCH[ \t]+([0-9]+)" _ ${VERSION_CONTENTS})
    set(MV_VERSION_PATCH "${CMAKE_MATCH_1}")
    string(REGEX MATCH "SUFFIX[ \t]+\"([^\"]+)\"" _ ${VERSION_CONTENTS})
    set(MV_VERSION_SUFFIX "${CMAKE_MATCH_1}")
    
    # Set helper variables
    set(VERSION_NUMERIC "${MV_VERSION_MAJOR}.${MV_VERSION_MINOR}.${MV_VERSION_PATCH}")
    set(MV_VERSION "${VERSION_NUMERIC}" PARENT_SCOPE)
    set(MV_VERSION_FULL "${VERSION_NUMERIC}" PARENT_SCOPE)
    
    # Only use suffix in version variable if non-empty
    if(NOT "${MV_VERSION_SUFFIX}" STREQUAL "")
        set(MV_VERSION_FULL "${VERSION_NUMERIC}${MV_VERSION_SUFFIX}" PARENT_SCOPE)
    endif()

    # Calculate the length of the string
    string(LENGTH "${MV_VERSION_SUFFIX}" PROJECT_VERSION_SUFFIX_LENGTH)
    set(PROJECT_VERSION_SUFFIX_LENGTH "${PROJECT_VERSION_SUFFIX_LENGTH}" PARENT_SCOPE)

    # Prepare the suffix content for array of char, i.e. turn "rc1" into 'r', 'c', '1', '\0'
    string(REGEX REPLACE "(.)" "'\\1', " VERSION_SUFFIX_ELEMENTS "${MV_VERSION_SUFFIX}")
    set(PROJECT_VERSION_SUFFIX "${VERSION_SUFFIX_ELEMENTS}" PARENT_SCOPE)
    
endfunction()