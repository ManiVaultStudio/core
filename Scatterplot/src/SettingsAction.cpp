#include "SettingsAction.h"

#include "ScatterplotPlugin.h"

using namespace hdps::gui;

SettingsAction::SettingsAction(ScatterplotPlugin* scatterplotPlugin) :
    PluginAction(scatterplotPlugin),
    _renderModeAction(scatterplotPlugin),
    _plotAction(scatterplotPlugin),
    _positionAction(scatterplotPlugin),
    _colorAction(scatterplotPlugin),
    _subsetAction(scatterplotPlugin),
    _selectionAction(scatterplotPlugin)
{
    const auto updateEnabled = [this]() {
        setEnabled(!_scatterplotPlugin->getCurrentDataset().isEmpty());
    };

    connect(scatterplotPlugin, &ScatterplotPlugin::currentDatasetChanged, this, [this, updateEnabled](const QString& datasetName) {
        updateEnabled();
    });

    updateEnabled();
}

QMenu* SettingsAction::getContextMenu()
{
    auto menu = new QMenu();

    menu->addMenu(_renderModeAction.getContextMenu());
    menu->addMenu(_plotAction.getContextMenu());
    menu->addSeparator();
    menu->addMenu(_positionAction.getContextMenu());
    menu->addMenu(_colorAction.getContextMenu());
    menu->addSeparator();
    menu->addMenu(_subsetAction.getContextMenu());
    menu->addMenu(_selectionAction.getContextMenu());

    return menu;
}

SettingsAction::Widget::Widget(QWidget* parent, SettingsAction* settingsAction) :
    WidgetAction::Widget(parent, settingsAction),
    _layout()
{
    setAutoFillBackground(true);

    _layout.setMargin(4);
    _layout.setMargin(0);

    _layout.addWidget(settingsAction->_renderModeAction.createWidget(this));
    _layout.addWidget(settingsAction->_plotAction.createWidget(this));
    _layout.addWidget(settingsAction->_positionAction.createWidget(this));
    _layout.addWidget(settingsAction->_colorAction.createWidget(this));
    _layout.addWidget(settingsAction->_subsetAction.createWidget(this));
    _layout.addWidget(settingsAction->_selectionAction.createWidget(this));
    _layout.addStretch(1);

    setLayout(&_layout);
}