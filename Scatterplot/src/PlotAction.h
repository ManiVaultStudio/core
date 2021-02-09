#pragma once

#include "widgets/WidgetAction.h"
#include "widgets/DoubleAction.h"

#include <QToolBar>
#include <QToolButton>

class ScatterplotWidget;

class PlotAction : public hdps::gui::WidgetAction
{
    Q_OBJECT

public:
    class Widget : public hdps::gui::WidgetAction::Widget {
    public:
        Widget(QWidget* parent, PlotAction* plotAction);

    private:
        QHBoxLayout                             _layout;
        QToolBar                                _toolBar;
        QToolButton                             _toolButton;
        hdps::gui::WidgetAction::PopupWidget    _popupWidget;
        QWidgetAction                           _popupWidgetAction;
    };

public:
    PlotAction(ScatterplotWidget* scatterplotWidget);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    ScatterplotWidget*          _scatterplotWidget;
    hdps::gui::DoubleAction     _pointSizeAction;
    hdps::gui::DoubleAction     _pointOpacityAction;
    hdps::gui::DoubleAction     _sigmaAction;

    friend class Widget;
};