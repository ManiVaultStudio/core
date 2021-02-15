#pragma once

#include "PluginAction.h"

#include "widgets/StandardAction.h"

#include <QActionGroup>
#include <QHBoxLayout>
#include <QToolBar>
#include <QToolButton>

class QMenu;

class RenderModeAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, RenderModeAction* renderModeAction);

    private:
        QHBoxLayout         _layout;
    };

public:
    RenderModeAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    hdps::gui::StandardAction   _scatterPlotAction;
    hdps::gui::StandardAction   _densityPlotAction;
    hdps::gui::StandardAction   _contourPlotAction;
    QActionGroup                _actionGroup;

    friend class Widget;
};