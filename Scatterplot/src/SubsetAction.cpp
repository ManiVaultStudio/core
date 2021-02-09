#include "SubsetAction.h"

#include "ScatterplotPlugin.h"

#include "widgets/ActionPushButton.h"

using namespace hdps::gui;

SubsetAction::SubsetAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _createSubsetAction("Create subset"),
    _createFromSourceDataAction("From source data")
{
    _createSubsetAction.setToolTip("Create subset from selected data points");
    _createFromSourceDataAction.setToolTip("Create subset from source data");

    const auto updateActions = [this]() -> void {
        const auto canCreateSubset = _scatterplotPlugin->getNumberOfSelectedPoints() >= 1;

        _createSubsetAction.setEnabled(canCreateSubset);
        _createFromSourceDataAction.setEnabled(canCreateSubset);
    };

    connect(_scatterplotPlugin, qOverload<>(&ScatterplotPlugin::selectionChanged), this, updateActions);

    updateActions();
}

QMenu* SubsetAction::getContextMenu()
{
    auto menu = new QMenu("Subset");

    menu->addAction(&_createSubsetAction);
    menu->addAction(&_createFromSourceDataAction);

    return menu;
}

SubsetAction::Widget::Widget(QWidget* parent, SubsetAction* subsetAction) :
    WidgetAction::Widget(parent, subsetAction),
    _layout(),
    _toolButton(),
    _popupWidget(this, "Subset"),
    _popupWidgetAction(this)
{
    _layout.addWidget(&_toolButton);
    
    _popupWidgetAction.setDefaultWidget(&_popupWidget);

    auto popupWidgetLayout = new QVBoxLayout();

    popupWidgetLayout->addWidget(new ActionPushButton(&subsetAction->_createSubsetAction));

    _popupWidget.setContentLayout(popupWidgetLayout);

    _toolButton.setPopupMode(QToolButton::InstantPopup);
    _toolButton.addAction(&_popupWidgetAction);
    

    setLayout(&_layout);
}