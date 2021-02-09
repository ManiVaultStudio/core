#pragma once

#include "PluginAction.h"

#include <QAction>
#include <QActionGroup>
#include <QHBoxLayout>
#include <QToolBar>
#include <QToolButton>

class QMenu;

class RenderModeAction : public PluginAction
{
    Q_OBJECT

public:
    class Widget : public PluginAction::Widget {
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
    RenderModeAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    QAction         _scatterPlotAction;
    QAction         _densityPlotAction;
    QAction         _contourPlotAction;
    QActionGroup    _actionGroup;

    friend class Widget;
};