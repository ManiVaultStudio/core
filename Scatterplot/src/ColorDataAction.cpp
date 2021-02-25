#include "ColorDataAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

ColorDataAction::ColorDataAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Coloring"),
    _colorDataAction(this, "Color data"),
    _resetAction(this, "Reset")
{
    scatterplotPlugin->addAction(&_colorDataAction);
    scatterplotPlugin->addAction(&_resetAction);

    _resetAction.setToolTip("Reset color settings");

    _colorDataAction.setEnabled(false);
}

QMenu* ColorDataAction::getContextMenu()
{
    auto menu = new QMenu("Color data");

    menu->addAction(&_colorDataAction);
    menu->addAction(&_resetAction);

    return menu;
}

ColorDataAction::Widget::Widget(QWidget* parent, ColorDataAction* coloringAction) :
    WidgetAction::Widget(parent, coloringAction),
    _layout(),
    _colorDataWidget(this, &coloringAction->_colorDataAction)
{
    _layout.addWidget(&_colorDataWidget);

    setLayout(&_layout);
}