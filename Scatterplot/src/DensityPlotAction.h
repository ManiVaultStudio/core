#pragma once

#include "PluginAction.h"

#include "widgets/OptionAction.h"
#include "widgets/DoubleAction.h"

#include <QLabel>

class DensityPlotAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, DensityPlotAction* densityPlotAction);

    private:
        QHBoxLayout     _layout;
        QLabel          _sigmaLabel;
    };

public:
    DensityPlotAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

    bool canReset() const;
    void reset();

protected:
    hdps::gui::DoubleAction     _sigmaAction;
    QAction                     _resetAction;

    static constexpr double DEFAULT_SIGMA = 25.0;

    friend class Widget;
};