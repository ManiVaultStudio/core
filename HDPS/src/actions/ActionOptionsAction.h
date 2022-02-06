#pragma once

#include "WidgetAction.h"
#include "TriggerAction.h"

namespace hdps {

namespace gui {

/**
 * Action options action class
 *
 * Action class for interacting with action options (load/save default etc.)
 *
 * @author Thomas Kroes
 */
class ActionOptionsAction : public WidgetAction
{
public:

     /**
      * Constructor
      * @param parent Pointer to parent object
      * @param widgetAction Pointer to widget actions for the options
      */
    ActionOptionsAction(QObject* parent, WidgetAction* widgetAction);

    /**
     * Get the context menu for the action
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr) override final;

    /** Update action states */
    void updateActions();

public: // Action getters

    TriggerAction& getLoadDefaultAction() { return _loadDefaultAction; }
    TriggerAction& getSaveDefaultAction() { return _saveDefaultAction; }
    TriggerAction& getFactoryDefaultAction() { return _factoryDefaultAction; }

protected:
    WidgetAction*   _widgetAction;              /** Pointer to widget actions for the options */
    TriggerAction   _loadDefaultAction;         /** Load default action */
    TriggerAction   _saveDefaultAction;         /** Save default action */
    TriggerAction   _factoryDefaultAction;      /** Restore factory default action */
    bool            _ignoreResettableSignals;   /** Ignore (factory) resettable signals */
};

}
}
