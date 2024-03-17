# -----------------------------------------------------------------------------
# Resource files MV_EXE
# -----------------------------------------------------------------------------
# defines RESOURCE_FILES and MAIN_APP_ICON_RESOURCE

set(QRESOURCES
    res/ResourcesCore.qrc
)

if(WIN32)
    set(MAIN_APP_ICON_RESOURCE ${MAIN_SOURCES} res/ResourcesWinIco.rc)
    source_group(Resources FILES res/ResourcesWinIco.rc)
endif(WIN32)

if(APPLE)
    set(MAIN_APP_ICON_RESOURCE ${MAIN_SOURCES} res/icons/AppIcon.icns)
    set_source_files_properties(res/icons/AppIcon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif(APPLE)

qt_add_resources(RESOURCE_FILES ${QRESOURCES})

source_group(Resources FILES ${QRESOURCES})
