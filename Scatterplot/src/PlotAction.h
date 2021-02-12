#pragma once

#include "PluginAction.h"
#include "PointPlotAction.h"
#include "DensityPlotAction.h"

class PlotAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, PlotAction* plotAction);

    private:
        QHBoxLayout     _layout;
    };

public:
    PlotAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    PointPlotAction     _pointPlotAction;
    DensityPlotAction   _densityPlotAction;

    friend class Widget;
};