#include "SubsetAction.h"

#include "ScatterplotPlugin.h"

#include "widgets/ActionPushButton.h"
#include "widgets/ActionCheckBox.h"

using namespace hdps::gui;

SubsetAction::SubsetAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _createSubsetAction("Create subset"),
    _fromSourceDataAction("From source data")
{
    _createSubsetAction.setToolTip("Create subset from selected data points");
    _fromSourceDataAction.setToolTip("Create subset from source data");

    const auto updateActions = [this]() -> void {
        const auto canCreateSubset = _scatterplotPlugin->getNumberOfSelectedPoints() >= 1;

        _createSubsetAction.setEnabled(canCreateSubset);
        _fromSourceDataAction.setEnabled(canCreateSubset);
    };

    connect(_scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), this, updateActions);

    updateActions();
}

QMenu* SubsetAction::getContextMenu()
{
    auto menu = new QMenu("Subset");

    menu->addAction(&_createSubsetAction);
    menu->addAction(&_fromSourceDataAction);

    return menu;
}

SubsetAction::Widget::Widget(QWidget* parent, SubsetAction* subsetAction) :
    WidgetAction::Widget(parent, subsetAction),
    _layout()
{
    _layout.addWidget(new ActionPushButton(&subsetAction->_createSubsetAction));
    _layout.addWidget(new ActionCheckBox(&subsetAction->_fromSourceDataAction));

    setLayout(&_layout);
}