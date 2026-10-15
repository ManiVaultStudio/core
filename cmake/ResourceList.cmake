# -----------------------------------------------------------------------------
# Resource files
# -----------------------------------------------------------------------------
# defines RESOURCE_FILES, MAIN_APP_ICON_RESOURCE, SHADERS and MAIN_STYLES
# all file path are relative to ROOT/src

set(QRESOURCES
    ${MV_ROOT_DIR}/res/ResourcesCore.qrc
)

if(WIN32)
    set(MAIN_APP_ICON_RESOURCE ${MAIN_SOURCES} ${MV_ROOT_DIR}/res/ResourcesWinIco.rc)
    source_group(Resources FILES ${MV_ROOT_DIR}/res/ResourcesWinIco.rc)
endif(WIN32)

if(APPLE)
    set(MAIN_APP_ICON_RESOURCE ${MAIN_SOURCES} ${MV_ROOT_DIR}/res/icons/AppIcon.icns)
    set_source_files_properties(${MV_ROOT_DIR}/res/icons/AppIcon.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
endif(APPLE)

set(MAIN_STYLES
    ${MV_ROOT_DIR}/res/styles/default.qss
)

set(SHADERS
    ${MV_ROOT_DIR}/res/shaders/Color.frag
    ${MV_ROOT_DIR}/res/shaders/DensityCompute.frag
    ${MV_ROOT_DIR}/res/shaders/DensityCompute.vert
    ${MV_ROOT_DIR}/res/shaders/DensityDraw.frag
    ${MV_ROOT_DIR}/res/shaders/GradientCompute.frag
    ${MV_ROOT_DIR}/res/shaders/GradientDraw.frag
    ${MV_ROOT_DIR}/res/shaders/IsoDensityDraw.frag
    ${MV_ROOT_DIR}/res/shaders/MeanshiftCompute.frag
    ${MV_ROOT_DIR}/res/shaders/PointPlot.frag
    ${MV_ROOT_DIR}/res/shaders/PointPlot.vert
    ${MV_ROOT_DIR}/res/shaders/Quad.vert
    ${MV_ROOT_DIR}/res/shaders/SelectionBox.vert
    ${MV_ROOT_DIR}/res/shaders/Texture.frag
)

qt_add_resources(RESOURCE_FILES ${QRESOURCES})

source_group(Resources FILES ${QRESOURCES} ${MAIN_STYLES})
source_group(Resources\\Shaders FILES ${SHADERS})
