# -----------------------------------------------------------------------------
# Resource files
# -----------------------------------------------------------------------------
# defines RESOURCE_FILES, MAIN_APP_ICON_RESOURCE, SHADERS and MAIN_STYLES

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

set(MAIN_STYLES
    res/styles/default.qss
)

set(SHADERS
    res/shaders/Color.frag
    res/shaders/DensityCompute.frag
    res/shaders/DensityCompute.vert
    res/shaders/DensityDraw.frag
    res/shaders/GradientCompute.frag
    res/shaders/GradientDraw.frag
    res/shaders/IsoDensityDraw.frag
    res/shaders/MeanshiftCompute.frag
    res/shaders/PointPlot.frag
    res/shaders/PointPlot.vert
    res/shaders/Quad.vert
    res/shaders/SelectionBox.vert
    res/shaders/Texture.frag
)

qt_add_resources(RESOURCE_FILES ${QRESOURCES})

source_group(Resources FILES ${QRESOURCES} ${MAIN_STYLES})
source_group(Resources\\Shaders FILES ${SHADERS})
