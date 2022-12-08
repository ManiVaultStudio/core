#pragma once

#include "AbstractManager.h"

#include <QObject>
#include <QAbstractItemModel>

namespace hdps
{

namespace gui {
    class WidgetAction;
}

class AbstractActionsManager : public AbstractManager
{
    Q_OBJECT

public:

    /** Resets the contents of the actions manager */
    //virtual void reset() = 0;

    /**
     * Add action to the manager
     * @param action Pointer to action
     */
    virtual void addAction(gui::WidgetAction* action) = 0;

    /**
     * Remove action from the manager
     * @param action Pointer to action
     */
    virtual void removeAction(gui::WidgetAction* action) = 0;

    /**
     * Get actions model
     * @return List model for actions
     */
    virtual const QAbstractItemModel& getModel() const = 0;

    /**
     * Establish whether an action is public
     * @param action Action to test
     * @return Boolean indicating whether an action is public or not
     */
    virtual bool isActionPublic(const gui::WidgetAction* action) const = 0;

    /**
     * Establish whether an action is published as a public action
     * @param action Action to test
     * @return Boolean indicating whether an action is published as a public action or not
     */
    virtual bool isActionPublished(const gui::WidgetAction* action) const = 0;

    /**
     * Establish whether an action is connected to a public action
     * @param action Action to test
     * @return Boolean indicating whether an action is connected to a public action or not
     */
    virtual bool isActionConnected(const gui::WidgetAction* action) const = 0;
};

}
