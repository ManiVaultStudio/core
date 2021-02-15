#pragma once

#include "PluginAction.h"

#include <QHBoxLayout>

class SubsetAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, SubsetAction* subsetAction);

    private:
        QHBoxLayout         _layout;
    };

    class PopupWidget : public PluginAction::PopupWidget {
    public:
        PopupWidget(QWidget* parent, SubsetAction* subsetAction);
    };

public:
    SubsetAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QWidget* getPopupWidget(QWidget* parent) override {
        return new PopupWidget(parent, this);
    };

    QMenu* getContextMenu();

protected:
    QAction   _createSubsetAction;
    QAction   _fromSourceDataAction;

    friend class Widget;
};