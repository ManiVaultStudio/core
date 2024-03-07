# -----------------------------------------------------------------------------
# Source files for target MV_EXE
# -----------------------------------------------------------------------------
# defines MAIN_SOURCES

set(MAIN_SOURCES
    src/Main.cpp
    res/styles/default.qss
)

if(WIN32)
    set(MAIN_SOURCES ${MAIN_SOURCES} res/ResourcesWinIco.rc)
endif(WIN32)

if(APPLE)
    set(MAIN_SOURCES ${MAIN_SOURCES} res/icons/AppIcon.icns)
endif(APPLE)
