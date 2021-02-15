#pragma once

#include "widgets/WidgetAction.h"

class ScatterplotPlugin;
class ScatterplotWidget;

class PluginAction : public hdps::gui::WidgetAction
{
public:
    PluginAction(ScatterplotPlugin* scatterplotPlugin, const QString& title);

    ScatterplotWidget* getScatterplotWidget();

protected:
    ScatterplotPlugin*  _scatterplotPlugin;
};