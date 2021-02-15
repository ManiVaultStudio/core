#pragma once

#include "PluginAction.h"

#include "widgets/StandardAction.h"
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
    hdps::gui::StandardAction   _rectangleAction;
    hdps::gui::StandardAction   _brushAction;
    hdps::gui::StandardAction   _lassoAction;
    hdps::gui::StandardAction   _polygonAction;
    QActionGroup                _typeActionGroup;
    hdps::gui::DoubleAction     _brushRadiusAction;
    hdps::gui::StandardAction   _modifierAddAction;
    hdps::gui::StandardAction   _modifierRemoveAction;
    hdps::gui::StandardAction   _clearSelectionAction;
    hdps::gui::StandardAction   _selectAllAction;
    hdps::gui::StandardAction   _invertSelectionAction;
    hdps::gui::StandardAction   _notifyDuringSelectionAction;

    friend class Widget;
};