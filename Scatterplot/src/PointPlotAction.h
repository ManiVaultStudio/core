#pragma once

#include "PluginAction.h"

#include "widgets/OptionAction.h"
#include "widgets/DoubleAction.h"

class PointPlotAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, PointPlotAction* pointPlotAction);

    private:
        QHBoxLayout     _layout;
        QLabel          _pointSizelabel;
        QLabel          _pointOpacitylabel;
    };

public:
    PointPlotAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

    bool canReset() const;
    void reset();

protected:
    hdps::gui::OptionAction     _pointSizeByAction;
    hdps::gui::DoubleAction     _pointSizeAction;
    hdps::gui::OptionAction     _pointOpacityByAction;
    hdps::gui::DoubleAction     _pointOpacityAction;
    QAction                     _resetAction;

    static constexpr double DEFAULT_POINT_SIZE = 10.0;
    static constexpr double DEFAULT_POINT_OPACITY = 50.0;

    friend class Widget;
};