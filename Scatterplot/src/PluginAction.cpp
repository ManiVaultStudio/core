#include "PluginAction.h"

#include "ScatterplotPlugin.h"
#include "ScatterplotWidget.h"

using namespace hdps::gui;

PluginAction::PluginAction(ScatterplotPlugin* scatterplotPlugin) :
    WidgetAction(reinterpret_cast<QObject*>(scatterplotPlugin)),
    _scatterplotPlugin(scatterplotPlugin)
{
}

ScatterplotWidget* PluginAction::getScatterplotWidget()
{
    Q_ASSERT(_scatterplotPlugin != nullptr);

    return _scatterplotPlugin->getScatterplotWidget();
}