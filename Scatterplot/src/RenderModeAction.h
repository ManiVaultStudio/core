#pragma once

#include "widgets/WidgetAction.h"

#include <QAction>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QToolBar>
#include <QToolButton>

class ScatterplotWidget;

class QMenu;

class RenderModeAction : public hdps::gui::WidgetAction
{
    Q_OBJECT

public:
    class Widget : public hdps::gui::WidgetAction::Widget {
    public:
        Widget(QWidget* parent, RenderModeAction* renderModeAction);

    private:
        QHBoxLayout         _layout;
        QToolBar            _toolBar;
        QToolButton         _toolButton;
        PopupWidget         _popupWidget;
        QWidgetAction       _popupWidgetAction;
    };

public:
    RenderModeAction(ScatterplotWidget* scatterplotWidget);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    QAction     _scatterPlotAction;
    QAction     _densityPlotAction;
    QAction     _contourPlotAction;

    friend class Widget;
};