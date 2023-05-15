# source files for target HDPS_EXE

set( MAIN_SOURCES
    src/Main.cpp
    res/styles/default.qss
)

if(WIN32)
    set(MAIN_SOURCES ${MAIN_SOURCES} res/ResourcesWinIco.rc)
endif(WIN32)

if(APPLE)
    set(MAIN_SOURCES ${MAIN_SOURCES} res/icons/AppIcon.icns)
endif(APPLE)
