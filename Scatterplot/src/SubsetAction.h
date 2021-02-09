#pragma once

#include "PluginAction.h"

#include <QHBoxLayout>
#include <QToolButton>

class SubsetAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, SubsetAction* subsetAction);

    private:
        QHBoxLayout         _layout;
        QToolButton         _toolButton;
        PopupWidget         _popupWidget;
        QWidgetAction       _popupWidgetAction;
    };

public:
    SubsetAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    QAction     _createSubsetAction;
    QAction     _createFromSourceDataAction;

    friend class Widget;
};