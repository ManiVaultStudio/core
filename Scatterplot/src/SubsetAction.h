#pragma once

#include "PluginAction.h"

#include "widgets/StandardAction.h"

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
    hdps::gui::StandardAction   _createSubsetAction;
    hdps::gui::StandardAction   _fromSourceDataAction;

    friend class Widget;
};