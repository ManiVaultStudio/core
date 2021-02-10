#pragma once

#include "PluginAction.h"

#include "widgets/OptionAction.h"
#include "widgets/DoubleAction.h"

#include <QAction>
#include <QActionGroup>
#include <QHBoxLayout>
#include <QToolBar>
#include <QToolButton>

class SelectionAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, SelectionAction* selectionAction);

    private:
        QHBoxLayout         _layout;
    };

public:
    SelectionAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    hdps::gui::OptionAction     _typeAction;
    QAction                     _rectangleAction;
    QAction                     _brushAction;
    QAction                     _lassoAction;
    QAction                     _polygonAction;
    QActionGroup                _typeActionGroup;
    QAction                     _modifierAddAction;
    QAction                     _modifierRemoveAction;
    hdps::gui::DoubleAction     _brushRadiusAction;
    QAction                     _clearSelectionAction;
    QAction                     _selectAllAction;
    QAction                     _invertSelectionAction;
    QAction                     _notifyDuringSelectionAction;

    friend class Widget;
};