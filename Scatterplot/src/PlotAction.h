#pragma once

#include "PluginAction.h"

#include "widgets/DoubleAction.h"

class PlotAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, PlotAction* plotAction);

    private:
        QHBoxLayout _layout;
    };

public:
    PlotAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    hdps::gui::DoubleAction     _pointSizeAction;
    hdps::gui::DoubleAction     _pointOpacityAction;
    hdps::gui::DoubleAction     _sigmaAction;
    QAction                     _resetAction;

    static constexpr double DEFAULT_POINT_SIZE = 10.0;
    static constexpr double DEFAULT_POINT_OPACITY = 50.0;
    static constexpr double DEFAULT_SIGMA = 25.0;

    friend class Widget;
};