# resource files for target HDPS_EXE

set(QRESOURCES
    res/ResourcesCore.qrc
)

if(APPLE)
    set_source_files_properties(res/icons/AppIcon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif(APPLE)

QT6_ADD_RESOURCES(RESOURCE_FILES ${QRESOURCES})

source_group(Resources FILES ${QRESOURCES})
source_group(Resources FILES res/ResourcesWinIco.rc)
source_group(Resources\\generated FILES ${UI_HEADERS})
