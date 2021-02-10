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
    };

public:
    SubsetAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    QAction     _createSubsetAction;
    QAction     _fromSourceDataAction;

    friend class Widget;
};