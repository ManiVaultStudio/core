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

public: // Event handling

    /**
     * Listens to the events of target \p object
     * @param object Target object to watch for events
     * @param event Event that occurred
     */
    bool eventFilter(QObject* object, QEvent* event) override;

protected:
    hdps::gui::OptionAction     _typeAction;
    QAction                     _rectangleAction;
    QAction                     _brushAction;
    QAction                     _lassoAction;
    QAction                     _polygonAction;
    QActionGroup                _typeActionGroup;
    hdps::gui::DoubleAction     _brushRadiusAction;
    QAction                     _modifierAddAction;
    QAction                     _modifierRemoveAction;
    QAction                     _clearSelectionAction;
    QAction                     _selectAllAction;
    QAction                     _invertSelectionAction;
    QAction                     _notifyDuringSelectionAction;

    friend class Widget;
};