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
    Q_OBJECT

public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, SelectionAction* selectionAction);

    private:
        QHBoxLayout         _layout;
        QToolBar            _toolBar;
        QToolButton         _toolButton;
        PopupWidget         _popupWidget;
        QWidgetAction       _popupWidgetAction;
    };

public:
    SelectionAction(ScatterplotPlugin* scatterplotPlugin);

    QWidget* createWidget(QWidget* parent) override {
        return new Widget(parent, this);
    }

    QMenu* getContextMenu();

protected:
    hdps::gui::OptionAction     _typeAction;
    QAction                     _brushAction;
    QAction                     _lassoAction;
    QAction                     _polygonAction;
    QAction                     _rectangleAction;
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