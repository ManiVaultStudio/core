#pragma once

#include "PluginAction.h"

#include <QActionGroup>

class SelectionAction : public PluginAction
{
public:
    class Widget : public PluginAction::Widget {
    public:
        Widget(QWidget* parent, SelectionAction* selectionAction);
    };

    class PopupWidget : public PluginAction::PopupWidget {
    public:
        PopupWidget(QWidget* parent, SelectionAction* selectionAction);
    };

public:
    SelectionAction(ScatterplotPlugin* scatterplotPlugin);

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
    hdps::gui::DecimalAction    _brushRadiusAction;
    hdps::gui::StandardAction   _modifierAddAction;
    hdps::gui::StandardAction   _modifierRemoveAction;
    QActionGroup                _modifierActionGroup;
    hdps::gui::StandardAction   _clearSelectionAction;
    hdps::gui::StandardAction   _selectAllAction;
    hdps::gui::StandardAction   _invertSelectionAction;
    hdps::gui::StandardAction   _notifyDuringSelectionAction;

    friend class Widget;
};