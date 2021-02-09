#pragma once

#include "widgets/WidgetAction.h"

class ScatterplotPlugin;
class ScatterplotWidget;

class PluginAction : public hdps::gui::WidgetAction
{
public:
    PluginAction(ScatterplotPlugin* scatterplotPlugin);

    ScatterplotWidget* getScatterplotWidget();

protected:
    ScatterplotPlugin*  _scatterplotPlugin;
};