#pragma once

#include "PluginAction.h"

#include <QActionGroup>
#include <QHBoxLayout>

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

    class PopupWidget : public PluginAction::PopupWidget {
    public:
        PopupWidget(QWidget* parent, RenderModeAction* renderModeAction);

    private:
        QHBoxLayout         _layout;
    };

public:
    RenderModeAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent, const WidgetType& widgetType = WidgetType::Standard) override {
        switch (widgetType)
        {
            case WidgetType::Standard:
                return new Widget(parent, this);

            case WidgetType::Compact:
                return new CompactWidget(parent, this);

            case WidgetType::Popup:
                return new PopupWidget(parent, this);

            default:
                break;
        }

        return nullptr;
    }

    QMenu* getContextMenu();

protected:
    QAction         _scatterPlotAction;
    QAction         _densityPlotAction;
    QAction         _contourPlotAction;
    QActionGroup    _actionGroup;

    friend class Widget;
};