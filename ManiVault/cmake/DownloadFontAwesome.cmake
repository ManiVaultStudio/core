# Downloads and extract font awesome resources

set(FONT_AWESOME_VERSIONS 
    5.14.0
    6.7.2
    7.0.0
)

set(TEMP_DIR "${CMAKE_BINARY_DIR}/Font-Awesome-Resources")
file(MAKE_DIRECTORY ${TEMP_DIR})

set(FONT_AWESOME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/res/iconfonts")
file(MAKE_DIRECTORY ${FONT_AWESOME_DIR})

message(STATUS "Downloading Font Awesome resources to: ${FONT_AWESOME_DIR}")

foreach(FONT_AWESOME_VERSION IN LISTS FONT_AWESOME_VERSIONS)
    # Define the URL and local paths
    set(CURRENT_URL "https://github.com/FortAwesome/Font-Awesome/archive/refs/tags/${FONT_AWESOME_VERSION}.tar.gz")
    set(CURRENT_ARCHIVE_PATH "${TEMP_DIR}/Font-Awesome-${FONT_AWESOME_VERSION}.tar.gz")
    set(CURRENT_EXTRACTED_DIR "${TEMP_DIR}/Font-Awesome-${FONT_AWESOME_VERSION}")
    string(REGEX MATCH "^[0-9]+" FONT_AWESOME_MAJOR_VERSION "${FONT_AWESOME_VERSION}")

    if(FONT_AWESOME_VERSION STREQUAL "6.5.0")
        set(CURRENT_EXTRACTED_DIR "${CURRENT_EXTRACTED_DIR}/free")
    endif()

    # Download and extract (only if not already done)
    if(NOT EXISTS ${CURRENT_ARCHIVE_PATH})
        file(DOWNLOAD ${CURRENT_URL} ${CURRENT_ARCHIVE_PATH})
    endif()
    
    if(NOT EXISTS ${CURRENT_EXTRACTED_DIR})
        file(ARCHIVE_EXTRACT INPUT ${CURRENT_ARCHIVE_PATH} DESTINATION ${TEMP_DIR})
    endif()

    # Copy files
    file(COPY_FILE "${CURRENT_EXTRACTED_DIR}/metadata/icons.json" "${FONT_AWESOME_DIR}/FontAwesome-${FONT_AWESOME_VERSION}.json" ONLY_IF_DIFFERENT)
    file(COPY_FILE "${CURRENT_EXTRACTED_DIR}/otfs/Font Awesome ${FONT_AWESOME_MAJOR_VERSION} Brands-Regular-400.otf" "${FONT_AWESOME_DIR}/FontAwesomeBrandsRegular-${FONT_AWESOME_VERSION}.otf" ONLY_IF_DIFFERENT)
    file(COPY_FILE "${CURRENT_EXTRACTED_DIR}/otfs/Font Awesome ${FONT_AWESOME_MAJOR_VERSION} Free-Regular-400.otf" "${FONT_AWESOME_DIR}/FontAwesomeRegular-${FONT_AWESOME_VERSION}.otf" ONLY_IF_DIFFERENT)
    file(COPY_FILE "${CURRENT_EXTRACTED_DIR}/otfs/Font Awesome ${FONT_AWESOME_MAJOR_VERSION} Free-Solid-900.otf" "${FONT_AWESOME_DIR}/FontAwesomeSolid-${FONT_AWESOME_VERSION}.otf" ONLY_IF_DIFFERENT)    
endforeach()

message(STATUS "Finished downloading Font Awesome resources")
