#ifndef HDPS_SHARED_ACTIONS_H
#define HDPS_SHARED_ACTIONS_H

#include "actions/WidgetAction.h"

namespace hdps {

namespace gui {

/**
 * Shared action mixin class
 *
 * @author Thomas Kroes
 */
class SharedActions
{
public:

    /** List of widget actions */
    using WidgetActionList = QList<gui::WidgetAction*>;

public:

    /**
     * Exposes a widget action
     * This class does not alter the ownership of the allocated widget action
     * @param widgetAction Widget action to expose
     */
    void exposeAction(gui::WidgetAction* widgetAction);

    /**
     * Conceals an action widget
     * This does not de-allocate the widget action memory
     * @param widgetAction Widget action to remove
     */
    void concealAction(gui::WidgetAction* widgetAction);

    /** Returns list of shared action widgets*/
    const WidgetActionList& getExposedActions() const;

    /**
     * Get action by name
     * @param name Name of the action
     * @return Pointer to action (if any)
     */
    gui::WidgetAction* getActionByName(const QString& name);

    /**
     * Populates existing menu with exposed actions menus
     * @param contextMenu Context menu to populate
     */
    void populateContextMenu(QMenu* contextMenu);

    /**
     * Gets context menu for the exposed actions
     * @param targetMenu Target menu to insert the exposed action menus
     * @return Context menu
     */
    QMenu* getContextMenu();

protected:
    WidgetActionList    _actions;     /** Exposed widget actions */
};

}
}

#endif // HDPS_SHARED_ACTIONS_H