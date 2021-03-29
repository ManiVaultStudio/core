#include "ColorDataAction.h"
#include "Application.h"

#include "ScatterplotPlugin.h"

#include <QMenu>

using namespace hdps::gui;

ColorDataAction::ColorDataAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin, "Coloring"),
    _datasetNameAction(this, "Color dataset name")
{
    scatterplotPlugin->addAction(&_datasetNameAction);

    _datasetNameAction.setEnabled(false);
}

QMenu* ColorDataAction::getContextMenu()
{
    auto menu = new QMenu("Color data");

    menu->addAction(&_datasetNameAction);

    return menu;
}

ColorDataAction::Widget::Widget(QWidget* parent, ColorDataAction* coloringAction) :
    WidgetAction::Widget(parent, coloringAction),
    _layout()
{
    _layout.addWidget(new StringAction::Widget(this, &coloringAction->_datasetNameAction, false));

    setLayout(&_layout);
}

ColorDataAction::PopupWidget::PopupWidget(QWidget* parent, ColorDataAction* coloringAction) :
    WidgetAction::PopupWidget(parent, coloringAction),
    _layout()
{
    _layout.addWidget(new StringAction::Widget(this, &coloringAction->_datasetNameAction));

    setLayout(&_layout);
}